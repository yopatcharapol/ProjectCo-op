/*
 * ======================================================================================
 * 📡 EXP-04: ESP32-Nano Omni-Frame Wi-Fi Sniffer (All 802.11 RF Radiations)
 * ======================================================================================
 * โครงการวิจัยและพัฒนาสหกิจศึกษา (Project Co-op)
 * 
 * เป้าหมายหลัก:
 * ดักจับ "คลื่นวิทยุทุกชนิดที่หลุดรอดออกมาจากสมาร์ตโฟน" ในย่าน 2.4 GHz โดยไม่จำกัดแค่ Probe Request:
 * 1. เฟรม Management:
 *    - Probe Request (0x04) -> โทรศัพท์สแกนหา AP แบบไร้สาย (แม้ไม่ต่อเน็ต)
 *    - Action Frame (0x0D)  -> Wi-Fi Direct, Quick Share P2P, Miracast Screen Mirroring
 *    - Beacon Frame (0x08)  -> โทรศัพท์เปิด Personal Hotspot
 *    - Auth/Assoc (0x00, 0x0B) -> โทรศัพท์กำลังพยายามเชื่อมต่อ
 * 2. เฟรม Data:
 *    - QoS Null Frame (0x0C) -> พัลส์ Keep-Alive / Power-Save ขณะต่อ Wi-Fi หรือจอดับ
 *    - Null Function (0x04)  -> พัลส์สแตนด์บายสลับโหมดพลังงาน
 *    - QoS Data (0x08)       -> แพ็กเก็ตข้อมูลแอปพลิเคชันพื้นหลัง (Notification/Sync)
 * 
 * ฮาร์ดแวร์เป้าหมาย: Arduino Nano ESP32 / ESP32-S3 / ESP32 DevKit
 * Baud Rate: 115200
 * ======================================================================================
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <WiFi.h>

// ช่องสัญญาณหลักที่จะทำการสลับ (Channel Hopping 1, 6, 11)
const uint8_t CHANNELS[] = {1, 6, 11};
const int NUM_CHANNELS = 3;
const unsigned long HOP_INTERVAL_MS = 300; // สลับช่องทุก 300ms เพื่อความครอบคลุมสูง

unsigned long lastHopTime = 0;
int currentChannelIndex = 0;
uint32_t packetCount = 0;

// ตัวนับสถิติแยกตามประเภทคลื่น
uint32_t countProbeReq = 0;
uint32_t countActionP2P = 0;
uint32_t countQoSNull = 0;
uint32_t countHotspotBeacon = 0;
uint32_t countData = 0;
uint32_t countOther = 0;

unsigned long lastPacketTime = 0;

// โครงสร้างส่วนหัวมาตรฐาน 802.11 MAC Header (24 ไบต์)
typedef struct {
    uint16_t frame_ctrl; // Control Frame (Type & Subtype, Flags)
    uint16_t duration;
    uint8_t addr1[6];    // Receiver Address (RA / DA)
    uint8_t addr2[6];    // Transmitter Address (TA / SA) -> MAC ของเครื่องที่ยิง!
    uint8_t addr3[6];    // Filtering / BSSID
    uint16_t seq_ctrl;   // 12-bit Sequence Number + 4-bit Fragment
} __attribute__((packed)) wifi_ieee80211_mac_header_t;

// ฟังก์ชันตรวจสอบว่าเป็น MAC สุ่มหรือไม่ (Locally Administered Bit)
bool isRandomMac(const uint8_t* mac) {
    return (mac[0] & 0x02) != 0;
}

// ตรวจสอบชื่อ Subtype ภาษาอังกฤษสั้นๆ
const char* getSubtypeName(uint8_t fType, uint8_t fSubtype) {
    if (fType == 0) { // Management
        switch (fSubtype) {
            case 0: return "ASSOC_REQ";
            case 1: return "ASSOC_RESP";
            case 2: return "REASSOC_REQ";
            case 4: return "PROBE_REQ";
            case 5: return "PROBE_RESP";
            case 8: return "BEACON";
            case 10: return "DISASSOC";
            case 11: return "AUTH";
            case 12: return "DEAUTH";
            case 13: return "ACTION_P2P";
            default: return "MGMT_OTHER";
        }
    } else if (fType == 2) { // Data
        switch (fSubtype) {
            case 0: return "DATA";
            case 4: return "NULL_DATA";
            case 8: return "QOS_DATA";
            case 12: return "QOS_NULL";
            default: return "DATA_OTHER";
        }
    }
    return "UNKNOWN";
}

// Callback ดักจับแพ็กเก็ตดิบจากอากาศ (Omni Promiscuous Callback)
void IRAM_ATTR wifiOmniCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    // เรารับทั้ง Management และ Data
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;

    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    int len = pkt->rx_ctrl.sig_len;
    uint8_t* payload = pkt->payload;

    if (len < sizeof(wifi_ieee80211_mac_header_t)) return;

    wifi_ieee80211_mac_header_t* header = (wifi_ieee80211_mac_header_t*)payload;
    uint16_t fc = header->frame_ctrl;

    uint8_t fType = (fc >> 2) & 0x03;
    uint8_t fSubtype = (fc >> 4) & 0x0F;
    bool toDs = (fc & 0x0100) != 0;
    bool fromDs = (fc & 0x0200) != 0;

    // กรองขยะ AP: ถ้าเป็น Data เฟรมที่ยิงมาจาก AP ส่งหา Client (FromDS=1, ToDS=0) เราข้ามได้
    // แต่ถ้า ToDS=1 (Client กำลังยิงหา AP) หรือ ToDS=0 & FromDS=0 (P2P/Direct) นี่คือโทรศัพท์แน่นอน!
    if (fType == 2 && fromDs && !toDs) {
        return; // ตัด Downlink จาก Access Point ออก เพื่อลดสัญญาณรบกวนจากเราเตอร์รอบข้าง
    }

    // ถ้าเป็น Beacon ที่เป็น Real MAC ทั่วไปและไม่ใช่สุ่ม อาจเป็นเราเตอร์บ้าน ยกเว้นคนเปิด Hotspot
    // เพื่อไม่ให้จอรก เราจะเน้น Probe Req, Action P2P, QoS Null, และเฟรมที่ส่งจาก Client (ToDS=1)
    if (fType == 0 && fSubtype == 8 && !isRandomMac(header->addr2)) {
        return; // ตัดเราเตอร์บ้านทั่วไปออก
    }

    packetCount++;
    unsigned long now = millis();
    unsigned long deltaT = (lastPacketTime == 0) ? 0 : (now - lastPacketTime);
    lastPacketTime = now;

    int rssi = pkt->rx_ctrl.rssi;
    uint16_t seqNum = (header->seq_ctrl >> 4) & 0x0FFF;
    bool randomMac = isRandomMac(header->addr2);

    // จำแนกหมวดหมู่คลื่น
    const char* category = "OTHER";
    if (fType == 0 && fSubtype == 4) {
        category = "PROBE_REQ";
        countProbeReq++;
    } else if (fType == 0 && fSubtype == 13) {
        category = "ACTION_P2P";
        countActionP2P++;
    } else if (fType == 2 && (fSubtype == 12 || fSubtype == 4)) {
        category = "QOS_NULL";
        countQoSNull++;
    } else if (fType == 0 && fSubtype == 8) {
        category = "HOTSPOT_AP";
        countHotspotBeacon++;
    } else if (fType == 2) {
        category = "DATA_UPLINK";
        countData++;
    } else {
        countOther++;
    }

    // สกัด SSID (กรณี Probe Request หรือ Beacon)
    char ssid[33] = "";
    int ssidLen = 0;
    if (fType == 0 && (fSubtype == 4 || fSubtype == 8)) {
        int offset = sizeof(wifi_ieee80211_mac_header_t);
        if (offset + 2 <= len) {
            uint8_t tagId = payload[offset];
            uint8_t tagLen = payload[offset + 1];
            if (tagId == 0 && tagLen > 0 && tagLen <= 32 && (offset + 2 + tagLen <= len)) {
                memcpy(ssid, &payload[offset + 2], tagLen);
                ssid[tagLen] = '\0';
                ssidLen = tagLen;
            }
        }
    }

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             header->addr2[0], header->addr2[1], header->addr2[2],
             header->addr2[3], header->addr2[4], header->addr2[5]);

    // พิมพ์ข้อมูลรูปแบบ CSV
    // Format: CSV,timestamp_ms,pkt_id,channel,rssi,delta_t,seq_num,mac,mac_type,category,subtype,to_ds,ssid
    Serial.print("CSV,");
    Serial.print(now);
    Serial.print(",");
    Serial.print(packetCount);
    Serial.print(",");
    Serial.print(pkt->rx_ctrl.channel);
    Serial.print(",");
    Serial.print(rssi);
    Serial.print(",");
    Serial.print(deltaT);
    Serial.print(",");
    Serial.print(seqNum);
    Serial.print(",");
    Serial.print(macStr);
    Serial.print(",");
    Serial.print(randomMac ? "RANDOM" : "REAL");
    Serial.print(",");
    Serial.print(category);
    Serial.print(",");
    Serial.print(getSubtypeName(fType, fSubtype));
    Serial.print(",");
    Serial.print(toDs ? "TO_DS" : "DIRECT");
    Serial.print(",\"");
    Serial.print(ssidLen == 0 ? "Wildcard" : ssid);
    Serial.println("\"");
}

void setup() {
    Serial.begin(115200);

    // หน่วงเวลารอ Serial Monitor (สำหรับ Native USB CDC บน ESP32-S3 / Nano ESP32)
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 3000)) {
        delay(10);
    }

    Serial.println("\n================================================================================");
    Serial.println("📡 EXP-04: ESP32-Nano Omni-Frame Wi-Fi Sniffer (Management + Data + P2P)");
    Serial.println("CSV Header: timestamp_ms,pkt_id,channel,rssi_dbm,delta_t_ms,seq_num,mac,mac_type,category,subtype,link_dir,ssid");
    Serial.println("================================================================================");
    Serial.println("💡 การตรวจจับคลื่นรอบทิศทาง (Omni-RF Detection):");
    Serial.println("   [1] PROBE_REQ   -> มือถือค้นหา AP (สแตนด์บาย / ปิดเน็ต)");
    Serial.println("   [2] ACTION_P2P  -> Wi-Fi Direct / Quick Share / การค้นหากลุ่มส่งไฟล์");
    Serial.println("   [3] QOS_NULL    -> คลื่นชีพจร Keep-Alive ขณะโทรศัพท์เชื่อมต่อ Wi-Fi");
    Serial.println("   [4] HOTSPOT_AP  -> คลื่นเมื่อเปิด Hotspot บนมือถือ");
    Serial.println("   [5] DATA_UPLINK -> ข้อมูลที่โทรศัพท์ยิงออกสู่อากาศ");
    Serial.println("--------------------------------------------------------------------------------\n");

    nvs_flash_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    // เปิดรับทั้งเฟรม MANAGEMENT และ DATA ครบวงจร!
    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA;
    esp_wifi_set_promiscuous_filter(&filter);

    esp_wifi_set_promiscuous_rx_cb(wifiOmniCallback);
    esp_wifi_set_promiscuous(true);

    esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    Serial.printf(">>> Omni Sniffer Started! สแกนช่อง 1, 6, 11 (Hop ทุก %lu ms)\n\n", HOP_INTERVAL_MS);
}

void loop() {
    unsigned long currentMillis = millis();

    // Channel Hopping สลับฟังช่องสัญญาณ 1 -> 6 -> 11
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }
}


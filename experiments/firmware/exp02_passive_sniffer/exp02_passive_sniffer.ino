/*
 * ======================================================================================
 * 📡 EXP-02: ESP32-Nano Passive Wi-Fi Sniffer (Probe Request & MAC Tracking)
 * ======================================================================================
 * โครงการวิจัยและพัฒนาสหกิจศึกษา (Project Co-op)
 * 
 * วัตถุประสงค์:
 * 1. ดักจับเฟรม 802.11 Management Subtype 0x0040 (Probe Request) แบบพาสซีฟ 100%
 * 2. พิสูจน์ว่าโทรศัพท์มือถือที่ "ปิด Hotspot / ไม่ได้ต่อเน็ต" ยังคงแผ่คลื่นออกมา
 * 3. ตรวจสอบการสุ่ม MAC Address (MAC Randomization) และสกัด Sequence Number
 * 4. ส่งออกข้อมูลทั้งแบบสรุปอ่านง่าย และแบบ CSV เพื่อนำไปพล็อตกราฟ
 * 
 * บอร์ดเป้าหมาย: Arduino Nano ESP32 / ESP32-S3 / ESP32 DevKit
 * Baud Rate: 115200
 * ======================================================================================
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <WiFi.h>

// ช่องสัญญาณหลักที่จะทำการสลับ (Channel Hopping 1, 6, 11 ตามมาตรฐานคลื่น 2.4 GHz)
const uint8_t CHANNELS[] = {1, 6, 11};
const int NUM_CHANNELS = 3;
const unsigned long HOP_INTERVAL_MS = 500; // สลับช่องทุกๆ 500 ms

unsigned long lastHopTime = 0;
int currentChannelIndex = 0;
uint32_t packetCount = 0;
uint32_t randomMacCount = 0;
uint32_t realMacCount = 0;

// โครงสร้างส่วนหัวของเฟรม 802.11 Management
typedef struct {
    uint16_t frame_ctrl;
    uint16_t duration;
    uint8_t da[6];       // Destination Address
    uint8_t sa[6];       // Source Address (MAC Address ของโทรศัพท์)
    uint8_t bssid[6];    // BSSID
    uint16_t seq_ctrl;   // Sequence Control (12-bit Sequence + 4-bit Fragment)
} __attribute__((packed)) wifi_mgmt_header_t;

// ฟังก์ชันตรวจสอบว่าเป็น MAC สุ่มหรือไม่ (Locally Administered Bit)
bool isRandomMac(const uint8_t* mac) {
    // ในไบต์แรก บิตที่ 2 (บิต U/L) ถ้าเป็น 1 แสดงว่าเป็น Locally Administered / Randomized MAC
    return (mac[0] & 0x02) != 0;
}

// Callback เมื่อ ESP32 ดักจับแพ็กเก็ตดิบจากอากาศได้
void IRAM_ATTR wifiPromiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return; // กรองเฉพาะเฟรม Management

    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    int rssi = pkt->rx_ctrl.rssi;
    int len = pkt->rx_ctrl.sig_len;
    uint8_t* payload = pkt->payload;

    if (len < sizeof(wifi_mgmt_header_t)) return;

    wifi_mgmt_header_t* header = (wifi_mgmt_header_t*)payload;
    
    // ตรวจสอบ Subtype: 0x0040 คือ Probe Request (Type 0, Subtype 4)
    uint8_t frameType = (header->frame_ctrl & 0x0C) >> 2;
    uint8_t frameSubtype = (header->frame_ctrl & 0xF0) >> 4;

    if (frameType == 0 && frameSubtype == 4) { // Management Frame + Probe Request
        packetCount++;
        
        // สกัด Sequence Number (12 บิตแรกของ seq_ctrl)
        uint16_t seqNum = (header->seq_ctrl >> 4) & 0x0FFF;
        
        // ตรวจสอบชนิด MAC Address
        bool randomized = isRandomMac(header->sa);
        if (randomized) randomMacCount++; else realMacCount++;

        // ดึงชื่อ SSID (กรณีเป็น Directed Probe Request)
        char ssid[33] = "";
        int ssidLen = 0;
        int offset = sizeof(wifi_mgmt_header_t);
        
        // Tag 0 คือ SSID Element
        if (offset + 2 <= len) {
            uint8_t tagId = payload[offset];
            uint8_t tagLen = payload[offset + 1];
            if (tagId == 0 && tagLen > 0 && tagLen <= 32 && (offset + 2 + tagLen <= len)) {
                memcpy(ssid, &payload[offset + 2], tagLen);
                ssid[tagLen] = '\0';
                ssidLen = tagLen;
            }
        }

        // แปลง MAC Address เป็น String
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 header->sa[0], header->sa[1], header->sa[2],
                 header->sa[3], header->sa[4], header->sa[5]);

        // พิมพ์ข้อมูลรูปแบบ CSV เพื่อการบันทึก
        Serial.print("CSV,");
        Serial.print(millis());
        Serial.print(",");
        Serial.print(packetCount);
        Serial.print(",");
        Serial.print(pkt->rx_ctrl.channel);
        Serial.print(",");
        Serial.print(rssi);
        Serial.print(",");
        Serial.print(seqNum);
        Serial.print(",");
        Serial.print(macStr);
        Serial.print(",");
        Serial.print(randomized ? "RANDOM" : "REAL");
        Serial.print(",\"");
        Serial.print(ssidLen == 0 ? "Wildcard" : ssid);
        Serial.println("\"");
    }
}

void setup() {
    Serial.begin(115200);
    
    // หน่วงเวลารอ Serial Monitor เชื่อมต่อ (สำหรับ USB CDC บน Arduino Nano ESP32)
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 3000)) {
        delay(10);
    }

    Serial.println("\n================================================================================");
    Serial.println("🕵️ EXP-02: ESP32-Nano Passive Wi-Fi Promiscuous Sniffer");
    Serial.println("CSV Header: timestamp_ms,pkt_id,channel,rssi_dbm,seq_num,mac,mac_type,ssid");
    Serial.println("================================================================================");

    // กำหนดค่าเริ่มต้นระบบ Wi-Fi ในโหมด NULL (ปิดการเชื่อมต่อ ไม่เป็นทั้ง AP หรือ STA)
    nvs_flash_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    // ตั้งค่าตัวกรองเฉพาะเฟรม Management (Probe Request)
    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT;
    esp_wifi_set_promiscuous_filter(&filter);

    // ลงทะเบียนฟังก์ชันดักจับ (Callback) และเปิดโหมด Promiscuous
    esp_wifi_set_promiscuous_rx_cb(wifiPromiscuousCallback);
    esp_wifi_set_promiscuous(true);

    // กำหนดช่องสัญญาณเริ่มต้น
    esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    Serial.printf(">>> Sniffer Ready! กวาดช่อง Ch 1, 6, 11 สลับทุกๆ %lu ms\n\n", HOP_INTERVAL_MS);
}

void loop() {
    // Channel Hopping: สลับฟังช่องสัญญาณ 1 -> 6 -> 11 เพื่อดักแพ็กเก็ตให้ครบทุกย่าน
    unsigned long currentMillis = millis();
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }
}


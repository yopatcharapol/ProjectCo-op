/*
 * ======================================================================================
 * 📡 LAB 01-03: ESP32 Passive Wi-Fi Sniffer for Probe Request & MAC Tracking
 * ======================================================================================
 * โครงการวิจัยและพัฒนาสหกิจศึกษา (Project Co-op)
 * ทดสอบการดักจับคลื่น Probe Request (Active Scan), วัดค่า RSSI, สกัด Sequence Number,
 * และตรวจสอบการสุ่ม MAC Address (MAC Randomization) จากสมาร์ตโฟนจริงในบริเวณ
 * 
 * บอร์ดที่รองรับ: ESP32 DevKit v1, NodeMCU-32S, Arduino Nano ESP32, ESP32-S3
 * Baud Rate: 115200
 * ======================================================================================
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <WiFi.h>

// ช่องสัญญาณหลักที่จะทำการสลับ (Channel Hopping 1, 6, 11 ตามคำแนะนำ Li et al. 2020)
const uint8_t CHANNELS[] = {1, 6, 11};
const int NUM_CHANNELS = 3;
const unsigned long HOP_INTERVAL_MS = 500; // สลับช่องทุก 500 ms

unsigned long lastHopTime = 0;
int currentChannelIndex = 0;
uint32_t packetCount = 0;

// โครงสร้างส่วนหัวของเฟรม 802.11 Management
typedef struct {
    uint16_t frame_ctrl;
    uint16_t duration;
    uint8_t da[6];       // Destination Address
    uint8_t sa[6];       // Source Address (MAC ของมือถือ)
    uint8_t bssid[6];    // BSSID
    uint16_t seq_ctrl;   // Sequence Control (12-bit SEQ + 4-bit Frag)
} __attribute__((packed)) wifi_mgmt_header_t;

// ฟังก์ชันตรวจสอบว่าเป็น MAC สุ่มหรือไม่ (Locally Administered Bit)
bool isRandomMac(const uint8_t* mac) {
    // ในไบต์แรกของ MAC บิตที่ 2 (บิต U/L) ถ้าเป็น 1 แสดงว่าสุ่ม (เช่น x2, x6, xA, xE)
    return (mac[0] & 0x02) != 0;
}

// Callback เมื่อรับแพ็กเก็ตดิบจากอากาศได้ (Promiscuous RX Callback)
void wifiPromiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
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

    if (frameType == 0 && frameSubtype == 4) { // Management + Probe Request
        packetCount++;
        
        // สกัด Sequence Number (12 บิตแรกของ seq_ctrl)
        uint16_t seqNum = (header->seq_ctrl >> 4) & 0x0FFF;
        
        // ตรวจสอบชนิด MAC Address
        bool randomized = isRandomMac(header->sa);

        // ดึงชื่อ SSID (ถ้าเป็น Directed Probe Request)
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

        // แสดงผลออกทาง Serial Monitor
        Serial.printf("[#%05u] Ch:%02d | RSSI:%4d dBm | SEQ:%04d | MAC: %02X:%02X:%02X:%02X:%02X:%02X [%s] | ",
            packetCount,
            pkt->rx_ctrl.channel,
            rssi,
            seqNum,
            header->sa[0], header->sa[1], header->sa[2],
            header->sa[3], header->sa[4], header->sa[5],
            randomized ? "RANDOM" : "REAL  "
        );

        if (ssidLen == 0) {
            Serial.println("SSID: (Wildcard - หา AP ทั้งหมด)");
        } else {
            Serial.printf("SSID: \"%s\" (Directed)\n", ssid);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=======================================================");
    Serial.println("📡 ESP32 Passive Wi-Fi Sniffer (Lab 01-03)");
    Serial.println("=======================================================");
    Serial.println("กำลังเปิดโหมด Promiscuous Sniffer ดักฟัง Probe Request...");

    // เริ่มต้นระบบ Wi-Fi ในโหมด NULL (ไม่ต้องต่อเราเตอร์ใดๆ)
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

    // ลงทะเบียนฟังก์ชัน Callback เมื่อมีแพ็กเก็ตหลุดเข้ามาในอากาศ
    esp_wifi_set_promiscuous_rx_cb(wifiPromiscuousCallback);
    esp_wifi_set_promiscuous(true);

    // กำหนดช่องเริ่มต้นที่ Channel 1
    esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    Serial.printf("พร้อมทำงาน! สลับฟังช่อง 1, 6, 11 ทุกๆ %d ms\n", HOP_INTERVAL_MS);
    Serial.println("-------------------------------------------------------");
}

void loop() {
    // ระบบ Channel Hopping อัตโนมัติ สลับช่อง 1 -> 6 -> 11
    unsigned long currentMillis = millis();
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }
}

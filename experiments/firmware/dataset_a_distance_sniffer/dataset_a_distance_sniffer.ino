/*
 * ======================================================================================
 * 📡 DATASET-A: ESP32-Nano Distance vs RSSI Auto-Tagged Sniffer
 * ======================================================================================
 * วัตถุประสงค์:
 * เก็บรวบรวมข้อมูลระยะทาง (Distance) เทียบกับความแรงสัญญาณ (RSSI) จำนวนมาก
 * เพื่อสร้างแบบจำลอง Log-Distance Path Loss และหาค่าการลดทอนจริง
 * 
 * 💡 ฟีเจอร์พิเศษช่วยเก็บข้อมูลง่าย:
 * คุณสามารถพิมพ์ระยะทางลงในช่องพิมพ์ของ Serial Monitor แล้วกด Enter ได้เลย!
 * เช่น พิมพ์: 1m, 3m, 5m, 10m, 15m, 20m, 25m, 30m
 * ตัวบอร์ดจะติดป้ายกำกับระยะนั้นลงในไฟล์ CSV ให้อัตโนมัติทุกบรรทัด!
 * ======================================================================================
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <WiFi.h>

const uint8_t CHANNELS[] = {1, 6, 11};
const int NUM_CHANNELS = 3;
const unsigned long HOP_INTERVAL_MS = 500; // สลับช่องทุกๆ 500 ms

unsigned long lastHopTime = 0;
int currentChannelIndex = 0;
uint32_t packetCount = 0;

// ป้ายกำกับระยะทางปัจจุบัน (เริ่มต้นที่ 1m)
String currentDistanceLabel = "1m";

typedef struct {
    uint16_t frame_ctrl;
    uint16_t duration;
    uint8_t da[6];
    uint8_t sa[6];
    uint8_t bssid[6];
    uint16_t seq_ctrl;
} __attribute__((packed)) wifi_mgmt_header_t;

bool isRandomMac(const uint8_t* mac) {
    return (mac[0] & 0x02) != 0;
}

void IRAM_ATTR wifiPromiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return;

    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    int rssi = pkt->rx_ctrl.rssi;
    int len = pkt->rx_ctrl.sig_len;
    uint8_t* payload = pkt->payload;

    if (len < sizeof(wifi_mgmt_header_t)) return;

    wifi_mgmt_header_t* header = (wifi_mgmt_header_t*)payload;
    
    // ตรวจสอบว่าเป็นเฟรม Probe Request (Type 0, Subtype 4)
    uint8_t frameType = (header->frame_ctrl & 0x0C) >> 2;
    uint8_t frameSubtype = (header->frame_ctrl & 0xF0) >> 4;

    if (frameType == 0 && frameSubtype == 4) {
        packetCount++;
        uint16_t seqNum = (header->seq_ctrl >> 4) & 0x0FFF;
        bool randomized = isRandomMac(header->sa);

        char ssid[33] = "";
        int ssidLen = 0;
        int offset = sizeof(wifi_mgmt_header_t);
        
        if (offset + 2 <= len) {
            uint8_t tagId = payload[offset];
            uint8_t tagLen = payload[offset + 1];
            if (tagId == 0 && tagLen > 0 && tagLen <= 32 && (offset + 2 + tagLen <= len)) {
                memcpy(ssid, &payload[offset + 2], tagLen);
                ssid[tagLen] = '\0';
                ssidLen = tagLen;
            }
        }

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 header->sa[0], header->sa[1], header->sa[2],
                 header->sa[3], header->sa[4], header->sa[5]);

        // พิมพ์ผลออกเป็น CSV พร้อมแนบป้ายระยะทางปัจจุบันไปด้วย
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
        Serial.print("\",");
        Serial.println(currentDistanceLabel);
    }
}

void setup() {
    Serial.begin(115200);
    
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 3000)) {
        delay(10);
    }

    Serial.println("\n================================================================================");
    Serial.println("📏 DATASET-A: ESP32-Nano Distance vs RSSI Auto-Tagged Sniffer");
    Serial.println("CSV Header: timestamp_ms,pkt_id,channel,rssi_dbm,seq_num,mac,mac_type,ssid,distance");
    Serial.println("================================================================================");
    Serial.println("💡 คำแนะนำ: พิมพ์ระยะทาง เช่น 1m, 3m, 5m, 10m แล้วกด Enter เพื่อเปลี่ยนป้ายกำกับระยะ");
    Serial.print(">>> ระยะเริ่มต้นตั้งไว้ที่: ");
    Serial.println(currentDistanceLabel);
    Serial.println("--------------------------------------------------------------------------------\n");

    nvs_flash_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT;
    esp_wifi_set_promiscuous_filter(&filter);

    esp_wifi_set_promiscuous_rx_cb(wifiPromiscuousCallback);
    esp_wifi_set_promiscuous(true);

    esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
}

void loop() {
    // 1. Channel Hopping 1 -> 6 -> 11
    unsigned long currentMillis = millis();
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }

    // 2. รับคำสั่งเปลี่ยนระยะทางจากช่องพิมพ์ Serial Monitor
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            currentDistanceLabel = input;
            Serial.println();
            Serial.print("🚩 >>> [DISTANCE_CHANGED] เปลี่ยนระยะทางเป็น: ");
            Serial.print(currentDistanceLabel);
            Serial.println(" <<<");
            Serial.println();
        }
    }
}


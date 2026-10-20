/*
 * ======================================================================================
 * 📱 DATASET-C: ESP32-Nano Multi-Device Diversity Sniffer (iOS vs Android Benchmarking)
 * ======================================================================================
 * วัตถุประสงค์:
 * เก็บรวบรวมข้อมูลเพื่อเปรียบเทียบความแตกต่างระหว่างสมาร์ตโฟนหลายรุ่น/หลายยี่ห้อ:
 * 1. เปรียบเทียบระหว่าง Android ต่างแบรนด์ หรือเปรียบเทียบ Android vs iOS (iPhone)
 * 2. ศึกษาพฤติกรรมการสุ่ม MAC Address และตัวนับ Sequence Number ของแต่ละค่าย
 * 3. ทดสอบการตรวจจับอุปกรณ์หลายเครื่องพร้อมกัน (Multi-Target Concurrent Tracking)
 * 
 * 💡 คำแนะนำการใช้งาน Serial Monitor:
 * คุณสามารถพิมพ์แท็กอุปกรณ์เพื่อแยกแยะได้ง่าย:
 *   - พิมพ์: 1 หรือ DEV_A หรือ ANDROID_1  --> เครื่องหลักเครื่องเดิม
 *   - พิมพ์: 2 หรือ DEV_B หรือ IPHONE     --> เครื่องที่สองที่นำมาเทียบ (เช่น iPhone/Samsung)
 *   - พิมพ์: 3 หรือ BOTH หรือ CONCURRENT  --> เปิดทดสอบพร้อมกัน 2 เครื่อง
 *   - หรือพิมพ์ชื่อรุ่นตรงๆ เช่น IPHONE_13, SAMSUNG, VIVO, OPPO
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

// ป้ายกำกับอุปกรณ์ปัจจุบัน (เริ่มต้นที่ DEV_B_TEST)
String currentDeviceTag = "DEV_B_TEST";

// MAC ของเครื่องคอมพิวเตอร์ประจำฐาน
const char* BASE_LAPTOP_MAC = "C0:A5:E8:A6:17:06";

unsigned long lastTargetPacketTimeMs = 0;

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
        unsigned long now = millis();

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 header->sa[0], header->sa[1], header->sa[2],
                 header->sa[3], header->sa[4], header->sa[5]);

        bool isBaseStation = (strcasecmp(macStr, BASE_LAPTOP_MAC) == 0);
        bool randomMac = isRandomMac(header->sa);
        uint16_t seqNum = (header->seq_ctrl >> 4);

        unsigned long deltaTargetMs = 0;
        if (!isBaseStation) {
            if (lastTargetPacketTimeMs > 0) {
                deltaTargetMs = now - lastTargetPacketTimeMs;
            }
            lastTargetPacketTimeMs = now;
        }

        // ดึงชื่อ SSID
        String ssid = "Wildcard";
        int offset = sizeof(wifi_mgmt_header_t);
        while (offset + 2 <= len) {
            uint8_t elemId = payload[offset];
            uint8_t elemLen = payload[offset + 1];
            if (elemId == 0) { // SSID
                if (elemLen > 0 && elemLen <= 32 && (offset + 2 + elemLen <= len)) {
                    char tempSsid[33];
                    memcpy(tempSsid, &payload[offset + 2], elemLen);
                    tempSsid[elemLen] = '\0';
                    ssid = String(tempSsid);
                }
                break;
            }
            offset += 2 + elemLen;
        }

        // รูปแบบ CSV:
        // CSV,timestamp_ms,pkt_id,channel,rssi_dbm,delta_t_ms,seq_num,mac,mac_type,device_tag,ssid
        Serial.print("CSV,");
        Serial.print(now);
        Serial.print(",");
        Serial.print(packetCount);
        Serial.print(",");
        Serial.print(CHANNELS[currentChannelIndex]);
        Serial.print(",");
        Serial.print(rssi);
        Serial.print(",");
        Serial.print(deltaTargetMs);
        Serial.print(",");
        Serial.print(seqNum);
        Serial.print(",");
        Serial.print(macStr);
        Serial.print(",");
        if (isBaseStation) {
            Serial.print("BASE_LAPTOP,");
        } else {
            Serial.print(randomMac ? "RANDOM," : "REAL,");
        }
        Serial.print(currentDeviceTag);
        Serial.print(",\"");
        Serial.print(ssid);
        Serial.println("\"");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n================================================================================");
    Serial.println("📱 DATASET-C: ESP32-Nano Multi-Device Diversity Sniffer");
    Serial.println("CSV Header: timestamp_ms,pkt_id,channel,rssi_dbm,delta_t_ms,seq_num,mac,mac_type,device_tag,ssid");
    Serial.println("================================================================================");
    Serial.println("💡 คำแนะนำ: พิมพ์แท็กอุปกรณ์ลงใน Serial Monitor แล้วกด Enter:");
    Serial.println("   [1] หรือ DEV_A      --> เครื่องแรกเดิม (Android หลัก)");
    Serial.println("   [2] หรือ DEV_B      --> เครื่องที่ 2 (iPhone หรือ Android เครื่องอื่น)");
    Serial.println("   [3] หรือ BOTH       --> วางทดสอบพร้อมกัน 2 เครื่อง");
    Serial.println("   หรือพิมพ์ชื่อรุ่น เช่น IPHONE, SAMSUNG, XIAOMI, OPPO ได้ตามต้องการ");
    Serial.print(">>> แท็กอุปกรณ์เริ่มต้น: ");
    Serial.println(currentDeviceTag);
    Serial.println("--------------------------------------------------------------------------------\n");

    nvs_flash_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifiPromiscuousCallback);
    esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    lastHopTime = millis();
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            if (input.equalsIgnoreCase("1") || input.equalsIgnoreCase("DEV_A")) {
                currentDeviceTag = "DEV_A_PRIMARY";
            } else if (input.equalsIgnoreCase("2") || input.equalsIgnoreCase("DEV_B")) {
                currentDeviceTag = "DEV_B_SECONDARY";
            } else if (input.equalsIgnoreCase("3") || input.equalsIgnoreCase("BOTH")) {
                currentDeviceTag = "CONCURRENT_BOTH";
            } else {
                currentDeviceTag = input;
            }

            Serial.println();
            Serial.print("🚩 >>> [DEVICE_TAG_CHANGED] เปลี่ยนป้ายกำกับอุปกรณ์เป็น: ");
            Serial.print(currentDeviceTag);
            Serial.println(" <<<");
            Serial.println();
        }
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }
}


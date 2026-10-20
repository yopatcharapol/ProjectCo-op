/*
 * ======================================================================================
 * 🏃 DATASET-B: ESP32-Nano Temporal & Motion Profiling Sniffer (Doze vs Walking)
 * ======================================================================================
 * วัตถุประสงค์:
 * เก็บรวบรวมข้อมูลในแกนเวลา (Time Domain) เพื่อศึกษาพฤติกรรมการยิง Probe Request:
 * 1. วางนิ่งจอดับ (Stationary / Deep Doze Mode) เพื่อดูรอบเวลาการส่งคลื่นตอนนอนหลับ
 * 2. เคลื่อนไหว/เดิน (Active / Motion Triggered) เพื่อดูการปลุก Wi-Fi จากเซนเซอร์ Accelerometer
 * 
 * 💡 คำแนะนำการใช้งาน Serial Monitor:
 * คุณสามารถพิมพ์สถานะเพื่อเปลี่ยนแท็กได้ทันที:
 *   - พิมพ์: 1 หรือ ST หรือ STATIONARY  --> วางนิ่งปิดจอ (คนหมดสติ / ของตก)
 *   - พิมพ์: 2 หรือ WK หรือ WALKING     --> ถือเดินแกว่งแขน (คนกำลังเดิน)
 *   - พิมพ์: 3 หรือ PK หรือ POCKET      --> ใส่กระเป๋ากางเกงเดิน (การพกพาจริง)
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

// สถานะการทดสอบปัจจุบัน (เริ่มต้นที่ STATIONARY)
String currentMotionState = "STATIONARY";

// ตัวแปรจับเวลาห่างระหว่างแพ็กเก็ต (Inter-arrival / Inter-burst Time)
unsigned long lastTargetPacketTimeMs = 0;

// MAC ของเครื่องคอมพิวเตอร์ประจำฐาน (เพื่อแยกออกจากการคำนวณช่วงเงียบของเป้าหมาย)
const char* BASE_LAPTOP_MAC = "C0:A5:E8:A6:17:06";

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

        // คำนวณช่วงเวลาห่างจากแพ็กเก็ตเป้าหมายตัวก่อนหน้า (Inter-arrival Time delta_t)
        unsigned long deltaTargetMs = 0;
        if (!isBaseStation) {
            if (lastTargetPacketTimeMs > 0) {
                deltaTargetMs = now - lastTargetPacketTimeMs;
            }
            lastTargetPacketTimeMs = now;
        }

        // ดึงชื่อ SSID ที่ Probe Request ถามหา (ถ้ามี)
        String ssid = "Wildcard";
        int offset = sizeof(wifi_mgmt_header_t);
        while (offset + 2 <= len) {
            uint8_t elemId = payload[offset];
            uint8_t elemLen = payload[offset + 1];
            if (elemId == 0) { // SSID parameter
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

        // ส่งออกรูปแบบ CSV:
        // CSV,timestamp_ms,pkt_id,channel,rssi_dbm,delta_t_ms,seq_num,mac,mac_type,state,ssid
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
        Serial.print(currentMotionState);
        Serial.print(",\"");
        Serial.print(ssid);
        Serial.println("\"");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n================================================================================");
    Serial.println("🏃 DATASET-B: ESP32-Nano Temporal & Motion Sniffer (Doze vs Motion)");
    Serial.println("CSV Header: timestamp_ms,pkt_id,channel,rssi_dbm,delta_t_ms,seq_num,mac,mac_type,state,ssid");
    Serial.println("================================================================================");
    Serial.println("💡 คำแนะนำ: พิมพ์สถานะลงใน Serial Monitor แล้วกด Enter:");
    Serial.println("   [1] หรือ STATIONARY --> วางนิ่งปิดจอ (คนหมดสติ/ของตกหล่น)");
    Serial.println("   [2] หรือ WALKING    --> ถือแกว่งแขนเดิน (คนกำลังเดินหาทาง)");
    Serial.println("   [3] หรือ POCKET     --> ใส่กระเป๋ากางเกงเดิน (การพกพาในชีวิตจริง)");
    Serial.print(">>> สถานะเริ่มต้นตั้งไว้ที่: ");
    Serial.println(currentMotionState);
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
    // รับคำสั่งเปลี่ยนสถานะการเคลื่อนไหวผ่าน Serial Monitor
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            if (input.equalsIgnoreCase("1") || input.equalsIgnoreCase("ST") || input.equalsIgnoreCase("STATIONARY")) {
                currentMotionState = "STATIONARY";
            } else if (input.equalsIgnoreCase("2") || input.equalsIgnoreCase("WK") || input.equalsIgnoreCase("WALKING")) {
                currentMotionState = "WALKING";
            } else if (input.equalsIgnoreCase("3") || input.equalsIgnoreCase("PK") || input.equalsIgnoreCase("POCKET")) {
                currentMotionState = "POCKET";
            } else {
                currentMotionState = input; // อนุญาตให้พิมพ์ป้ายกำกับอิสระได้
            }

            Serial.println();
            Serial.print("🚩 >>> [STATE_CHANGED] เปลี่ยนสถานะการเคลื่อนไหวเป็น: ");
            Serial.print(currentMotionState);
            Serial.println(" <<<");
            Serial.println();
        }
    }

    // สลับช่องสัญญาณ (Channel Hopping 1 -> 6 -> 11)
    unsigned long currentMillis = millis();
    if (currentMillis - lastHopTime >= HOP_INTERVAL_MS) {
        lastHopTime = currentMillis;
        currentChannelIndex = (currentChannelIndex + 1) % NUM_CHANNELS;
        esp_wifi_set_channel(CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
    }
}


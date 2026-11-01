/*
 * ======================================================================================
 * 📡 EXP-05 (v2.0): Tactical Dragon Radar — Dual-Radio Fusion Sniffer (Wi-Fi + BLE TDM)
 * ======================================================================================
 * โครงการวิจัยและพัฒนาสหกิจศึกษา (Project Co-op)
 * 
 * คุณสมบัติสำคัญในเวอร์ชันอัปเกรด v2.0 (Dormitory Field-Test Upgraded):
 * 1. Time-Division Multiplexing (TDM):
 *    - สลับภาครับวิทยุ 2.4 GHz ระหว่าง Wi-Fi Promiscuous (1,650 ms) และ BLE GAP Scanner (850 ms)
 * 
 * 2. Dual-Calibration Log-Distance Path Loss Model:
 *    - แยกค่าอ้างอิงกำลังส่งที่ 1 เมตรระหว่าง Wi-Fi และ BLE เพื่อความแม่นยำสูงสุด:
 *      * PL_0_WIFI = -38.2 dBm (คำนวณระยะ Wi-Fi: -45 dBm -> 1.89m, -57 dBm -> 5.85m)
 *      * PL_0_BLE  = -51.0 dBm (ชดเชยกำลังส่ง BLE ที่เบากว่า Wi-Fi ~12.8 dB: -55 dBm -> 1.46m, -70 dBm -> 5.96m)
 *    - ฟิลเตอร์ความผันผวน RSSI ด้วย Exponential Moving Average (EMA, alpha = 0.35)
 * 
 * 3. Smart Target De-Randomization & Time-Locked Multi-Advertising Fusion:
 *    - ฝั่ง Wi-Fi: รวบ MAC สุ่มด้วย 12-bit Hardware Sequence Continuity (Modulo 4096) + Delta RSSI
 *    - ฝั่ง BLE: แยกอุปกรณ์ BLE ประจำห้องออกจากกันอย่างเด็ดขาด (ไม่นำ Dummy Counter มาปะปน)
 *      พร้อมอัลกอริทึมตรวจจับ "คู่แฝด 14 ms" (Time-Locked Dual BLE Advertising: Quick Share + Honor Share/MTA)
 *      ยุบรวม 2 MAC ที่ยิงสลับคิวฮาร์ดแวร์ระดับ 10-25 ms ให้เป็นสมาร์ทโฟนเป้าหมายตัวเดียวกันอัตโนมัติ!
 * 
 * ฮาร์ดแวร์: Arduino Nano ESP32 / ESP32-S3
 * Baud Rate: 115200
 * ======================================================================================
 */

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// ======================================================================================
// 1. กำหนดค่าและพารามิเตอร์ระบบ (System Configuration)
// ======================================================================================

// พารามิเตอร์โมเดลการลดทอนสัญญาณแบบแยกคลื่น (Dual-Calibration Path Loss Model)
const float PL_0_WIFI = -38.2f; // RSSI อ้างอิงของ Wi-Fi ที่ระยะ 1 เมตร (dBm)
const float PL_0_BLE  = -51.0f; // RSSI อ้างอิงของ BLE ที่ระยะ 1 เมตร (dBm) - ชดเชยกำลังส่ง BLE
const float N_EXP     = 2.45f;  // Path Loss Exponent ของสภาพแวดล้อมจริง
const float EMA_ALPHA = 0.35f;  // ค่าถ่วงน้ำหนัก Exponential Moving Average

// คาบเวลา Time-Division Multiplexing (TDM)
const unsigned long WIFI_HOP_INTERVAL_MS = 150; // สลับช่อง Wi-Fi ทุกๆ 150 ms
const uint8_t WIFI_CHANNELS[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10};
const int NUM_WIFI_CHANNELS = 11;
const unsigned long WIFI_PHASE_DURATION_MS = 1650; // กวาดครบ 11 ช่อง = 1.65 วินาที
const unsigned long BLE_PHASE_DURATION_MS  = 850;  // สแกน BLE = 0.85 วินาที

// ตารางจดจำเป้าหมายใน RAM (Target Cache Table - ขยายรองรับหอพักหนาแน่น 48 เป้าหมาย)
#define MAX_TARGETS 48
#define TARGET_TIMEOUT_MS 45000 // หากไม่เห็นเกิน 45 วินาทีจะถือว่าหลุดระยะ

struct TargetEntry {
    char targetId[10];          // เช่น "TGT-01", "TGT-02"
    uint8_t lastMac[6];         // MAC Address หลักล่าสุด
    uint8_t altMac[6];          // MAC Address คู่แฝด (กรณี BLE Multi-Advertising เช่น Quick Share + Honor Share)
    bool hasAltMac;             // มี MAC คู่แฝดผูกอยู่หรือไม่
    bool isBle;                 // เป็นเป้าหมายฝั่ง BLE หรือ Wi-Fi
    bool isRandomMac;           // เป็น MAC สุ่มหรือไม่
    uint16_t lastSeqNum;        // 12-bit Sequence Number ล่าสุด
    int lastRssi;               // ค่า RSSI ดิบ
    float smoothRssi;           // ค่า RSSI ที่ผ่านการฟิลเตอร์
    float estDistance;          // ระยะทางที่คำนวณได้ (เมตร)
    unsigned long firstSeenMs;  // เวลาที่พบครั้งแรก
    unsigned long lastSeenMs;   // เวลาที่พบล่าสุด
    unsigned long lastEmitMs;   // เวลาที่ส่งออก Serial ล่าสุด (ป้องกัน USB CDC สำลักเมื่อเจอ Packet Burst)
    uint32_t packetCount;       // จำนวนแพ็กเก็ตสะสม
    char brand[28];             // ยี่ห้อ / บริษัท
    char primaryCategory[20];   // หมวดหมู่คลื่นหลัก (PROBE_REQ, BLE_MTA_ALLIANCE, QOS_NULL, etc.)
    bool active;
};

TargetEntry targets[MAX_TARGETS];
int targetCount = 0;
uint32_t totalFusionPackets = 0;
volatile bool wifiSniffActive = false;
volatile bool callbackBusy = false;

// ตัวแปรควบคุมสถานะ TDM
enum RadioPhase {
    PHASE_WIFI_SNIFF,
    PHASE_BLE_SCAN
};

RadioPhase currentPhase = PHASE_WIFI_SNIFF;
unsigned long phaseStartTimeMs = 0;
unsigned long lastWifiHopMs = 0;
int currentChannelIndex = 0;

// อ็อบเจกต์ BLE
BLEScan* pBLEScan = nullptr;

// ======================================================================================
// 2. ฟังก์ชันอรรถประโยชน์ทางวิศวกรรม (Engineering Utilities)
// ======================================================================================

// ตรวจสอบ MAC สุ่ม (Locally Administered Bit: บิตที่ 2 ของไบต์แรกเป็น 1)
bool isRandomMacAddress(const uint8_t* mac) {
    return (mac[0] & 0x02) != 0;
}

// คำนวณระยะทางจาก RSSI ด้วยสูตร Dual-Calibration Log-Distance Path Loss
float calculateDistance(float rssi, bool isBle) {
    if (rssi >= 0) return 0.5f;
    float pl0 = isBle ? PL_0_BLE : PL_0_WIFI;
    float exponent = (pl0 - rssi) / (10.0f * N_EXP);
    float dist = pow(10.0f, exponent);
    if (dist < 0.3f) dist = 0.3f;
    if (dist > 99.9f) dist = 99.9f;
    return dist;
}

// ตรวจสอบความต่อเนื่องของ Sequence Number แบบ 12 บิต (Modulo 4096) เฉพาะฝั่ง Wi-Fi
bool isSequenceContinuous(uint16_t oldSeq, uint16_t newSeq, uint16_t maxDiff = 25) {
    uint16_t diff;
    if (newSeq >= oldSeq) {
        diff = newSeq - oldSeq;
    } else {
        diff = (newSeq + 4096) - oldSeq; // รองรับกรณีวนรอบ Modulo 4096
    }
    return (diff >= 1 && diff <= maxDiff);
}

// ถอดรหัสยี่ห้อจาก OUI (3 ไบต์แรกของ MAC แท้)
void getVendorFromMac(const uint8_t* mac, char* buffer, size_t maxLen) {
    if (isRandomMacAddress(mac)) {
        snprintf(buffer, maxLen, "Randomized(Privacy)");
        return;
    }

    if (mac[0] == 0xB8 && mac[1] == 0xB1 && mac[2] == 0xEA) {
        snprintf(buffer, maxLen, "Honor");
    } else if (mac[0] == 0xC0 && mac[1] == 0x06 && mac[2] == 0xC3) {
        snprintf(buffer, maxLen, "Xiaomi/POCO");
    } else if (mac[0] == 0xC0 && mac[1] == 0xA5 && mac[2] == 0xE8) {
        snprintf(buffer, maxLen, "Laptop(BaseStation)");
    } else if (mac[0] == 0x38 && mac[1] == 0x18 && mac[2] == 0x4C) {
        snprintf(buffer, maxLen, "Apple");
    } else if (mac[0] == 0x28 && mac[1] == 0x6D && mac[2] == 0xCD) {
        snprintf(buffer, maxLen, "Apple");
    } else if (mac[0] == 0xDC && mac[1] == 0x2C && mac[2] == 0x26) {
        snprintf(buffer, maxLen, "Samsung");
    } else {
        snprintf(buffer, maxLen, "Vendor(%02X:%02X:%02X)", mac[0], mac[1], mac[2]);
    }
}

// ======================================================================================
// 3. ระบบเชื่อมโยงเป้าหมายและรวมร่างคู่แฝด 14ms (Smart Target Fusion Engine v2.0)
// ======================================================================================

int findOrCreateTarget(const uint8_t* mac, bool isRandom, bool isBle, uint16_t seqNum, int rssi, const char* category, const char* brandHint) {
    unsigned long now = millis();
    int matchedIndex = -1;

    // 1. ตรวจสอบการจับคู่แบบ MAC Address ตรงกันเป๊ะ (ทั้ง MAC หลัก และ MAC คู่แฝด altMac)
    for (int i = 0; i < targetCount; i++) {
        if (!targets[i].active) continue;
        if (memcmp(targets[i].lastMac, mac, 6) == 0) {
            matchedIndex = i;
            break;
        }
        if (targets[i].hasAltMac && memcmp(targets[i].altMac, mac, 6) == 0) {
            matchedIndex = i;
            break;
        }
    }

    // 2A. กรณีฝั่ง Wi-Fi (ใช้ 12-bit Hardware Sequence Continuity + Delta RSSI Window)
    if (matchedIndex == -1 && !isBle && isRandom) {
        for (int i = 0; i < targetCount; i++) {
            if (!targets[i].active || targets[i].isBle) continue;
            if (now - targets[i].lastSeenMs < 30000) {
                if (isSequenceContinuous(targets[i].lastSeqNum, seqNum, 25)) {
                    if (abs(rssi - targets[i].lastRssi) <= 10) {
                        matchedIndex = i; // มือถือเครื่องเดิมที่เพิ่งสุ่มเปลี่ยน MAC บน Wi-Fi!
                        memcpy(targets[i].lastMac, mac, 6);
                        break;
                    }
                }
            }
        }
    }

    // 2B. กรณีฝั่ง BLE: ตรวจจับ "คู่แฝด Time-Locked Multi-Advertising (10-25 ms)" เช่น Quick Share + Honor Share
    //     โดยไม่เอาอุปกรณ์ BLE ประจำห้องตัวอื่นมามั่วรวมกัน!
    if (matchedIndex == -1 && isBle && isRandom && category != nullptr) {
        bool isCurrentSharing = (strcmp(category, "BLE_MTA_ALLIANCE") == 0 ||
                                 strcmp(category, "BLE_HONORSHARE") == 0 ||
                                 strcmp(category, "BLE_QUICKSHARE") == 0 ||
                                 strcmp(category, "BLE_MISHARE") == 0);

        for (int i = 0; i < targetCount; i++) {
            if (!targets[i].active || !targets[i].isBle || targets[i].hasAltMac) continue;

            unsigned long dt = now - targets[i].lastSeenMs;
            bool isTargetSharing = (strcmp(targets[i].primaryCategory, "BLE_MTA_ALLIANCE") == 0 ||
                                    strcmp(targets[i].primaryCategory, "BLE_HONORSHARE") == 0 ||
                                    strcmp(targets[i].primaryCategory, "BLE_QUICKSHARE") == 0 ||
                                    strcmp(targets[i].primaryCategory, "BLE_MISHARE") == 0);

            // หากยิงตามหลังกันในระดับฮาร์ดแวร์ 10-25 ms และตัวใดตัวหนึ่งเป็นโปรโตคอลแชร์ไฟล์ (MTA / Honor / Quick Share)
            // และมีระดับความแรงสัญญาณสอดคล้องกัน (ห่างกันไม่เกิน 8 dB) -> รวมร่างเป็นสมาร์ทโฟนเครื่องเดียวกัน!
            if (dt >= 10 && dt <= 25 && (isCurrentSharing || isTargetSharing)) {
                if (abs(rssi - targets[i].lastRssi) <= 8) {
                    matchedIndex = i;
                    memcpy(targets[i].altMac, mac, 6);
                    targets[i].hasAltMac = true;
                    snprintf(targets[i].brand, sizeof(targets[i].brand), "Honor/MTA(Dual-BLE)");
                    if (isCurrentSharing) {
                        strncpy(targets[i].primaryCategory, category, sizeof(targets[i].primaryCategory) - 1);
                    }
                    break;
                }
            }
        }
    }

    // 3. หากเป็นเป้าหมายใหม่ ให้ลงทะเบียนในตาราง
    if (matchedIndex == -1) {
        if (targetCount < MAX_TARGETS) {
            matchedIndex = targetCount++;
        } else {
            // หากตารางเต็ม ให้แทนที่เป้าหมายที่ไม่ได้ส่งสัญญาณนานที่สุด
            unsigned long oldestSeen = now;
            int oldestIndex = 0;
            for (int i = 0; i < MAX_TARGETS; i++) {
                if (!targets[i].active) {
                    oldestIndex = i;
                    break;
                }
                if (targets[i].lastSeenMs < oldestSeen) {
                    oldestSeen = targets[i].lastSeenMs;
                    oldestIndex = i;
                }
            }
            matchedIndex = oldestIndex;
        }

        snprintf(targets[matchedIndex].targetId, sizeof(targets[matchedIndex].targetId), "TGT-%02d", matchedIndex + 1);
        memcpy(targets[matchedIndex].lastMac, mac, 6);
        memset(targets[matchedIndex].altMac, 0, 6);
        targets[matchedIndex].hasAltMac = false;
        targets[matchedIndex].isBle = isBle;
        targets[matchedIndex].firstSeenMs = now;
        targets[matchedIndex].lastEmitMs = 0;
        targets[matchedIndex].packetCount = 0;
        targets[matchedIndex].smoothRssi = (float)rssi;
        targets[matchedIndex].active = true;

        if (brandHint && strlen(brandHint) > 0) {
            strncpy(targets[matchedIndex].brand, brandHint, sizeof(targets[matchedIndex].brand) - 1);
        } else {
            getVendorFromMac(mac, targets[matchedIndex].brand, sizeof(targets[matchedIndex].brand));
        }
        if (category && strlen(category) > 0) {
            strncpy(targets[matchedIndex].primaryCategory, category, sizeof(targets[matchedIndex].primaryCategory) - 1);
        }
    }

    // 4. ปรับปรุงข้อมูลเป้าหมาย (Update Telemetry)
    targets[matchedIndex].packetCount++;
    targets[matchedIndex].lastSeenMs = now;
    targets[matchedIndex].isRandomMac = isRandom;
    targets[matchedIndex].lastSeqNum = seqNum;
    targets[matchedIndex].lastRssi = rssi;

    // คำนวณ EMA Filter สำหรับ RSSI และแปลงเป็นระยะทางด้วยพารามิเตอร์เฉพาะของคลื่นนั้น (Wi-Fi vs BLE)
    targets[matchedIndex].smoothRssi = (EMA_ALPHA * (float)rssi) + ((1.0f - EMA_ALPHA) * targets[matchedIndex].smoothRssi);
    targets[matchedIndex].estDistance = calculateDistance(targets[matchedIndex].smoothRssi, targets[matchedIndex].isBle);

    return matchedIndex;
}

// ส่งออกข้อมูลในรูปแบบ CSV_FUSION สู่พอร์ต Serial แบบ Atomic บรรทัดเดียว (ป้องกัน USB CDC ของ ESP32-Nano ค้างหรือหลุด)
void emitFusionPacket(int targetIdx, const char* radioType, uint8_t channel, int rawRssi, unsigned long deltaT, uint16_t seqNum, const char* macStr, bool isRandom, const char* category, const char* extraInfo) {
    unsigned long now = millis();

    // ป้องกัน Wi-Fi Burst (เช่น 80 แพ็กเก็ตใน 200ms) อัดพอร์ต USB CDC จนเต็มแล้วบอร์ดตัดการเชื่อมต่อ
    // โดยยังคงคำนวณ EMA RSSI ทุกแพ็กเก็ต แต่จำกัดการพิมพ์ออก Serial ของเป้าหมาย Wi-Fi เดิมไม่ให้ถี่เกินทุกๆ 45 ms
    if (!targets[targetIdx].isBle && targets[targetIdx].lastEmitMs != 0 && (now - targets[targetIdx].lastEmitMs < 45)) {
        return;
    }
    targets[targetIdx].lastEmitMs = now;
    totalFusionPackets++;

    char lineBuf[220];
    snprintf(lineBuf, sizeof(lineBuf),
             "CSV_FUSION,%lu,%s,%s,%u,%d,%.1f,%.2f,%lu,%u,%s,%s,%s,\"%s\",\"%s\"",
             now,
             targets[targetIdx].targetId,
             radioType,
             channel,
             rawRssi,
             targets[targetIdx].smoothRssi,
             targets[targetIdx].estDistance,
             deltaT,
             seqNum,
             macStr,
             isRandom ? "RANDOM" : "REAL",
             category,
             targets[targetIdx].brand,
             extraInfo ? extraInfo : "");
    Serial.println(lineBuf);
}

// ======================================================================================
// 4. Wi-Fi Promiscuous Sniffer Callbacks & Types
// ======================================================================================

typedef struct {
    uint16_t frame_ctrl;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6]; // Transmitter Address (TA)
    uint8_t addr3[6];
    uint16_t seq_ctrl;
} __attribute__((packed)) wifi_ieee80211_mac_header_t;

unsigned long lastWifiPktTimeMs = 0;

void IRAM_ATTR wifiFusionCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (!wifiSniffActive || callbackBusy) return;
    if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;
    callbackBusy = true;

    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    int len = pkt->rx_ctrl.sig_len;
    uint8_t* payload = pkt->payload;

    if (len < sizeof(wifi_ieee80211_mac_header_t)) {
        callbackBusy = false;
        return;
    }

    wifi_ieee80211_mac_header_t* header = (wifi_ieee80211_mac_header_t*)payload;
    uint16_t fc = header->frame_ctrl;
    uint8_t fType = (fc >> 2) & 0x03;
    uint8_t fSubtype = (fc >> 4) & 0x0F;
    bool toDs = (fc & 0x0100) != 0;
    bool fromDs = (fc & 0x0200) != 0;

    // กรอง Downlink จาก Access Point ปกติทิ้ง เพื่อลดสัญญาณรบกวน
    if (fType == 2 && fromDs && !toDs) {
        callbackBusy = false;
        return;
    }

    // กรองเราเตอร์บ้านที่อยู่ไกลในกรณี Beacon
    if (fType == 0 && fSubtype == 8) {
        if (!isRandomMacAddress(header->addr2) && pkt->rx_ctrl.rssi < -68) {
            callbackBusy = false;
            return;
        }
    }

    unsigned long now = millis();
    unsigned long deltaT = (lastWifiPktTimeMs == 0) ? 0 : (now - lastWifiPktTimeMs);
    lastWifiPktTimeMs = now;

    int rssi = pkt->rx_ctrl.rssi;
    uint16_t seqNum = (header->seq_ctrl >> 4) & 0x0FFF;
    bool randomMac = isRandomMacAddress(header->addr2);

    const char* category = "WIFI_OTHER";
    if (fType == 0 && fSubtype == 4) category = "PROBE_REQ";
    else if (fType == 0 && fSubtype == 13) category = "ACTION_NAN";
    else if (fType == 2 && (fSubtype == 12 || fSubtype == 4)) category = "QOS_NULL";
    else if (fType == 0 && fSubtype == 8) category = "HOTSPOT_AP";
    else if (fType == 2) category = "DATA_UPLINK";

    char ssid[33] = "";
    int ssidLen = 0;
    if (fType == 0 && (fSubtype == 4 || fSubtype == 8 || fSubtype == 5)) {
        int offset = sizeof(wifi_ieee80211_mac_header_t);
        if (fSubtype == 8 || fSubtype == 5) offset += 12;
        if (offset + 2 <= len) {
            uint8_t tagId = payload[offset];
            uint8_t tagLen = payload[offset + 1];
            if (tagId == 0 && tagLen <= 32 && (offset + 2 + tagLen <= len)) {
                if (tagLen > 0) {
                    memcpy(ssid, &payload[offset + 2], tagLen);
                    ssid[tagLen] = '\0';
                    ssidLen = tagLen;
                }
            }
        }
    }

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             header->addr2[0], header->addr2[1], header->addr2[2],
             header->addr2[3], header->addr2[4], header->addr2[5]);

    int targetIdx = findOrCreateTarget(header->addr2, randomMac, false, seqNum, rssi, category, nullptr);
    emitFusionPacket(targetIdx, "WIFI", pkt->rx_ctrl.channel, rssi, deltaT, seqNum, macStr, randomMac, category, ssidLen > 0 ? ssid : "Wildcard");
    callbackBusy = false;
}

// ======================================================================================
// 5. BLE GAP Advertised Device Callbacks
// ======================================================================================

unsigned long lastBlePktTimeMs = 0;

class BleFusionCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        unsigned long now = millis();
        unsigned long deltaMs = (lastBlePktTimeMs == 0) ? 0 : (now - lastBlePktTimeMs);
        lastBlePktTimeMs = now;

        int rssi = advertisedDevice.getRSSI();
        BLEAddress bleAddr = advertisedDevice.getAddress();
        String macStr = String(bleAddr.toString().c_str());
        macStr.toUpperCase();

        uint8_t rawMac[6];
        esp_bd_addr_t* pNative = bleAddr.getNative();
        if (pNative != nullptr) {
            memcpy(rawMac, (const void*)pNative, 6);
        } else {
            memset(rawMac, 0, 6);
        }

        bool isRandom = (advertisedDevice.getAddressType() == BLE_ADDR_TYPE_RANDOM);

        String brandStr = "";
        String category = "BLE_ADV";
        String extraInfo = "";

        if (advertisedDevice.haveName()) {
            extraInfo = String(advertisedDevice.getName().c_str());
            extraInfo.replace(",", "_");
        }

        if (advertisedDevice.haveManufacturerData()) {
            std::string mData = advertisedDevice.getManufacturerData();
            if (mData.length() >= 2) {
                uint16_t companyId = (uint8_t)mData[0] | ((uint8_t)mData[1] << 8);
                if (companyId == 0x038F) {
                    brandStr = "Xiaomi/POCO";
                    category = "BLE_MISHARE";
                } else if (companyId == 0x004C) {
                    brandStr = "Apple";
                    category = "BLE_AIRDROP";
                } else if (companyId == 0x0075) {
                    brandStr = "Samsung";
                    category = "BLE_QUICKSHARE";
                } else if (companyId == 0x00E0) {
                    brandStr = "Google";
                    category = "BLE_FASTPAIR";
                } else if (companyId == 0x027D) {
                    brandStr = "Huawei/Honor";
                    category = "BLE_HONORSHARE";
                }
            }
        }

        if (advertisedDevice.haveServiceUUID()) {
            String sUuid = String(advertisedDevice.getServiceUUID().toString().c_str());
            if (sUuid.indexOf("3331") >= 0 || sUuid.indexOf("fd2d") >= 0 || sUuid.indexOf("FD2D") >= 0) {
                category = "BLE_MTA_ALLIANCE";
                if (brandStr.length() == 0) brandStr = "MTA_Alliance";
            } else if (sUuid.indexOf("fef3") >= 0 || sUuid.indexOf("FEF3") >= 0) {
                category = "BLE_QUICKSHARE";
                if (brandStr.length() == 0) brandStr = "Android_QuickShare";
            }
        }

        static uint16_t bleDummySeq = 0;
        bleDummySeq = (bleDummySeq + 1) & 0x0FFF;

        int targetIdx = findOrCreateTarget(rawMac, isRandom, true, bleDummySeq, rssi, category.c_str(), brandStr.length() > 0 ? brandStr.c_str() : nullptr);
        emitFusionPacket(targetIdx, "BLE", 37, rssi, deltaMs, bleDummySeq, macStr.c_str(), isRandom, category.c_str(), extraInfo.c_str());
    }
};

// ======================================================================================
// 6. Setup และระบบจัดการ Time-Division Multiplexing (TDM Loop)
// ======================================================================================

void initWifiSniffer() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA;
    esp_wifi_set_promiscuous_filter(&filter);
    esp_wifi_set_promiscuous_rx_cb(wifiFusionCallback);
    wifiSniffActive = true;
    esp_wifi_set_promiscuous(true);

    esp_wifi_set_channel(WIFI_CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
}

void initBleScanner() {
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new BleFusionCallbacks(), true);
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(80);
}

void setup() {
    Serial.begin(115200);

    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 3000)) {
        delay(10);
    }

    Serial.println("\n================================================================================");
    Serial.println("🐉 TACTICAL DRAGON RADAR — EXP-05 v2.1 DUAL-RADIO FUSION (Wi-Fi + BLE TDM)");
    Serial.println("CSV Header: CSV_FUSION,timestamp_ms,target_id,radio_type,channel,rssi_raw,rssi_smooth,est_dist_m,delta_t_ms,seq_num,mac,mac_type,category,brand,extra_info");
    Serial.println("================================================================================");
    Serial.println("⚙️ พารามิเตอร์ระบบ (v2.1 Anti-Disconnect USB Guard & Multi-Adv Fusion):");
    Serial.printf("   - Path Loss Model: PL_0_WIFI = %.1f dBm | PL_0_BLE = %.1f dBm | n = %.2f\n", PL_0_WIFI, PL_0_BLE, N_EXP);
    Serial.printf("   - TDM คาบเวลา: Wi-Fi Sniff %lu ms (กวาด 11 ช่อง) | BLE Scan %lu ms\n", WIFI_PHASE_DURATION_MS, BLE_PHASE_DURATION_MS);
    Serial.println("   - กลไกจำแนกเป้าหมาย: Wi-Fi 12-bit Seq Continuity + BLE 14ms Multi-Adv Fusion");
    Serial.println("--------------------------------------------------------------------------------\n");

    nvs_flash_init();
    initWifiSniffer();
    initBleScanner();

    currentPhase = PHASE_WIFI_SNIFF;
    phaseStartTimeMs = millis();
    lastWifiHopMs = millis();
}

void loop() {
    unsigned long now = millis();

    if (currentPhase == PHASE_WIFI_SNIFF) {
        if (now - lastWifiHopMs >= WIFI_HOP_INTERVAL_MS) {
            lastWifiHopMs = now;
            currentChannelIndex = (currentChannelIndex + 1) % NUM_WIFI_CHANNELS;
            esp_wifi_set_channel(WIFI_CHANNELS[currentChannelIndex], WIFI_SECOND_CHAN_NONE);
        }

        if (now - phaseStartTimeMs >= WIFI_PHASE_DURATION_MS) {
            wifiSniffActive = false;
            esp_wifi_set_promiscuous(false);
            delay(20); // รอให้ Callback ค้างท่อของ Wi-Fi จบงานก่อนเปิด BLE ป้องกัน RF Arbiter ชนกัน
            currentPhase = PHASE_BLE_SCAN;
            phaseStartTimeMs = millis();
        }
    } else if (currentPhase == PHASE_BLE_SCAN) {
        pBLEScan->start(1, false);
        pBLEScan->clearResults();

        if (millis() - phaseStartTimeMs >= BLE_PHASE_DURATION_MS) {
            pBLEScan->stop();
            delay(15); // พักสลับฮาร์ดแวร์วิทยุก่อนเปิด Wi-Fi Promiscuous
            wifiSniffActive = true;
            esp_wifi_set_promiscuous(true);
            currentPhase = PHASE_WIFI_SNIFF;
            phaseStartTimeMs = millis();
            lastWifiHopMs = millis();
        }
    }

    for (int i = 0; i < targetCount; i++) {
        if (targets[i].active && (now - targets[i].lastSeenMs > TARGET_TIMEOUT_MS)) {
            targets[i].active = false;
        }
    }
    delay(2); // คืนเวลาให้ FreeRTOS Idle / USB CDC Background Task ทำงานอย่างเสถียร
}

/*
 * ======================================================================================
 * 📡 EXP-03: ESP32-Nano BLE (Bluetooth Low Energy) RF Emission Sniffer
 * ======================================================================================
 * วัตถุประสงค์:
 * ดักจับคลื่นแม่เหล็กไฟฟ้าในย่าน Bluetooth Low Energy (2.4 GHz ISM Band)
 * ที่แผ่ออกมาจากสมาร์ตโฟน (Android / iPhone) เช่น:
 * 1. Google Fast Pair / Nearby Share / Quick Share Beacons (Xiaomi/Honor/Samsung)
 * 2. Apple Find My / Continuity Offline Beacons
 * 3. General BLE Advertisements (ADV_IND, ADV_NONCONN_IND)
 * 
 * 💡 ฟีเจอร์พิเศษ:
 * - สแกนความไวสูงแบบ Full Duty Cycle (Interval: 100ms, Window: 99ms)
 * - ปิด Duplicate Filter เพื่อวัดคาบเวลาการเต้นของชีพจรคลื่น (Pulse Interval: delta_t)
 * - ถอดรหัส Company Identifier (Apple, Google, Xiaomi, Samsung, Microsoft) อัตโนมัติ
 * - คุณสามารถพิมพ์ป้ายกำกับ เช่น POCO, HONOR, 1m, 3m ลงใน Serial Monitor ได้เลย!
 * ======================================================================================
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

uint32_t blePacketCount = 0;
unsigned long lastBlePacketTimeMs = 0;

// ป้ายกำกับปัจจุบัน (สามารถเปลี่ยนผ่าน Serial Monitor ได้ตลอดเวลา)
String currentTag = "SCANNING";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        blePacketCount++;
        unsigned long now = millis();

        unsigned long deltaMs = 0;
        if (lastBlePacketTimeMs > 0) {
            deltaMs = now - lastBlePacketTimeMs;
        }
        lastBlePacketTimeMs = now;

        int rssi = advertisedDevice.getRSSI();
        String macStr = String(advertisedDevice.getAddress().toString().c_str());
        
        // ตรวจสอบประเภท MAC (Random Private vs Public)
        // บิตที่ 6 และ 7 ของไบต์แรก หรือเช็คจาก BLE Address Type
        esp_ble_addr_type_t addrType = advertisedDevice.getAddressType();
        String macTypeStr = (addrType == BLE_ADDR_TYPE_RANDOM) ? "RANDOM" : "PUBLIC";

        // ถอดรหัสชื่ออุปกรณ์ (ถ้ามีส่งมาใน Scan Response)
        String devName = "Unknown";
        if (advertisedDevice.haveName()) {
            devName = String(advertisedDevice.getName().c_str());
            devName.replace(",", "_"); // ป้องกัน CSV เพี้ยน
        }

        // ถอดรหัสรหัสบริษัทผู้ผลิต (Manufacturer Company ID)
        String companyStr = "None";
        if (advertisedDevice.haveManufacturerData()) {
            std::string mData = advertisedDevice.getManufacturerData();
            if (mData.length() >= 2) {
                uint16_t companyId = (uint8_t)mData[0] | ((uint8_t)mData[1] << 8);
                char hexBuf[12];
                snprintf(hexBuf, sizeof(hexBuf), "0x%04X", companyId);
                companyStr = String(hexBuf);

                if (companyId == 0x004C) companyStr += "(Apple)";
                else if (companyId == 0x0075) companyStr += "(Samsung)";
                else if (companyId == 0x00E0) companyStr += "(Google)";
                else if (companyId == 0x038F) companyStr += "(Xiaomi)";
                else if (companyId == 0x0006) companyStr += "(Microsoft)";
                else if (companyId == 0x02E5) companyStr += "(Espressif)";
            }
        }

        // ตรวจสอบ Service UUID (เช่น Google Fast Pair, Find My)
        String serviceStr = "None";
        if (advertisedDevice.haveServiceUUID()) {
            serviceStr = String(advertisedDevice.getServiceUUID().toString().c_str());
            if (serviceStr.indexOf("fe9f") >= 0 || serviceStr.indexOf("FE9F") >= 0) {
                serviceStr = "0xFE9F(GoogleFastPair)";
            } else if (serviceStr.indexOf("feed") >= 0 || serviceStr.indexOf("FEED") >= 0) {
                serviceStr = "0xFEED(Tile/FindMy)";
            }
        }

        // ส่งออก CSV:
        // CSV_BLE,timestamp_ms,pkt_id,rssi_dbm,delta_t_ms,mac,mac_type,company,name,service,tag
        Serial.print("CSV_BLE,");
        Serial.print(now);
        Serial.print(",");
        Serial.print(blePacketCount);
        Serial.print(",");
        Serial.print(rssi);
        Serial.print(",");
        Serial.print(deltaMs);
        Serial.print(",");
        Serial.print(macStr);
        Serial.print(",");
        Serial.print(macTypeStr);
        Serial.print(",");
        Serial.print(companyStr);
        Serial.print(",\"");
        Serial.print(devName);
        Serial.print("\",\"");
        Serial.print(serviceStr);
        Serial.print("\",");
        Serial.println(currentTag);
    }
};

BLEScan* pBLEScan;

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n================================================================================");
    Serial.println("📡 EXP-03: ESP32-Nano Bluetooth Low Energy (BLE) RF Sniffer");
    Serial.println("CSV Header: timestamp_ms,pkt_id,rssi_dbm,delta_t_ms,mac,mac_type,company,name,service,tag");
    Serial.println("================================================================================");
    Serial.println("💡 คำแนะนำ: คุณสามารถพิมพ์ป้ายกำกับ เช่น POCO, HONOR, 1m ลงใน Serial Monitor แล้วกด Enter");
    Serial.print(">>> ป้ายกำกับเริ่มต้น: ");
    Serial.println(currentTag);
    Serial.println("--------------------------------------------------------------------------------\n");

    BLEDevice::init("Radar_Sniffer");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // ขอ Scan Response เพื่อดู Device Name
    pBLEScan->setInterval(100);    // สแกนทุก 100 ms
    pBLEScan->setWindow(99);       // เฝ้าฟัง 99 ms (99% Duty Cycle)
    pBLEScan->setDuplicateFilter(false); // สำคัญมาก: ไม่ตัดแพ็กเก็ตซ้ำ เพื่อดูความถี่การยิงจริง
}

void loop() {
    // ตรวจสอบข้อความจากผู้ใช้ผ่าน Serial Monitor เพื่อเปลี่ยน Tag
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            currentTag = input;
            Serial.println();
            Serial.print("🚩 >>> [TAG_CHANGED] เปลี่ยนป้ายกำกับเป็น: ");
            Serial.print(currentTag);
            Serial.println(" <<<");
            Serial.println();
        }
    }

    // เริ่มสแกนเป็นรอบๆ รอบละ 5 วินาที แล้ววนสแกนต่อทันทีแบบ Non-blocking
    pBLEScan->start(5, false);
    pBLEScan->clearResults(); // เคลียร์บัฟเฟอร์ในแรมเพื่อไม่ให้ล้น
}

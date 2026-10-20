/*
 * ======================================================================================
 * 📡 EXP-03: ESP32-Nano BLE (Bluetooth Low Energy) RF Emission Sniffer (v2 - Diagnostic)
 * ======================================================================================
 * วัตถุประสงค์:
 * ดักจับคลื่นแม่เหล็กไฟฟ้าในย่าน Bluetooth Low Energy (2.4 GHz ISM Band)
 * ที่แผ่ออกมาจากสมาร์ตโฟนและอุปกรณ์รอบข้าง เช่น:
 * 1. Google Fast Pair / Quick Share / Nearby Share (เมื่อเปิดแชร์ใกล้เคียงหรือปลดล็อกจอ)
 * 2. Honor Share / Xiaomi Mi Share Beacons
 * 3. Apple Find My / Continuity (iPhone / iPad / AirPods / AirTag)
 * 4. Smartwatch / Mi Band / หูฟังไร้สาย TWS
 * 
 * 🔧 สิ่งที่ปรับปรุงใน v2:
 * - เพิ่มระบบ Heartbeat แจ้งสถานะทุกๆ 3 วินาที (ไม่ปล่อยให้หน้าจอขาวว่างเปล่า)
 * - ปรับแต่ง Window/Interval (80/100 ms) ให้เสถียรและไม่บล็อกคิวส่งของ USB CDC
 * - พิมพ์อะไรก็ได้ลงใน Serial Monitor แล้วกด Enter เพื่อเปลี่ยนป้ายกำกับหรือดูเมนูได้ทันที
 * ======================================================================================
 */

#include <Arduino.h>
#include <nvs_flash.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

const int SCAN_TIME_SEC = 3; // สแกนรอบละ 3 วินาที

uint32_t totalBlePackets = 0;
uint32_t cyclePackets = 0;
uint32_t scanCycleCount = 0;
unsigned long lastBlePacketTimeMs = 0;

String currentTag = "BLE_SCAN";

void printHeader() {
    Serial.println("\n================================================================================");
    Serial.println("📡 EXP-03 (v2): ESP32-Nano Bluetooth Low Energy (BLE) RF Sniffer");
    Serial.println("CSV Header: timestamp_ms,pkt_id,rssi_dbm,delta_t_ms,mac,mac_type,company,name,service,tag");
    Serial.println("================================================================================");
    Serial.println("💡 ทริคการทดสอบ BLE บนมือถือ Android (Honor / POCO):");
    Serial.println("   1. เปิด Bluetooth และลองเปิดเมนู 'Quick Share / แชร์ใกล้เคียง' หรือหน้าจับคู่บลูทูธ");
    Serial.println("   2. หรือลองเปิดฝากล่องหูฟังไร้สาย / นาฬิกาสมาร์ตวอทช์ ใกล้ๆ บอร์ด");
    Serial.println("--------------------------------------------------------------------------------\n");
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        totalBlePackets++;
        cyclePackets++;
        unsigned long now = millis();

        unsigned long deltaMs = 0;
        if (lastBlePacketTimeMs > 0) {
            deltaMs = now - lastBlePacketTimeMs;
        }
        lastBlePacketTimeMs = now;

        int rssi = advertisedDevice.getRSSI();
        String macStr = String(advertisedDevice.getAddress().toString().c_str());
        macStr.toUpperCase();

        esp_ble_addr_type_t addrType = advertisedDevice.getAddressType();
        String macTypeStr = (addrType == BLE_ADDR_TYPE_RANDOM) ? "RANDOM" : "PUBLIC";

        // ชื่ออุปกรณ์
        String devName = "Unknown";
        if (advertisedDevice.haveName()) {
            devName = String(advertisedDevice.getName().c_str());
            devName.replace(",", "_");
        }

        // รหัสบริษัทผู้ผลิต (Manufacturer Specific Data)
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
                else if (companyId == 0x027D) companyStr += "(Huawei/Honor)";
            }
        }

        // Service UUID (เช่น Fast Pair, Nearby Share, Mi Share)
        String serviceStr = "None";
        if (advertisedDevice.haveServiceUUID()) {
            serviceStr = String(advertisedDevice.getServiceUUID().toString().c_str());
            if (serviceStr.indexOf("fe9f") >= 0 || serviceStr.indexOf("FE9F") >= 0) {
                serviceStr = "0xFE9F(GoogleFastPair)";
            } else if (serviceStr.indexOf("fef3") >= 0 || serviceStr.indexOf("FEF3") >= 0) {
                serviceStr = "0xFEF3(GoogleNearby/QuickShare)";
            } else if (serviceStr.indexOf("fd5a") >= 0 || serviceStr.indexOf("FD5A") >= 0) {
                serviceStr = "0xFD5A(SamsungFind)";
            } else if (serviceStr.indexOf("feed") >= 0 || serviceStr.indexOf("FEED") >= 0) {
                serviceStr = "0xFEED(Tile/FindMy)";
            }
        }

        // ส่งออก CSV ทันทีที่จับได้:
        Serial.print("CSV_BLE,");
        Serial.print(now);
        Serial.print(",");
        Serial.print(totalBlePackets);
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

BLEScan* pBLEScan = nullptr;

void setup() {
    Serial.begin(115200);
    // รอให้ USB CDC เชื่อมต่อกับคอมพิวเตอร์สูงสุด 3 วินาที
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 3000)) {
        delay(50);
    }
    delay(500);

    nvs_flash_init();
    printHeader();

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
    pBLEScan->setActiveScan(true); // ส่ง SCAN_REQ เพื่อดึงชื่อและข้อมูลเพิ่มเติม
    pBLEScan->setInterval(100);    // 100 ms
    pBLEScan->setWindow(80);       // 80 ms (เว้น 20 ms ให้ส่ง SCAN_REQ และคืนคิว USB Serial)

    Serial.println("✅ [SYSTEM] เริ่มต้นระบบ BLE Radar สำเร็จ! กำลังเริ่มกวาดคลื่น...\n");
}

void loop() {
    // รับคำสั่งเปลี่ยนป้ายกำกับจาก Serial Monitor
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            if (input.equalsIgnoreCase("help") || input.equalsIgnoreCase("h")) {
                printHeader();
            } else {
                currentTag = input;
                Serial.println();
                Serial.print("🚩 >>> [TAG_CHANGED] เปลี่ยนป้ายกำกับเป็น: ");
                Serial.print(currentTag);
                Serial.println(" <<<");
                Serial.println();
            }
        }
    }

    scanCycleCount++;
    cyclePackets = 0;

    // สแกนเป็นเวลา 3 วินาที
    pBLEScan->start(SCAN_TIME_SEC, false);

    // หากในรอบ 3 วินาทีนี้ไม่มีคลื่นเลย ให้พิมพ์ Heartbeat บอกว่าบอร์ดยังทำงานปกติ!
    if (cyclePackets == 0) {
        Serial.print("⏳ [BLE_HEARTBEAT รอบที่ #");
        Serial.print(scanCycleCount);
        Serial.print("] กำลังฟังคลื่น 2.4GHz BLE... (ยังไม่พบการปล่อย Beacon ในรอบนี้ | สะสมรวม: ");
        Serial.print(totalBlePackets);
        Serial.println(" แพ็กเก็ต)");
    }

    pBLEScan->clearResults();
    delay(50);
}

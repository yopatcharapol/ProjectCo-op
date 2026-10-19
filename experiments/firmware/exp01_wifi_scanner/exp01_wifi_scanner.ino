/*
 * EXP-01: ESP32-Nano Wi-Fi Scanner with Structured CSV Output
 * 
 * บอร์ดเป้าหมาย: Arduino Nano ESP32 / ESP32-Nano
 * วัตถุประสงค์: สแกนหาคลื่น Wi-Fi 2.4 GHz และส่งออกข้อมูลเป็น CSV ผ่าน Serial Monitor
 * เพื่อให้สามารถคัดลอกไปบันทึกลงไฟล์ในโฟลเดอร์ experiments/raw_logs/ ได้ทันที
 */

#include <WiFi.h>

const unsigned long SCAN_INTERVAL_MS = 5000; // รอบการสแกนทุกๆ 5 วินาที
unsigned long lastScanTime = 0;
uint32_t scanCounter = 0;

void setup() {
  Serial.begin(115200);
  
  // หน่วงเวลารอ Serial Monitor เชื่อมต่อ (สำหรับบอร์ด USB Native อย่าง Nano ESP32)
  unsigned long startWait = millis();
  while (!Serial && (millis() - startWait < 3000)) {
    delay(10);
  }

  Serial.println();
  Serial.println("================================================================================");
  Serial.println("📡 ESP32-Nano Wi-Fi Scanner Initiated");
  Serial.println("CSV Header Format: timestamp_ms,scan_id,bssid,rssi_dbm,channel,encryption,ssid");
  Serial.println("================================================================================");

  // ตั้งค่าเป็นโหมด Station (STA) และตัดการเชื่อมต่อกับเราเตอร์เดิม
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastScanTime >= SCAN_INTERVAL_MS) {
    lastScanTime = currentMillis;
    scanCounter++;

    Serial.print(">>> [SCAN_START] ScanID: ");
    Serial.print(scanCounter);
    Serial.print(" at ");
    Serial.print(currentMillis);
    Serial.println(" ms");

    // เริ่มทำการสแกนเครือข่าย Wi-Fi (blocking scan)
    int networkCount = WiFi.scanNetworks();

    if (networkCount == 0) {
      Serial.println("CSV: NO_NETWORKS_FOUND");
    } else {
      for (int i = 0; i < networkCount; ++i) {
        // พิมพ์ข้อมูลในรูปแบบ CSV เพื่อนำไปพล็อตกราฟหรือคำนวณง่ายๆ
        Serial.print("CSV,");
        Serial.print(millis());
        Serial.print(",");
        Serial.print(scanCounter);
        Serial.print(",");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.print(",");
        Serial.print(WiFi.RSSI(i));
        Serial.print(",");
        Serial.print(WiFi.channel(i));
        Serial.print(",");
        Serial.print(WiFi.encryptionType(i));
        Serial.print(",\"");
        Serial.print(WiFi.SSID(i));
        Serial.println("\"");
      }
    }

    Serial.print("<<< [SCAN_END] Found ");
    Serial.print(networkCount);
    Serial.println(" networks\n");

    // เคลียร์หน่วยความจำผลการสแกน
    WiFi.scanDelete();
  }
}

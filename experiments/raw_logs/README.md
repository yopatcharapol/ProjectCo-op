# 📁 โฟลเดอร์เก็บข้อมูลดิบจากการทดลอง (Raw Logs & CSV Data)

โฟลเดอร์นี้ใช้สำหรับจัดเก็บไฟล์ข้อมูลดิบ (Raw Data) ที่ได้จาก Serial Monitor ของบอร์ด ESP32-Nano หรือบันทึกผ่านสคริปต์

---

## 🏷️ กฎการตั้งชื่อไฟล์ (File Naming Convention)

ตั้งชื่อไฟล์ด้วยรูปแบบ:
```text
YYYY-MM-DD_<รหัสการทดลอง>_<เงื่อนไขหรือระยะทาง>.csv
```

**ตัวอย่างชื่อไฟล์:**
* `2026-10-19_exp01_wifi_1m_LOS.csv` (สแกนที่ระยะ 1 เมตร โล่งสายตา)
* `2026-10-19_exp01_wifi_5m_NLOS.csv` (สแกนที่ระยะ 5 เมตร มีสิ่งกีดขวาง)
* `2026-10-19_exp01_wifi_hotspot_walkaway.csv` (ทดสอบถือโทรศัพท์เดินออกห่าง)

---

## 📊 ตัวอย่างฟอร์แมตข้อมูลในไฟล์ CSV (`exp01`)

```csv
timestamp_ms,scan_id,bssid,rssi_dbm,channel,encryption,ssid
15200,1,AA:BB:CC:DD:EE:01,-48,1,4,"iPhone_Hotspot"
15240,1,11:22:33:44:55:02,-72,6,4,"Lab_WiFi"
20200,2,AA:BB:CC:DD:EE:01,-50,1,4,"iPhone_Hotspot"
20240,2,11:22:33:44:55:02,-75,6,4,"Lab_WiFi"
```

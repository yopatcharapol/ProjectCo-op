# 🧪 รายงานผลการทดลองวิจัย: การดักจับคลื่นวิทยุสมาร์ตโฟนรอบด้าน (EXP-04: Omni-Frame Wi-Fi Sniffer)
## (Comprehensive 802.11 Multi-Frame Sniffing: Management, QoS Null & Action Frames)

<div align="center">

![Experiment: EXP-04](https://img.shields.io/badge/Experiment-EXP--04-blue?style=for-the-badge)
![Target: 802.11 All Frame Types](https://img.shields.io/badge/Target-802.11%20Omni--Frames-orange?style=for-the-badge)
![Focus: All Phone RF Emissions](https://img.shields.io/badge/Focus-Multi--RF%20Radar-green?style=for-the-badge)
![Status: Ready for Field Testing](https://img.shields.io/badge/Status-Ready%20for%20Field%20Testing-yellow?style=for-the-badge)

</div>

* **รหัสการทดลอง:** `EXP-04-OMNI-WIFI-SNIFFER`
* **วันที่เริ่มเก็บข้อมูล:** 2026-10-20
* **ฮาร์ดแวร์ภาครับ (Receiver):** Arduino Nano ESP32 (ชิป ESP32-S3)
* **เฟิร์มแวร์:** [`experiments/firmware/exp04_omni_wifi_sniffer/exp04_omni_wifi_sniffer.ino`](file:///d:/ProjectCo-op/experiments/firmware/exp04_omni_wifi_sniffer/exp04_omni_wifi_sniffer.ino)
* **ไฟล์บันทึกข้อมูลดิบ:** [`experiments/raw_logs/exp04_omni_wifi_results.csv`](file:///d:/ProjectCo-op/experiments/raw_logs/exp04_omni_wifi_results.csv)

---

## 1. วัตถุประสงค์และสมมติฐานทางวิทยาศาสตร์ (Objectives & Hypotheses)

การทดลองก่อนหน้า (DATASET A, B, C) ตรวจจับเฉพาะ **Probe Request (Subtype 0x04)** ซึ่งมีข้อจำกัดทางกายภาพคือ:
* หากผู้ประสบภัยเชื่อมต่อ Wi-Fi อยู่ (เช่น Wi-Fi ออฟฟิศ, Pocket Wi-Fi ในรถ, หรือจุดปล่อยสัญญาณฉุกเฉิน) โทรศัพท์จะ **หยุดส่ง Probe Request** เกือบทั้งหมด
* หากผู้ประสบภัยเปิด Personal Hotspot เพื่อพยายามแชร์สัญญาณ สัญญาณจะกลายเป็น **Beacon Frame (Subtype 0x08)**
* หากผู้ประสบภัยพยายามส่งรูปถ่าย หรือเปิด Quick Share / Wi-Fi Direct คลื่นจะแผ่ออกมาในรูป **Action Frame (Subtype 0x0D)**
* ในสภาวะสแตนด์บายเชื่อมต่อเน็ต โทรศัพท์จะยิงพัลส์ **QoS Null Data Frame (Subtype 0x0C)** เพื่อรักษา Power Management State

### 🎯 เป้าหมายของ EXP-04:
ปลดล็อกการดักจับคลื่นทุกความถี่ที่แผ่ออกจากสมาร์ตโฟนครอบคลุม 5 ประเภทหลัก:
1. `PROBE_REQ` (Management Subtype 4): คลื่นถามหาเครือข่ายขณะตัดการเชื่อมต่อ
2. `ACTION_P2P` (Management Subtype 13): คลื่นจับคู่อุปกรณ์ระยะประชิด (Wi-Fi Direct / Quick Share)
3. `QOS_NULL` (Data Subtype 12): คลื่นชีพจรเต้นเบาๆ รักษาการประหยัดพลังงานเมื่อเครื่องต่อเน็ต
4. `HOTSPOT_AP` (Management Subtype 8): คลื่นกระจายสัญญาณเมื่อเปิด Hotspot
5. `DATA_UPLINK` (Data Subtype 0 / 8): คลื่นข้อมูลการส่งข้อความ/แจ้งเตือนของสมาร์ตโฟน

---

## 2. ตารางผลการทดสอบเชิงประจักษ์ (Empirical Observation Table)

*(กำลังรอข้อมูลจากการทดสอบจริง)*

| หมวดหมู่คลื่น (Category) | IEEE 802.11 Frame Type / Subtype | ทิศทางลิงก์ (Link Direction) | บทบาทเชิงคลื่นวิทยุ | ผลกระทบต่อการตรวจจับกู้ภัย (SAR Radar Impact) |
| :--- | :---: | :---: | :--- | :--- |
| **PROBE_REQ** | Type 0 / Subtype 4 | Client Broadcast | ค้นหา AP แบบอิสระ | ตรวจจับผู้ประสบภัยที่ปิดเน็ต / จอดับ |
| **ACTION_P2P** | Type 0 / Subtype 13 | Direct P2P | ส่งข้อมูล Quick Share / Wi-Fi Direct | ตรวจจับตอนพยายามแชร์พิกัด/ไฟล์ |
| **QOS_NULL** | Type 2 / Subtype 12 | TO_DS (Client ➔ AP) | พัลส์ Keep-Alive สแตนด์บาย | **ตรวจจับผู้ประสบภัยที่เครื่องต่อ Wi-Fi ค้างอยู่!** |
| **HOTSPOT_AP** | Type 0 / Subtype 8 | AP Broadcast | เปิด Hotspot แชร์เน็ต | ตรวจจับผู้ประสบภัยที่เปิด Hotspot ขอความช่วยเหลือ |
| **DATA_UPLINK** | Type 2 / Subtype 0, 8 | TO_DS (Client ➔ AP) | การส่งข้อมูลขึ้นสู่ระบบ | ตรวจจับทันทีที่มีการส่งแอปพลิเคชัน |

---

## 3. บทวิเคราะห์ทางวิศวกรรม (Engineering Insights)
*(จะได้รับการอัปเดตทันทีเมื่อบันทึกข้อมูลจาก Serial Monitor ลงสู่ CSV)*

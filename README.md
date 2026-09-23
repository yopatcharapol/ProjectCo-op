<div align="center">

# 📡 Project Co-op
### ระบบค้นหาและระบุตำแหน่งผู้ประสบภัยด้วย Passive Smartphone RF Sniffing
**Passive Smartphone RF Sniffing for Search and Rescue (SAR) Localization**

[![Project Status](https://img.shields.io/badge/Status-Active%20Research-blue?style=for-the-badge&logo=git)](https://github.com/yopatcharapol/ProjectCo-op)
[![Platform](https://img.shields.io/badge/Hardware-ESP32%20%7C%20Arduino-red?style=for-the-badge&logo=espressif)](share_friend/Experiments/01_03_wifi_sniffer/)
[![Standard](https://img.shields.io/badge/Standard-IEEE%20802.11--2024-success?style=for-the-badge)](share_friend/Research/)
[![Docs](https://img.shields.io/badge/Docs-Obsidian%20Vault-purple?style=for-the-badge&logo=obsidian)](share_friend/)

---

<p align="center">
  <b>การค้นหาผู้ประสบภัยหรือผู้หลงป่าผ่านคลื่นสัญญาณวิทยุที่สมาร์ตโฟนแผ่ออกมาตามธรรมชาติ (Unintentional & Spontaneous RF Emissions) โดยไม่ต้องพึ่งพาเครือข่ายมือถือ (Cellular Base Station) และไม่ต้องติดตั้งแอปพลิเคชันใดๆ ลงบนเครื่องเป้าหมาย</b>
</p>

</div>

---

## 🎯 ไฮไลต์และหลักการทำงานของโครงการ (Core Highlights)

> [!IMPORTANT]
> **หลักการพื้นฐาน (Zero-App & Passive Sensing):**  
> แม้ว่าผู้ประสบภัยจะไม่มีสัญญาณโทรศัพท์ ไม่มีอินเทอร์เน็ต และหน้าจอดับอยู่ ชิปเซ็ต Wi-Fi/Bluetooth ในสมาร์ตโฟนจะยังคง "ส่งสัญญาณค้นหาเครือข่าย" (**Active Scan / Probe Request**) ออกมาในอากาศเป็นระยะๆ ตามธรรมชาติ ระบบ Sniffer ของเราจะดักรับสัญญาณนี้เพื่อนำค่าความแรงสัญญาณ (**RSSI**) ไปคำนวณหาระยะทางและระบุพิกัดตำแหน่ง (**Localization**)

```
 [📱 สมาร์ตโฟนผู้ประสบภัย] (หน้าจอดับ / ไม่มีเน็ต)
           │
           │  (แผ่คลื่น Probe Request ทุกๆ 5-60 วินาที)
           ▼
 ┌─────────────────────────────────────────────────────────┐
 │   🛰️ สถานีดักจับพาสซีฟ (Passive Sniffer Nodes: ESP32)     │
 │   • Promiscuous Mode ดักจับเฟรม 802.11 Management       │
 │   • ตรวจวัดค่า RSSI (Signal Strength)                   │
 │   • ถอดรหัส Sequence Number เพื่อรับมือ MAC Randomization   │
 └─────────────────────────────────────────────────────────┘
           │
           ▼
 ┌─────────────────────────────────────────────────────────┐
 │   📍 อัลกอริทึมประมวลผลพิกัด (Localization Engine)        │
 │   • RSSI-to-Distance Path Loss Modeling                 │
 │   • Multilateration / Trilateration (หาพิกัดกู้ภัย)         │
 └─────────────────────────────────────────────────────────┘
```

---

## 📂 โครงสร้างคลังข้อมูล (Repository Structure)

```
ProjectCo-op/
├── 📁 share_friend/                           <-- 🌟 [คลังเอกสารและแล็บที่กำลังทำงานอยู่ - Sync กับเพื่อน]
│   ├── 🧪 Experiments/01_03_wifi_sniffer/    <-- ซอร์สโค้ดเฟิร์มแวร์ ESP32 Sniffer + คู่มือ Lab Guide
│   ├── 📚 Research/                          <-- สรุปเนื้อหาวิชาการ IEEE 802.11 และงานวิจัยต่างประเทศ
│   ├── 📋 Todo/                              <-- กระดานสถานะความคืบหน้า (Kanban Board)
│   └── 📑 Wi-Fi Study Guide.md               <-- แผนที่การศึกษา 16 หัวข้อหลักสำหรับปริญญานิพนธ์
│
├── 📁 docs/                                   <-- เอกสารสถาปัตยกรรมและบทวิเคราะห์คลื่นวิทยุ
│   ├── 08_smartphone_rf_sensing.md           <-- [แกนหลัก 1] สถาปัตยกรรม Passive Smartphone RF Sniffing
│   └── 09_smartphone_rf_signals_analysis.md   <-- [แกนหลัก 2] บทวิเคราะห์คลื่นวิทยุสมาร์ตโฟนเชิงลึก
│
├── 📁 reports_and_notes/                      <-- บันทึกการวิจัยประจำวันและรายงานนำเสนออาจารย์
│   ├── ADVISOR_REPORT.md                     <-- สรุปรายงานยุทธศาสตร์ฉบับนำเสนออาจารย์ที่ปรึกษา
│   └── 2026-09-13.md                         <-- บันทึกการวิจัย Kickoff Core Project
│
├── 📁 process_and_workflow/                   <-- แผนภูมิและกระบวนการทำงานวิศวกรรม
└── 📁 archive_initial_research/               <-- คลังเก็บงานวิจัยและเอกสารตั้งต้น
```

---

## 🔬 เฟิร์มแวร์และการทดลองจริง (Hands-On Labs)

โค้ดตัวอย่างและคู่มือปฏิบัติการทดลองจริงบนฮาร์ดแวร์ **ESP32**:

| ลำดับ | รายการ | รายละเอียด | ลิงก์เข้าสู่เอกสาร |
| :---: | :--- | :--- | :--- |
| **01** | **ESP32 Wi-Fi Sniffer Code** | ซอร์สโค้ด Arduino ดักจับ Probe Request, แยก MAC Randomization, บันทึก Sequence Number | [`esp32_wifi_sniffer.ino`](share_friend/Experiments/01_03_wifi_sniffer/esp32_wifi_sniffer.ino) |
| **02** | **Lab Guide 01–03** | คู่มือการทดลอง 4 ขั้นตอน: วัด RSSI vs ระยะทาง, ทดสอบ Doze Mode, ส่อง MAC Randomization | [`LAB_GUIDE_01_03.md`](share_friend/Experiments/01_03_wifi_sniffer/LAB_GUIDE_01_03.md) |
| **03** | **Wi-Fi Study Guide** | แผนที่การศึกษา 16 สาระสำคัญของ IEEE 802.11 สำหรับวิทยานิพนธ์ | [`Wi-Fi Study Guide.md`](share_friend/Wi-Fi%20Study%20Guide.md) |

---

## 📑 สรุปงานวิจัยที่สำคัญ (Key Research Modules)

* 📡 **[IEEE 802.11 Scanning](share_friend/Research/สรุป/IEEE%20802.11%20Scanning/01_IEEE_802.11_Scanning.md):** กลไกการกวาดหาสัญญาณของสมาร์ตโฟน (Active vs Passive Scan)
* 📬 **[Probe Request Frames](share_friend/Research/สรุป/IEEE%20802.11%20Scanning/02_Probe_Request.md):** โครงสร้างเฟรม 802.11 Management Subtype `0x0040` และพฤติกรรมการส่ง
* 🔄 **[Passive vs Active Trade-offs](share_friend/Research/สรุป/IEEE%20802.11%20Scanning/03_Passive_vs_Active_Scan.md):** การเปรียบเทียบข้อจำกัดด้านพลังงานและโอกาสในการตรวจจับ
* ⚡ **[Unconventional RF & Side-Channel](share_friend/Research/สรุป/02_Smartphone_Signals_and_Unconventional_Emissions.md):** บทวิเคราะห์คลื่นแม่เหล็กไฟฟ้าและการรั่วไหลทางกายภาพ (SMPS, OLED PWM, MagSafe, NLJD)
* 📚 **[Academic Papers Synthesis](share_friend/Research/yo_research/yopaper.md):** สรุปเปเปอร์วิชาการระดับนานาชาติ 5 ฉบับ (ACM IMC, ACM WiSec) และมาตรฐาน IEEE Std 802.11-2024

---

<div align="center">
  <sub>โครงงานวิจัยสหกิจศึกษา (Cooperative Education Project) · อัปเดตล่าสุด: กันยายน 2026</sub>
</div>
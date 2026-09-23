<div align="center">

# 📡 Project Co-op
### การศึกษาความเป็นไปได้: การตรวจจับคลื่นวิทยุสมาร์ตโฟนและโครงข่ายสื่อสารฉุกเฉิน
**Feasibility Study: Passive Smartphone RF Sensing & Emergency Mesh Communications**

[![Phase](https://img.shields.io/badge/Phase-Topic%20Discovery%20%26%20Feasibility-orange?style=for-the-badge&logo=compass)](PROJECT_EVOLUTION.md)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32%20%7C%20LoRa%20%7C%20Arduino-red?style=for-the-badge&logo=espressif)](share_friend/Experiments/01_03_wifi_sniffer/)
[![Standards](https://img.shields.io/badge/Standard-IEEE%20802.11--2024%20%7C%20LoRaWAN-blue?style=for-the-badge)](share_friend/Research/)
[![Docs](https://img.shields.io/badge/Docs-Obsidian%20Vault-purple?style=for-the-badge&logo=obsidian)](share_friend/)

---

<p align="center">
  <b>โครงการสหกิจศึกษา (Project Co-op) อยู่ระหว่างการศึกษาความเป็นไปได้ (Feasibility Study)<br>
  เพื่อพัฒนาหัวข้อวิจัยระหว่าง "ระบบตรวจจับคลื่นวิทยุสมาร์ตโฟนพาสซีฟ (Sensing Layer)" และ "โครงข่ายส่งสัญญาณฉุกเฉิน LoRa Mesh (Backhaul Layer)"</b>
</p>

</div>

---

## 🧭 ลำดับขั้นตอนและวิวัฒนาการของโครงงาน (Project Evolution)

> [!NOTE]
> เอกสารใน Repository นี้ประกอบด้วยทั้งงานวิจัย **LoRa Mesh** และ **Smartphone RF Sensing** ซึ่งไม่ใช่เอกสารที่ปนกันโดยบังเอิญ แต่เป็น **"ร่องรอยการพัฒนาหัวข้อวิจัย (Research Footprints)"** ตามคำแนะนำของอาจารย์ที่ปรึกษา  
> 👉 **[อ่านเอกสารสรุปไทม์ไลน์และเกณฑ์การตัดสินใจฉบับเต็มได้ที่: `PROJECT_EVOLUTION.md`](PROJECT_EVOLUTION.md)**

```
 [Phase 0: จุดเริ่มต้น]          [Phase 1: คำแนะนำอาจารย์]         [Phase 2: ศึกษาความเป็นไปได้]       [Phase 3: สรุปหัวข้อวิจัย]
 ศึกษาระบบสื่อสารฉุกเฉิน   ──►   "สมาร์ตโฟนปล่อยคลื่นอะไร?   ──►   ทดลองบนบอร์ด ESP32 จริง     ──►   ตัดสินใจเลือก Scope ปริญญานิพนธ์
 ด้วย LoRa Mesh                  จะตรวจจับได้อย่างไร?"           (Wi-Fi Probe Requests / BLE)      (RF Sensing vs Localization vs Mesh)
 (Docs 01–07)                    (Docs 08–09, IEEE 802.11)       (Lab Guide 01–03) [📍ปัจจุบัน]
```

---

## 🧩 ความสัมพันธ์เชิงสถาปัตยกรรม (Architectural Synergy)

ทั้งสองเทคโนโลยีสามารถผสานรวมกันเป็นโซลูชันค้นหาและกู้ภัย (**Search and Rescue - SAR**) ที่สมบูรณ์แบบ:

```
  ┌──────────────────────────────────────────────────────────────────┐
  │ 📡 1. SENSING LAYER: การตรวจจับเป้าหมายแบบพาสซีฟ (Zero-App)          │
  │ • สมาร์ตโฟนผู้ประสบภัยแผ่คลื่น Probe Request แม้หน้าจอดับหรือไม่มีเน็ต     │
  │ • โหนด ESP32 ดักจับเฟรม 802.11 และวัดระดับความแรงสัญญาณ (RSSI)        │
  └─────────────────────────────────┬────────────────────────────────┘
                                    │ ส่งพิกัดและค่า RSSI
                                    ▼
  ┌──────────────────────────────────────────────────────────────────┐
  │ 🌐 2. BACKHAUL LAYER: โครงข่ายส่งข้อมูลระยะไกลผ่าน LoRa Mesh      │
  │ • ถ่ายทอดข้อมูลพิกัดข้ามป่า/เขา ระหว่างโหนดกู้ภัยโดยไม่ต้องพึ่งพาเสามือถือ    │
  │ • รวมข้อมูลสู่ศูนย์บัญชาการเพื่อคำนวณตำแหน่งพิกัดผู้ประสบภัย (Trilateration)│
  └──────────────────────────────────────────────────────────────────┘
```

---

## 📂 โครงสร้างการจัดหมวดหมู่เอกสาร (Repository Structure)

```
ProjectCo-op/
├── 🧭 PROJECT_EVOLUTION.md                    <-- [เอกสารสำคัญ] ไทม์ไลน์วิจัย 4 เฟสและเกณฑ์การเลือกหัวข้อ
├── 📖 README.md                               <-- [คุณอยู่ที่นี่] ภาพรวมโครงการและสารบัญหลัก
│
├── 📁 share_friend/                           <-- 🌟 [คลังค้นคว้าหลัก Phase 2 - กำลังศึกษาและทดลองร่วมกัน]
│   ├── 🧪 Experiments/01_03_wifi_sniffer/    <-- โค้ดเฟิร์มแวร์ ESP32 Sniffer + คู่มือ Lab Guide 01–03
│   ├── 📚 Research/                          <-- สรุปเจาะลึกมาตรฐาน IEEE 802.11 และ 5 เปเปอร์สากล
│   ├── 📋 Todo/                              <-- กระดานติดตามงาน (Kanban Board)
│   └── 📑 Wi-Fi Study Guide.md               <-- แผนที่การศึกษา 16 สาระสำคัญของ Wi-Fi
│
├── 📁 docs/                                   <-- เอกสารสเปกหลักด้าน Smartphone RF Sensing (Docs 08–09)
├── 📁 reports_and_notes/                      <-- บันทึกการวิจัยประจำวันและรายงานนำเสนออาจารย์ที่ปรึกษา
├── 📁 process_and_workflow/                   <-- แผนภูมิและกระบวนการทำงานวิศวกรรม
└── 📁 archive_initial_research/               <-- คลังงานวิจัยตั้งต้น Phase 0: LoRa Mesh (Docs 01–07 & เปเปอร์เดิม)
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

* 🧭 **[Project Evolution & Timeline](PROJECT_EVOLUTION.md):** บันทึกการเดินทางทางวิชาการและกรอบการตัดสินใจ
* 📡 **[IEEE 802.11 Scanning](share_friend/Research/สรุป/IEEE%20802.11%20Scanning/01_IEEE_802.11_Scanning.md):** กลไกการกวาดหาสัญญาณของสมาร์ตโฟน (Active vs Passive Scan)
* 📬 **[Probe Request Frames](share_friend/Research/สรุป/IEEE%20802.11%20Scanning/02_Probe_Request.md):** โครงสร้างเฟรม 802.11 Management Subtype `0x0040` และพฤติกรรมการส่ง
* ⚡ **[Unconventional RF & Side-Channel](share_friend/Research/สรุป/02_Smartphone_Signals_and_Unconventional_Emissions.md):** บทวิเคราะห์คลื่นแม่เหล็กไฟฟ้าและการรั่วไหลทางกายภาพ (SMPS, OLED PWM, MagSafe, NLJD)
* 🌐 **[Initial LoRa Mesh Matrix](archive_initial_research/research_notes/comparison_matrix.md):** การวิเคราะห์เปรียบเทียบโปรโตคอล LoRa Mesh ตั้งต้น 4 รูปแบบ

---

<div align="center">
  <sub>โครงงานวิจัยสหกิจศึกษา (Cooperative Education Project) · กำลังศึกษาความเป็นไปได้ (Feasibility Study)</sub>
</div>
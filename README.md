# 📡 Project Co-op: ระบบค้นหาและระบุตำแหน่งผู้ประสบภัยด้วย Passive Smartphone RF Sniffing & LoRa Mesh

ยินดีต้อนรับสู่คลังความรู้และเอกสารข้อกำหนดทางวิศวกรรม **โครงงานสหกิจศึกษา (Project Co-op)**  
โฟลเดอร์โครงการ: [`d:\ProjectCo-op`](file:///d:/ProjectCo-op) (และ [`d:\ProjectLoRa`](file:///d:/ProjectLoRa))

> 🚀 **เอกสารสเปกหลักของโครงการปัจจุบัน (Active Core Specs):**
> 1. [**`docs/08_smartphone_rf_sensing.md`**](file:///d:/ProjectLoRa/docs/08_smartphone_rf_sensing.md): สถาปัตยกรรมระบบดักจับคลื่นสมาร์ตโฟนพาสซีฟ และการเชื่อมต่อส่งผ่าน LoRa Mesh สู่การทำ Trilateration
> 2. [**`docs/09_smartphone_rf_signals_analysis.md`**](file:///d:/ProjectLoRa/docs/09_smartphone_rf_signals_analysis.md): บทวิเคราะห์คลื่นสัญญาณวิทยุทั้งหมดที่สมาร์ตโฟนแผ่ออกมา (Wi-Fi, BLE, Cellular, UWB) และกลยุทธ์การตรวจจับบน ESP32
> 3. [**`ADVISOR_REPORT.md`**](file:///d:/ProjectCo-op/reports_and_notes/ADVISOR_REPORT.md): สรุปรายงานยุทธศาสตร์และพิมพ์เขียวฉบับนำเสนออาจารย์ที่ปรึกษา

---

## 📂 โครงสร้างการจัดหมวดหมู่ไฟล์ (Directory Structure)

```
ProjectCo-op/
│
├── README.md                                 <-- [คุณอยู่ที่นี่] สารบัญหลักและคู่มือการใช้งาน
│
├── reports_and_notes/                        <-- [รวบรวมรายงานและบันทึกประจำวัน]
│   ├── ADVISOR_REPORT.md                     <-- สรุปรายงานยุทธศาสตร์สำหรับนำเสนออาจารย์
│   ├── 2026-09-13.md                         <-- บันทึกการวิจัยประจำวัน (Kickoff Core Project)
│   ├── 2026-09-18.md                         <-- บันทึกการวิจัยประจำวัน (RF Emissions, Checkpoint & SX1262)
│   ├── Untitled.base                         <-- Obsidian Table View Base
│   └── Untitled.canvas                       <-- Obsidian Canvas
│
├── docs/                                     <-- [เอกสารงานวิจัยหลักของโครงการปัจจุบัน]
│   ├── 08_smartphone_rf_sensing.md           <-- [แกนหลัก 1] สถาปัตยกรรม Passive Smartphone RF Sniffing & LoRa Mesh
│   └── 09_smartphone_rf_signals_analysis.md   <-- [แกนหลัก 2] บทวิเคราะห์คลื่นวิทยุสมาร์ตโฟนเชิงลึก (Wi-Fi, BLE, Cellular, UWB)
│
├── assets/images/                            <-- รูปภาพ 3D Concept Architecture และไดอะแกรมสถาปัตยกรรม
│
├── process_and_workflow/                     <-- แผนและกระบวนการทำงานจริง (Roadmap & Logs)
│   ├── project_roadmap.md                    <-- ลำดับขั้นการวิจัย 5 ระยะหลัก
│   └── conversation_history.md               <-- บันทึกประวัติการพูดคุยและข้อตกลงโครงการทั้งหมด
│
├── share_friend/                             <-- [โฟลเดอร์แชร์เพื่อนผ่าน Obsidian - ซิงค์งานวิจัยร่วมกัน]
│
└── archive_initial_research/                 <-- [คลังเก็บงานวิจัยและเอกสารตั้งต้น]
    ├── docs_01_to_07/                        <-- เอกสารวิจัยเดิม Docs 01 ถึง 07 (Meshtastic, LIMA, AlLoRa, ฯลฯ)
    ├── papers_pdf/                           <-- ไฟล์เปเปอร์วิชาการต้นฉบับทั้ง 4 ฉบับ
    ├── exports_for_notebooklm/               <-- ไฟล์สรุปเดิมสำหรับ NotebookLM
    └── research_notes/                       <-- บันทึกการวิเคราะห์เปรียบเทียบเดิม
```

---

## 📑 สรุปย่อสาระสำคัญของเอกสารหลัก (Active Core Specifications)

| รหัสเอกสาร | ชื่อเรื่อง / สเปกทางวิศวกรรม | เทคโนโลยีหลัก | หัวใจสำคัญ | ลิงก์เอกสาร |
| :--- | :--- | :--- | :--- | :--- |
| **DOC-08** | Passive Smartphone RF Sniffing & Rescue | **Multi-RAT Sniffing (Wi-Fi/BLE) + LoRa Mesh** | ดักจับคลื่นมือถือคนหลงป่าไร้แอป + ส่งผลผ่าน LoRa Mesh หาพิกัด Trilateration | [อ่านสเปก DOC-08](file:///d:/ProjectLoRa/docs/08_smartphone_rf_sensing.md) |
| **DOC-09** | Smartphone RF Emissions & Passive Detection | **Emitted RF Taxonomy (Wi-Fi, BLE, Cellular, UWB)** | เจาะลึกกลไก Probe Request, BLE Background Beacons, Doze Mode, และ Dual-RAT ESP32 | [อ่านสเปก DOC-09](file:///d:/ProjectLoRa/docs/09_smartphone_rf_signals_analysis.md) |


---

## 📊 ตารางเปรียบเทียบโปรโตคอลแบบเข้าใจง่าย (Protocol Comparison)

| มิติการเปรียบเทียบ | 🌐 Meshtastic | ⚡ LIMA | 📦 AlLoRa | 🏷️ LoRaWAN Relay (TS011) |
| :--- | :--- | :--- | :--- | :--- |
| **1. โครงสร้างเครือข่าย** | **Peer-to-Peer Mesh**<br>(กระจายตัว ไม่มีศูนย์กลาง) | **Tree-based Mesh Overlay**<br>(ซ้อนทับบน LoRaWAN) | **Request-Reply / Hybrid**<br>(สลับ P2P เป็น Mesh เมื่อหลุด) | **Single-hop Relay**<br>(ทวนสัญญาณขั้นเดียว) |
| **2. อัลกอริทึม Routing** | **Managed Flooding**<br>(บรอดแคสต์ต่อๆ กันไป) | **Reverse Path Forwarding (RPF)**<br>(คำนวณ Cost จาก -RSSI) | **On-demand Flooding Relay**<br>(Relay เฉพาะเมื่อสั่งเปิดบิต) | **Fixed Direct Relay**<br>(จับคู่โหนดคงที่) |
| **3. รองรับ LoRaWAN เดิมไหม?**| ❌ **ไม่รองรับ**<br>(เป็นระบบปิดของตัวเอง) | ✅ **รองรับ 100%**<br>(ไม่ต้องแก้โหนดหรือเซิร์ฟเวอร์) | ❌ **ไม่รองรับ**<br>(สร้างโปรโตคอลเฉพาะขึ้นมาเอง) | ⚠️ **ต้องอัปเกรด**<br>(ต้องใช้ FW ตามสเปก TS011) |
| **4. ประเภทข้อมูลหลัก** | ข้อความสั้น / พิกัด GPS | ข้อมูลเซ็นเซอร์ IoT ทั่วไป | **ข้อมูลขนาดใหญ่ / ไฟล์หลาย KB** | ข้อมูลเซ็นเซอร์ IoT ทั่วไป |
| **5. การรับมือคลื่นชนกัน** | Random Delay Backoff | Stagger Delay + Overhearing | Sleep bit + Random Backoff | Wake-on-Radio (WOR) Timing |
| **6. จุดเด่นที่สุด** | **ติดตั้งง่ายสุด** ไม่ต้องมีเสาเน็ต ใช้แค่มือถือต่อบลูทูธ | **ประหยัดไฟโหนดสูงสุด** ขยายระยะทาง LoRaWAN ได้หลาย Hop | **ส่งข้อมูลก้อนใหญ่ได้ชัวร์** มีระบบแบ่งชิ้นและประกอบไฟล์ | ได้รับการรับรองตามมาตรฐานสมาคม LoRa Alliance |
| **7. ข้อจำกัดสำคัญ** | จราจรติดขัดง่ายถ้ามีหลายสิบโหนด (Broadcast Storm) | ต้องมีโหนด Router หนาแน่นพอให้ลิงก์ที่ SF7 ตลอดสาย | โหนดเซ็นเซอร์ส่งเตือนเองไม่ได้ ต้องรอ Gateway เรียกถาม | ขยายได้แค่ 1 Hop และรองรับลูกข่ายได้ไม่เกิน 16 ตัว |

---

## 🔍 เอกสารวิเคราะห์และกระบวนการทำงาน (Research & Processes)

* 📊 **[ตารางเปรียบเทียบโปรโตคอลและ 4 กลุ่มสถาปัตยกรรมวิชาการ (Comparison Matrix)](file:///d:/ProjectLoRa/research_notes/comparison_matrix.md):** รวบรวมการวิเคราะห์ 4 กลุ่มงานวิจัย (Tree/TDMA, Concurrent TX, P2P/Gateway-Free, Linear Multi-hop) พร้อมตารางเปรียบเทียบระบบจริงและ Decision Tree
* 📊 **[ตารางเปรียบเทียบโปรโตคอลเดิม (Comparison Matrix)](file:///d:/ProjectLoRa/archive_initial_research/research_notes/comparison_matrix.md):** รวบรวมการวิเคราะห์ 4 กลุ่มงานวิจัยเดิม (Tree/TDMA, Concurrent TX, P2P, Linear Multi-hop)
* 🚀 **[ไอเดีย R&D เดิม (Future Innovations)](file:///d:/ProjectLoRa/archive_initial_research/research_notes/future_rd_and_innovation.md):** 5 ช่องว่างเทคโนโลยีเดิมและโจทย์วิจัยต่อยอด
* 🎬 **[คู่มือ Prompt สไลด์เดิม (Slides Guide)](file:///d:/ProjectLoRa/archive_initial_research/research_notes/slides_source_and_prompts.md):** รวบรวมคำสั่ง Prompt และร่างสไลด์เดิมบน NotebookLM
* 📚 **[แหล่งอ้างอิงวิชาการเดิม (References & Citations)](file:///d:/ProjectLoRa/archive_initial_research/research_notes/references_and_citations.md):** รวมรายการบทความวิจัย IEEE/Elsevier/arXiv เดิม
* 🛠️ **[กระบวนการทำงานและแผนงานวิจัย (Project Roadmap)](file:///d:/ProjectLoRa/process_and_workflow/project_roadmap.md):** แผนการดำเนินงานวิศวกรรม 5 ระยะหลัก
* 📝 **[บันทึกประวัติการสนทนาและข้อตกลง (Conversation History)](file:///d:/ProjectLoRa/process_and_workflow/conversation_history.md):** บันทึก Timeline ประวัติการตัดสินใจและการคุยกันทุกขั้นตอนของโครงการ

---

## 💡 คำแนะนำสำหรับการถาม-ตอบกับผู้ช่วย AI ในอนาคต
เมื่อต้องการถามคำถามเจาะจง สามารถอ้างอิงชื่อไฟล์หรือหัวข้อได้ทันที เช่น:
* *"จากไฟล์ `02_lima_lorawan_mesh.md` ช่วยอธิบายการเลือก DER เพิ่มเติมหน่อย"*
* *"ช่วยเขียนโค้ดตามแนวคิด Dual-Priority ใน `future_rd_and_innovation.md`"*
* *"ถ้าจะเริ่มทำตาม Phase 3 ใน `project_roadmap.md` ต้องติดตั้งเครื่องมือ ns-3 อย่างไร"*
#   P r o j e c t C o - o p  
 
# 📝 บันทึกประวัติการวิจัยและการสนทนา (Research Discussion & Progress Log)

> **โฟลเดอร์เป้าหมาย:** [`share_friend/Research/yo_research/`](file:///d:/ProjectCo-op/share_friend/Research/yo_research)  
> **ไฟล์บันทึกหลัก:** [`yo_research_log.md`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/yo_research_log.md)  
> **เอกสารวิเคราะห์ฉบับเต็ม:** [`Research/สรุป/yopaper.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/yopaper.md)  
> **สเปกวิศวกรรมหลักของโครงงาน:** [`docs/08_smartphone_rf_sensing.md`](file:///d:/ProjectCo-op/docs/08_smartphone_rf_sensing.md) | [`docs/09_smartphone_rf_signals_analysis.md`](file:///d:/ProjectCo-op/docs/09_smartphone_rf_signals_analysis.md)  
> **เครื่องมืออ้างอิง:** [Conversation Logger & Progress Tracker Skill](file:///d:/ProjectCo-op/.agents/skills/conversation-logger/SKILL.md)

---

## 📌 วัตถุประสงค์ของเอกสาร (Purpose)
เอกสารนี้จัดทำขึ้นเพื่อเป็น **ศูนย์กลางบันทึกประวัติการพูดคุย (Discussion Log) ข้อตกลงทางวิศวกรรม และข้อค้นพบจากการวิจัย** ที่ทำร่วมกับเพื่อน (Yo) ในโฟลเดอร์ `yo_research` เพื่อป้องกันไม่ให้ข้อมูลและไอเดียการพัฒนาสูญหาย และเป็นจุดซิงค์ข้อมูลอัตโนมัติทุกครั้งที่มีการเรียกใช้คำสั่ง `conversation-logger`

---

## 📑 สารบัญบันทึกประวัติ (Log Index)
* [📅 2026-09-21 04:45 — วิเคราะห์เปเปอร์วิจัย 3 ฉบับใน yo_research & ชี้เป้า 5 ช่องว่างวิจัย](#-2026-09-21-0445--วิเคราะห์เปเปอร์วิจัย-3-ฉบับใน-yo_research--ชี้เป้า-5-ช่องว่างวิจัย)
* [📅 2026-09-21 04:50 — ทฤษฎี vs การปฏิบัติจริง: การใช้ ESP32 สแกนสัญญาณ & การต่อยอดคำแนะนำอาจารย์](#-2026-09-21-0450--ทฤษฎี-vs-การปฏิบัติจริง-การใช้-esp32-สแกนสัญญาณ--การต่อยอดคำแนะนำอาจารย์)
* [📅 2026-09-23 21:25 — จัดทำพิมพ์เขียววิชาการหัวข้อ 01 IEEE 802.11 Scanning สำหรับบทที่ 2 & อัปเดต Kanban](#-2026-09-23-2125--จัดทำพิมพ์เขียววิชาการหัวข้อ-01-ieee-80211-scanning-สำหรับบทที่-2--อัปเดต-kanban)
* [📅 2026-09-23 22:15 — ปรับโครงสร้างโฟลเดอร์ให้ share_friend เป็น Root หลัก](#-2026-09-23-2215--ปรับโครงสร้างโฟลเดอร์ให้-share_friend-เป็น-root-หลัก)

---

## 📅 [2026-09-21 04:45] — วิเคราะห์เปเปอร์วิจัย 3 ฉบับใน yo_research & ชี้เป้า 5 ช่องว่างวิจัย

### 1. สรุปสาระสำคัญของเปเปอร์ทั้ง 3 ฉบับ
1. **Paper 1: Wang et al. (ACM APSys 2013 - National University of Singapore):**
   * *ชื่อเรื่อง:* *Feasibility Study of Mobile Phone WiFi Detection in Aerial Search and Rescue Operations*
   * *ไฟล์:* [`2500727.2500729.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/2500727.2500729.pdf)
   * *สาระสำคัญ:* บุกเบิกการใช้โดรนบินดักคลื่น Wi-Fi ค้นหาคนหลงป่า (Aerial SAR) ตรวจจับได้ไกลกว่า **200 เมตร** ในที่โล่ง และพบว่าโทรศัพท์แนวตั้งรับสัญญาณได้ดีกว่าแนวนอน
   * *จุดอ่อน:* ใช้วิธี **Active RTS/CTS Probing** ซึ่งต้องรู้ MAC Address ของผู้ประสบภัยล่วงหน้า และเสนอให้คนเดินป่าติดตั้งแอป "SOS Beacon" ซึ่งไม่ตอบโจทย์ความจริงที่คนหลงป่าไม่ได้ติดตั้งแอปเฉพาะทาง
2. **Paper 2: Pérez-Hernández et al. (IEEE Access ตุลาคม 2024 - Galgus & Univ. of Seville):**
   * *ชื่อเรื่อง:* *De-Randomization of MAC Addresses Using Fingerprints and RSSI With ML for Wi-Fi Analytics*
   * *ไฟล์:* [`De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf)
   * *สาระสำคัญ:* งานวิจัยใหม่ล่าสุดปี 2024 ที่ทลายกำแพงการสุ่ม MAC Address (MAC Randomization) โดยสกัด **Information Elements (IEs)** 6 ชนิดหลักมาคำนวณ **64-bit FNV-1a Hash** ได้ Fingerprint ประจำเครื่อง และใช้ **Unsupervised ML (k-means++)** บนมิติ RSSI แยกแยะเครื่องที่ได้ Hash ซ้ำกัน แม่นยำสูงถึง **89.7% – 97.9%**
   * *จุดอ่อน:* ออกแบบสำหรับระบบในอาคารที่เน้นสแกน **ย่าน 5 GHz**, ต้องใช้ AP หลายตัวเชื่อมต่อผ่านสาย LAN Switch ซิงค์เวลา NTP และส่งข้อมูลดิบขึ้นระบบ Cloud (Kafka) ซึ่งทำในป่าไม่ได้
3. **Paper 3: Li et al. (IEEE Access 2020 - Univ. of Wollongong & RMIT):**
   * *ชื่อเรื่อง:* *A Case Study of WiFi Sniffing Performance Evaluation*
   * *ไฟล์:* [`A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf)
   * *สาระสำคัญ:* ทดสอบสมรรถนะ Wi-Fi Sniffer เปรียบเทียบ **Raspberry Pi 3** กับ **Pycom LoPy4 (ใช้ชิป ESP32)** พบว่า ESP32 เก็บแพ็กเก็ตได้ประมาณ 60% ของ Raspberry Pi เนื่องจากข้อจำกัดเรื่อง RAM บัฟเฟอร์ แต่ **จำนวนอุปกรณ์ที่ตรวจพบ (Unique Devices) และระดับความแรงสัญญาณ RSSI มีความใกล้เคียงกัน** พร้อมแนะนำว่าการสลับช่องสัญญาณ 1, 6, 11 ควรหน่วงเวลา 0.5–1.5 วินาที
   * *จุดอ่อน:* ผู้วิจัยเลือกตัดแพ็กเก็ตที่เป็นสุ่ม MAC ทิ้งถึง 87% (วิเคราะห์เฉพาะ Real MAC) และสมมติว่าอุปกรณ์เชื่อมต่อกับ Access Point ซึ่งขัดกับสภาพในป่าที่ไม่มีเราเตอร์

### 2. ชี้เป้า 5 ช่องว่างทางงานวิจัย (The 5 Critical Research Gaps)
* **Gap 1 (Wi-Fi Only Blindness):** ทุกเปเปอร์พึ่งพาแต่ Wi-Fi แต่เมื่อสมาร์ตโฟนยุคใหม่จอดับจะเข้าสู่ Doze Mode หยุดส่ง Wi-Fi นาน 5–15 นาที การขาด BLE Sniffing ทำให้พลาดผู้ประสบภัย
* **Gap 2 (Offline Edge De-randomization):** เปเปอร์แก้ MAC Randomization ต้องพึ่งพา Cloud, AP 5GHz, และสาย LAN ซิงค์ NTP ซึ่งในป่าไม่มีโครงข่ายพื้นฐานเหล่านี้
* **Gap 3 (Off-grid Long-range Backhaul):** ไม่มีเปเปอร์ใดมีโซลูชันส่งพิกัดที่ตรวจพบข้ามป่าข้ามเขาหลายกิโลเมตรกลับศูนย์กู้ภัย
* **Gap 4 (Tropical Foliage & NLOS Neglect):** ขาดแบบจำลองการลดทอนของทรงพุ่มไม้และความชื้นในป่า (Canopy & Rain Attenuation)
* **Gap 5 (Energy-Autonomous Edge Deployment):** เปเปอร์เดิมใช้อุปกรณ์กินไฟสูง (x86, RPi, PoE AP) ขาดสถาปัตยกรรมพลังงานต่ำระดับไมโครวัตต์ (ESP32 + LoRa SX1262)

### 3. ตารางเปรียบเทียบเชิงวิเคราะห์
| มิติการวิเคราะห์ | 🚁 Paper 1: Wang et al. (2013) | 🧠 Paper 2: Pérez-Hernández (2024) | 🔬 Paper 3: Li et al. (2020) | 📡 **Project Co-op ของเรา** |
| :--- | :--- | :--- | :--- | :--- |
| **เป้าหมายหลัก** | ค้นหาคนหลงป่าด้วยโดรน (Aerial SAR) | แก้ปัญหา MAC Randomization เพื่อระบุคน | เปรียบเทียบ Channel Hopping & RPi vs ESP32 | **ตรวจจับผู้ประสบภัยไร้แอป + ส่งผลข้ามป่าด้วย LoRa Mesh** |
| **เทคโนโลยีคลื่น** | Wi-Fi 2.4 GHz เท่านั้น | Wi-Fi 5 GHz (และ 2.4 GHz) | Wi-Fi 2.4 GHz เท่านั้น | **Dual-RAT (BLE 2.4 GHz + Wi-Fi 2.4 GHz)** |
| **การจัดการสุ่ม MAC** | ❌ ไม่แก้ (ต้องลงแอปหรือรู้ MAC จริง) | ✅ **ใช้ IEs (FNV-1a Hash) + RSSI ML Clustering** | ❌ ไม่แก้ (ตัดทิ้ง 87% เหลือแต่ Real MAC) | ✅ **IE Fingerprint (Tag 1, 45, 50, 127) + Sequence 12-bit** |
| **สภาวะหน้าจอดับ** | สแกนลดฮวบฮาบ (แก้โดยลงแอปบังคับยิง) | ไม่ได้แก้ปัญหาจอดับ (เน้นเครื่องใน ม. ที่มีคนใช้) | ไม่ได้พิจารณา Doze Mode ในป่า | ✅ **ใช้ BLE Beacons (Apple Find My/Fast Pair) ดักจับทุก 1–3s** |
| **ฮาร์ดแวร์โหนด** | ALIX x86 + Atheros NIC (กินไฟสูง) | Galgus IC450 Enterprise APs | Raspberry Pi 3 & Pycom LoPy4 (ESP32) | **ESP32-S3 + Semtech SX1262 LoRa (กินไฟต่ำหลักร้อย mW)** |
| **การส่งข้อมูลกลับฐาน** | ดึง Log จากโดรนเมื่อบินกลับ | สาย LAN Switch + Cloud Server (Kafka) | จัดเก็บลง SD Card บนตัวบอร์ด | **LoRa Mesh Multi-hop ข้ามเขา 3–10+ กม. (เพย์โหลด 15B)** |
| **สภาพแวดล้อม** | สนามกีฬากว้าง 250 ม. (LOS 100%) | ห้องเรียนและอาคารมหาวิทยาลัย (ในร่ม) | ห้องนั่งเล่น, ห้องทำงาน, มหาวิทยาลัย | **ป่าดงดิบเขตร้อน / ร่องเขาอับสัญญาณ (Tropical NLOS)** |

---

## 📅 [2026-09-21 04:50] — ทฤษฎี vs การปฏิบัติจริง: การใช้ ESP32 สแกนสัญญาณ & การต่อยอดคำแนะนำอาจารย์

### 1. การเปรียบเทียบในทางทฤษฎี vs การปฏิบัติจริงบน ESP32
* **ESP32 ทำได้จริง 100%:** 
  * Wi-Fi Promiscuous Mode ผ่านคำสั่ง `esp_wifi_set_promiscuous(true)` ดักจับ Probe Request (`0x0040`) พร้อม RSSI
  * BLE Passive Scanning ผ่านคำสั่ง `esp_ble_gap_start_scanning()` ดักฟังช่อง 37, 38, 39 รับ Apple Find My และ Fast Pair
* **ข้อจำกัดจริงจาก Paper 3 (Li et al.):**
  * บอร์ด ESP32 มีบัฟเฟอร์ขนาดเล็ก อาจดรอปแพ็กเก็ตเมื่อเทียบกับ Raspberry Pi (~60% packet capture) แต่ **ตรวจพบ Unique Devices และได้ระดับความแรงสัญญาณ RSSI เท่าเทียมกัน 100%**
  * การเปลี่ยนช่องสัญญาณ (Channel Hopping) ควรตั้งหน่วงเวลาที่ **0.5 – 1.0 วินาที** สำหรับช่อง 1, 6, 11
* **สิ่งที่ทำไม่ได้บน ESP32 (ตัดทิ้ง):**
  * สัญญาณ Cellular 4G/5G PRACH (ต้องใช้บอร์ด SDR เช่น HackRF ซึ่งกินไฟสูงและติดข้อกฎหมาย)
  * คลื่น Wi-Fi ย่าน 5 GHz (ฮาร์ดแวร์ ESP32 ไม่รองรับ)

### 2. 3 แนวทางการพัฒนาต่อยอดคำแนะนำอาจารย์ที่ปรึกษา (PoC Testbed 3 โหนด)
1. **Dual-RAT Coexistence บน ESP32:** สลับฟัง BLE 70% (7 วินาที) ดักจับ Apple/Android Beacons ยามจอดับ สลับกับ Wi-Fi 30% (3 วินาที) ดัก Probe Request ระยะไกล
2. **Edge-based IE Fingerprinting:** สกัด Tag 1, 45, 50, 127 บน ESP32 ทำ CRC32/FNV-1a Hash ยุบเป็น Device Signature 4 ไบต์ แก้ปัญหา MAC Randomization โดยไม่ต้องพึ่ง Cloud
3. **แผนการทดลอง 3 ก้าว (3-Step PoC Testbed 40x40m):**
   * *ก้าวที่ 1 (RF Profiling):* วางมือถือ iPhone/Android จอดับ ทดสอบวัดรอบการยิงสัญญาณจริง
   * *ก้าวที่ 2 (Distance vs RSSI Mapping):* เดินทดสอบ 5m, 10m, 20m, 30m, 40m เพื่อคำนวณหาค่า Path Loss Exponent ($n$) ในสภาพแวดล้อมจริง
   * *ก้าวที่ 3 (3-Node Trilateration):* วาง ESP32 3 ตัวเป็นรูปสามเหลี่ยม ส่งข้อมูลผ่าน LoRa คำนวณพิกัดตัดวงกลมบนคอมพิวเตอร์ให้ความคลาดเคลื่อน $< 10$ เมตร

---

## 📅 [2026-09-23 21:25] — จัดทำพิมพ์เขียววิชาการหัวข้อ 01 IEEE 802.11 Scanning สำหรับบทที่ 2 & อัปเดต Kanban

### 1. สรุปประเด็นการสนทนาและการตัดสินใจ
1. **จัดทำตารางแผนการดำเนินงาน 4 ระยะ (Operational Schedule & Sprint Breakdown):**
   * บูรณาการ 16 หัวข้อใน `Wi-Fi Study Guide.md` ควบคู่กับการพัฒนาฮาร์ดแวร์จริง (ESP32 Firmware + LoRa Checkpoint + Field Test)
2. **สัมภาษณ์เชิงลึก (/grill-me) เพื่อกำหนดขอบเขตหัวข้อ 01 — IEEE 802.11 scanning:**
   * ตกลงสร้างไฟล์สังเคราะห์ฉบับสมบูรณ์แยกใหม่สำหรับใช้เป็นเนื้อหาบทที่ 2 ของเล่มโครงงาน
   * บรรจุครบ 4 มิติทางวิศวกรรม: (1) กลไก Active/Passive ตามมาตรฐาน IEEE Std 802.11-2024, (2) แบบจำลองคณิตศาสตร์ Timing & Dwell Time (Min/MaxChannelTime), (3) การเปรียบเทียบ OS API Request vs Radio-level Scan, และ (4) ข้อแนะนำการตั้งค่า Channel Hopping บน ESP32 Sniffer
3. **จัดทำเอกสารและอัปเดตระบบ:**
   * สร้างไฟล์: [`share_friend/Research/สรุป/IEEE 802.11 Scanning/01_IEEE_802.11_Scanning.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/IEEE%20802.11%20Scanning/01_IEEE_802.11_Scanning.md)
   * อัปเดตลิงก์ใน [`share_friend/Wi-Fi Study Guide.md`](file:///d:/ProjectCo-op/share_friend/Wi-Fi%20Study%20Guide.md)
   * อัปเดตบอร์ด Kanban [`share_friend/Todo/TO-DO List Research.md`](file:///d:/ProjectCo-op/share_friend/Todo/TO-DO%20List%20Research.md): ย้ายหัวข้อ 01 เข้า `Success` และย้ายหัวข้อ 02 (Probe Request) เข้าสู่ `Doing`

### 2. สิ่งที่จะทำต่อไป (Next Action Items)
* [ ] เริ่มวิเคราะห์และร่างเนื้อหาหัวข้อ `02 — Probe Request` (สกัดโครงสร้างเฟรม Subtype `0x0040`, Wildcard vs Directed SSID, Sequence Number 12-bit, และ Information Elements Tag 1, 45, 50, 127 สำหรับทำ Fingerprint)

---

## 📅 [2026-09-23 22:15] — ปรับโครงสร้างโฟลเดอร์ให้ share_friend เป็น Root หลัก

### 1. สรุปการปรับปรุงโครงสร้าง
* ย้ายเนื้อหาทั้งหมดจาก `share_friend/share/` ขึ้นมาไว้ที่ `share_friend/` โดยตรงเพื่อความสะดวกในการเปิดด้วย Obsidian
* ลบโฟลเดอร์ซ้อน `share` ทิ้ง
* อัปเดตเส้นทางและลิงก์ทั้งหมดให้สอดคล้องกันอย่างสมบูรณ์

---

## 📅 [2026-09-23 23:59] — เจาะลึกโปรโตคอลการส่งและคลื่นแม่เหล็กไฟฟ้าผิดปกติจากสมาร์ตโฟน (Unconventional Emissions & EM Physics)

### 1. สรุปประเด็นการสนทนาและทิศทางงานวิจัย
1. **พักเรื่อง LoRa ไว้ก่อนชั่วคราว (Focus on Smartphone Emissions):**
   * ผู้ใช้สั่งพักการออกแบบเครือข่าย LoRa และเน้นเจาะลึกเฉพาะ **พฤติกรรมการแผ่คลื่น โปรโตคอลการส่งของแต่ละสัญญาณ และการตรวจจับคลื่นแปลกๆ จากสมาร์ตโฟน (รวมถึงคลื่นแม่เหล็กไฟฟ้าและสิ่งที่มีการค้นพบในงานวิจัย/วงการแฮกเกอร์)**
2. **สร้างเอกสารวิจัยฉบับเต็ม:**
   * บันทึกเอกสารฉบับสมบูรณ์ลงใน [`share_friend/Research/สรุป/02_Smartphone_Signals_and_Unconventional_Emissions.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/02_Smartphone_Signals_and_Unconventional_Emissions.md)
3. **สาระสำคัญของคลื่นและปรากฏการณ์ฟิสิกส์ที่สรุปได้:**
   * **คลื่นวิทยุมาตรฐาน (Standard RF):**
     * Wi-Fi (Active Probe Request, Wildcard vs Directed, Information Elements, Sequence 12-bit)
     * BLE Advertising (Channel 37, 38, 39, Apple Find My, Android Fast Pair)
     * Cellular Uplink (PRACH Bursts กำลังส่งสูงถึง +23 dBm ใน 4G/5G และ +33 dBm / 2W ใน 2G GSM)
     * NFC (13.56 MHz Inductive Polling Loop)
     * UWB (IEEE 802.15.4z Nanosecond Pulses 6.5–8.0 GHz)
   * **คลื่นแปลกและการแผ่รังสีรั่วไหลตามปรากฏการณ์ฟิสิกส์ (Unconventional Emissions):**
     * **Magnetic Anomaly Detection (MAD):** การบิดเบือนสนามแม่เหล็กโลกจากชิ้นส่วนแม่เหล็กถาวร (MagSafe Ring 18–36 ชิ้น, ลำโพง, มอเตอร์สั่น Taptic Engine) ตรวจจับได้ด้วย 3-Axis Magnetometer ในระยะ 0.5–2 เมตร แม้โทรศัพท์จะ **ปิดเครื่องสนิทหรือแบตหมด 100%**
     * **SMPS DC-DC Converter Switching Noise:** คลื่นกวนความถี่ 1–3.2 MHz รั่วไหลจากชิปจ่ายไฟ PMIC ขณะซีพียูประมวลผล
     * **Display PWM & Refresh Harmonics (TEMPEST):** รังสีรั่วไหลจากการหรี่แสงจอ OLED (PWM 240–3840 Hz) และสาย MIPI DSI
     * **Cellular RF Envelope Demodulation:** ปรากฏการณ์เสียงตื๊ดๆ ของลำโพงแอนะล็อก นำมาทำวงจร RF Schottky Diode Detector ตรวจจับพัลส์วิทยุกำลังสูงราคาประหยัด
     * **Semiconductor Non-Linear Junction Detection (NLJD):** การยิงคลื่น $f_0$ เพื่อรับคลื่นฮาร์มอนิก $2f_0$ ที่สะท้อนจากรอยต่อ PN สารกึ่งตัวนำ ตรวจจับเครื่องได้แม้ถอดแบตเตอรี่ออก


---

## 📅 [2026-09-24 00:07] — การอ่านและวิเคราะห์ไฟล์ทั้งหมดใน yo_research แบบละเอียด (ค้นพบ 3 เอกสารใหม่!)

### 1. สรุปรายการไฟล์ทั้งหมดใน yo_research (รวม 7 ไฟล์)
* 📄 `2500727.2500729.pdf`: Wang et al. (ACM ApSys 2013) - บุกเบิกโดรนบินดัก Wi-Fi SAR ในระยะ 200 เมตร
* 📄 `De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf`: Pérez-Hernández et al. (IEEE Access 2024) - สกัด 6 IEs + 64-bit FNV-1a Hash + k-means++ ML
* 📄 `A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf`: Li et al. (IEEE Access 2020) - เปรียบเทียบ RPi vs LoPy4 (ESP32) ยืนยันสกัด RSSI และนับเครื่องแม่นยำเทียบเท่า RPi
* 📄 `How talkative is your mobile device.pdf`: **[เอกสารใหม่]** Julien Freudiger (ACM WiSec 2015 - PARC Xerox) - ศึกษาพฤติกรรมการยิงโพรบ, สถาปัตยกรรมเสาอากาศ 6 แบบ, และเทคนิคทลาย MAC Randomization ด้วย Sequence Number
* 📄 `Signals from the Crowd.pdf`: **[เอกสารใหม่]** Barbera et al. (ACM IMC 2013 - Sapienza Univ. of Rome) - ชุดข้อมูลจริง 11M โพรบจาก 164,740 เครื่อง ค้นพบ PNL Limit 16 SSIDs ในแอนดรอยด์ และการสร้างกราฟสังคม
* 📕 `802.11-2024.pdf`: **[เอกสารใหม่]** IEEE Std 802.11-2024 ฉบับสมบูรณ์ (5,956 หน้า อนุมัติ ธ.ค. 2024) มาตรฐานแม่บทสำหรับการอ้างอิงบทที่ 2
* 📝 `yo_research_log.md`: บันทึกประวัติและข้อตกลงการวิจัย

### 2. ข้อมูลเชิงลึกใหม่ที่สกัดได้ (New Technical Insights & Breakthroughs)
1. **การพิสูจน์การเจาะ MAC Randomization ด้วย Sequence Number (Freudiger 2015):** มีหลักฐานแพ็กเก็ตจริงใน Wireshark ว่าฮาร์ดแวร์มือถือ (iOS 8.1.3) นับเลข Sequence Control 12-bit ต่อเนื่องกันระหว่างโหมดปกติกับโหมดสุ่ม MAC (`SEQ=1039` -> `SEQ=1040`) ช่วยยืนยันแนวทางการสร้างตัวติดตามเครื่องบน ESP32
2. **การจัดวางสายอากาศภาครับ (Antenna Setup):** การใช้ 3 เสาแช่ช่อง 1, 6, 11 (`3.static`) ดักจับแพ็กเก็ตได้มากกว่าเสาเดียวสลับช่อง (`1.dynamic`) ถึง **เกือบ 2 เท่า** (เสาเดียวสลับช่องพลาดแพ็กเก็ตไปถึง 43%)
3. **อัตราการยิงแบบ Burst:** แอนดรอยด์ยิงหนาแน่นถึง 50 แพ็กเก็ตใน 1 วินาที และยิงทุกๆ 66–72 วินาที ขณะที่ iOS ยิงห่างกว่า (เฉลี่ยทุก 330 วินาที)
4. **Android PNL Limit 16 SSIDs (Barbera 2013):** พบการจำกัดฮาร์ดโค้ดใน `wpa_supplicant/driver.h` ของระบบแอนดรอยด์ ทำให้เครื่องส่ง Directed Probe ได้ไม่เกิน 16 รายชื่อ
5. **อัปเดตบทวิเคราะห์ฉบับเต็ม:** บันทึกลงใน `share_friend/Research/สรุป/yopaper.md` เรียบร้อยแล้ว

---

## 📅 [2026-09-24 01:46] — สังเคราะห์เนื้อหาฉบับสมบูรณ์หัวข้อ 02 Probe Request & อัปเดต Kanban

### 1. สรุปการดำเนินงาน
* **จัดทำเอกสารสังเคราะห์ฉบับสมบูรณ์:** [`share_friend/Research/สรุป/IEEE 802.11 Scanning/02_Probe_Request.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/IEEE%20802.11%20Scanning/02_Probe_Request.md)
  * เจาะลึกโครงสร้างไบนารีระดับบิตของเฟรม Management Subtype `0x0040` (Frame Control, Duration, Addr 1-3, Seq-Ctl, Frame Body, FCS)
  * วิเคราะห์เปรียบเทียบ Wildcard SSID (Length 0) vs Directed SSID (พร้อมหลักฐานฮาร์ดโค้ด 16 SSIDs บน Android จาก Barbera et al. 2013)
  * สถาปัตยกรรม Information Elements (IEs) Tag 1, 45, 50, 127 และการทำ FNV-1a Device Fingerprint ทลายการสุ่ม MAC (Pérez-Hernández et al. 2024)
  * การพิสูจน์ความต่อเนื่องของ Sequence Control 12-bit ในฮาร์ดแวร์เพื่อเชื่อมโยง MAC สุ่ม (Freudiger 2015)
  * กลไก Retry Bit และ Burst Loops (ยิงสูงสุด 50 โพรบ/วินาที)
* **อัปเดตระบบ:**
  * เชื่อมโยงลิงก์ใน [`share_friend/Wi-Fi Study Guide.md`](file:///d:/ProjectCo-op/share_friend/Wi-Fi%20Study%20Guide.md)
  * อัปเดต Kanban [`share_friend/Todo/TO-DO List Research.md`](file:///d:/ProjectCo-op/share_friend/Todo/TO-DO%20List%20Research.md): ย้ายหัวข้อ 02 เข้า `Success` และเลื่อนหัวข้อ 03 (Passive vs Active Scan) เข้าสู่ `Doing`

---

## 📅 [2026-09-24 02:09] — สังเคราะห์เนื้อหาฉบับสมบูรณ์หัวข้อ 03 Passive vs Active Scan & อัปเดต Kanban

### 1. สรุปการดำเนินงาน
* **จัดทำเอกสารสังเคราะห์ฉบับสมบูรณ์:** [`share_friend/Research/สรุป/IEEE 802.11 Scanning/03_Passive_vs_Active_Scan.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/IEEE%20802.11%20Scanning/03_Passive_vs_Active_Scan.md)
  * เปรียบเทียบความแตกต่างระหว่าง **Passive Scan** (มือถือฟัง Beacon เงียบๆ ไม่ส่งคลื่น) vs **Active Scan** (มือถือยิง Probe Request ตะโกนถาม AP)
  * ชี้ชัดจุดสับสนสำคัญ: แยกแยะระหว่างคำว่า **Passive ของมือถือ** (Rx only ดักจับไม่ได้) กับ **Passive Sniffing ของนักวิจัย** (โหนดตรวจจับทำตัวเป็น Silent Observer ดักจับตอนมือถือทำ Active Scan)
* **อัปเดตระบบ:**
  * เชื่อมโยงลิงก์ใน [`share_friend/Wi-Fi Study Guide.md`](file:///d:/ProjectCo-op/share_friend/Wi-Fi%20Study%20Guide.md)
  * อัปเดต Kanban [`share_friend/Todo/TO-DO List Research.md`](file:///d:/ProjectCo-op/share_friend/Todo/TO-DO%20List%20Research.md): ย้ายหัวข้อ 03 เข้า `Success` และเลื่อนหัวข้อ 04 (Wi-Fi association process) เข้าสู่ `Doing`

---

## 📅 [30:02 Thu-Sep-2026] — จัดทำ Lab ปฏิบัติการจริงสำหรับหัวข้อ 01–03 (ESP32 Wi-Fi Sniffer)

### 1. สรุปการดำเนินงาน
* สร้างโค้ดเฟิร์มแวร์ Arduino ESP32 สำหรับทดลองจริง: [`share_friend/Experiments/01_03_wifi_sniffer/esp32_wifi_sniffer.ino`](file:///d:/ProjectCo-op/share_friend/Experiments/01_03_wifi_sniffer/esp32_wifi_sniffer.ino)
  * เปิด Promiscuous Mode กรองเฉพาะเฟรม Management Subtype `0x0040` (Probe Request)
  * สลับฟังช่อง 1, 6, 11 (Channel Hopping 500ms)
  * สกัด RSSI, Sequence Number 12-bit, ตรวจสอบบิต MAC Randomization, และอ่านชื่อ SSID
* จัดทำคู่มือปฏิบัติการทดลองจริง 4 สเต็ป: [`share_friend/Experiments/01_03_wifi_sniffer/LAB_GUIDE_01_03.md`](file:///d:/ProjectCo-op/share_friend/Experiments/01_03_wifi_sniffer/LAB_GUIDE_01_03.md)
  * Lab 1: พิสูจน์ Active Scan และ Probe Request หลุดออกมาในอากาศ
  * Lab 2: วัดระยะห่าง (1m, 5m, 10m) เทียบกับระดับความแรงสัญญาณ RSSI (-35 ถึง -85 dBm)
  * Lab 3: พิสูจน์ Doze Mode ยามจอดับ vs การเขย่าเครื่องปลุก Accelerometer
  * Lab 4: ตรวจสอบการสุ่ม MAC และความต่อเนื่องของตัวนับ Sequence Number

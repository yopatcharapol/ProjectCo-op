# 📑 บทวิเคราะห์เปรียบเทียบเชิงลึก: งานวิจัยสากล 5 ฉบับและมาตรฐาน IEEE 802.11-2024 ใน yo_research

> **โฟลเดอร์แหล่งข้อมูล:** [`share_friend/Research/yo_research/`](file:///d:/ProjectCo-op/share_friend/Research/yo_research)  
> **ไฟล์เอกสารเป้าหมาย:** [`yopaper.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/yopaper.md)  
> **สถานะ:** อัปเดตสมบูรณ์ (ครอบคลุม 5 งานวิจัยสากล + มาตรฐาน IEEE Std 802.11-2024)  
> **โครงงานหลัก:** ระบบค้นหาและระบุตำแหน่งผู้ประสบภัยด้วย Passive Smartphone RF Sniffing  

---

## 📌 บทนำและสาระสำคัญของเอกสารใน yo_research (Executive Summary)

จากการตรวจสอบโฟลเดอร์ `yo_research` อย่างละเอียด พบว่ามีเอกสารวิชาการสำคัญระดับนานาชาติรวมทั้งสิ้น **6 ฉบับ (5 งานวิจัย + 1 มาตรฐานแม่บท 5,956 หน้า)** ซึ่งครอบคลุมตั้งแต่ระดับทฤษฎีมาตรฐานสากล สถิติการทดลองจริงในระดับฝูงชนหลายแสนคน พฤติกรรมการยิงคลื่นของระบบปฏิบัติการมือถือ ไปจนถึงการแฮกทลายกำแพง MAC Randomization:

```
📁 share_friend/Research/yo_research/
├── 📄 2500727.2500729.pdf                                (Wang et al., ACM ApSys 2013 - Aerial Drone Wi-Fi SAR)
├── 📄 De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf (Pérez-Hernández et al., IEEE Access 2024 - ML De-randomization)
├── 📄 A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf (Li et al., IEEE Access 2020 - ESP32 vs Raspberry Pi Sniffer)
├── 📄 How talkative is your mobile device.pdf           (Freudiger, ACM WiSec 2015 - [เพิ่มใหม่] พฤติกรรมการยิง & เจาะ MAC Randomization)
├── 📄 Signals from the Crowd.pdf                         (Barbera et al., ACM IMC 2013 - [เพิ่มใหม่] วิเคราะห์ 11M โพรบจาก 160K อุปกรณ์)
├── 📕 802.11-2024.pdf                                    (IEEE Std 802.11-2024 - [เพิ่มใหม่] มาตรฐานทางการฉบับล่าสุด 5,956 หน้า)
└── 📝 yo_research_log.md                                 (บันทึกประวัติการวิเคราะห์และข้อตกลงของทีม)
```

---

## 🔬 การวิเคราะห์เอกสารวิจัย 5 ฉบับแบบเจาะลึก (In-Depth Paper Reviews)

---

### 🚁 ฉบับที่ 1: Wang et al. (ACM ApSys 2013 - National University of Singapore)
* **ชื่อบทความ:** *Feasibility study of mobile phone WiFi detection in aerial search and rescue operations*
* **ไฟล์ต้นฉบับ:** [`2500727.2500729.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/2500727.2500729.pdf)
* **สาระสำคัญทางเทคนิค:**
  * บุกเบิกการนำตัวรับสัญญาณ Wi-Fi ติดตั้งบนอากาศยานไร้คนขับ (UAV/Drone) เพื่อบินค้นหาผู้ประสบภัยในพื้นที่เปิดโล่งและป่าเขา (Aerial Search and Rescue)
  * พิสูจน์ระยะตรวจจับจริง: พบว่าในแนวสายตา (LOS) สามารถตรวจจับสัญญาณ Wi-Fi จากมือถือได้ไกลถึง **200 เมตร**
  * พบคุณสมบัติเชิงโพลาไรเซชัน: มือถือวางแนวตั้ง (Vertical) กระจายคลื่นได้ดีกว่าแนวนอน (Horizontal)
* **ข้อจำกัดทางวิศวกรรม:**
  * ใช้วิธี **Active RTS/CTS Probing** ซึ่งต้องรู้ MAC Address ล่วงหน้า และเสนอให้คนเดินป่าติดตั้งแอป "SOS Beacon" ซึ่งขัดกับความจริงที่ผู้ประสบภัยไม่ได้ติดตั้งแอปเฉพาะทาง

---

### 🧠 ฉบับที่ 2: Pérez-Hernández et al. (IEEE Access ตุลาคม 2024 - Galgus & Univ. of Seville)
* **ชื่อบทความ:** *De-Randomization of MAC Addresses Using Fingerprints and RSSI With ML for Wi-Fi Analytics*
* **ไฟล์ต้นฉบับ:** [`De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/De-Randomization_of_MAC_Addresses_Using_Fingerprin.pdf)
* **สาระสำคัญทางเทคนิค:**
  * งานวิจัยใหม่ล่าสุดที่แก้ปัญหา **MAC Randomization** ได้สำเร็จอย่างสมบูรณ์แบบ
  * สกัดฟิลด์ **Information Elements (IEs)** 6 ชนิดหลัก: Tag 1 (Rates), Tag 45 (HT Caps), Tag 50 (Ext Rates), Tag 127 (Ext Caps), Tag 191 (VHT Caps), Tag 255 (HE Caps)
  * นำมาร้อยเรียงคำนวณ **64-bit FNV-1a Hash** ได้ Fingerprint ประจำตัวเครื่อง และใช้โมเดล **Unsupervised ML (k-means++)** บนมิติ RSSI เวกเตอร์ แยกแยะเครื่องที่ได้ค่า Hash เหมือนกันได้แม่นยำสูงถึง **89.7% – 97.9%**
* **ข้อจำกัดทางวิศวกรรม:**
  * ออกแบบสำหรับระบบในอาคาร ต้องพึ่งพา AP Enterprise หลายตัวต่อสาย LAN และส่งข้อมูลขึ้นเซิร์ฟเวอร์ Cloud (Kafka)

---

### 🔬 ฉบับที่ 3: Li et al. (IEEE Access 2020 - Univ. of Wollongong & RMIT)
* **ชื่อบทความ:** *A Case Study of WiFi Sniffing Performance Evaluation*
* **ไฟล์ต้นฉบับ:** [`A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/A_Case_Study_of_WiFi_Sniffing_Performance_Evaluati.pdf)
* **สาระสำคัญทางเทคนิค:**
  * ทดสอบสมรรถนะของอุปกรณ์ดักจับ เปรียบเทียบ **Raspberry Pi 3** กับ **Pycom LoPy4 (ใช้ชิป ESP32)**
  * **ยืนยันศักยภาพของ ESP32:** แม้ ESP32 จะดรอปแพ็กเก็ตไปบ้าง (จับได้ ~60% ของ RPi) เนื่องจากบัฟเฟอร์ขนาดเล็ก แต่ **จำนวนอุปกรณ์เฉพาะ (Unique Devices Found) และระดับความแรงสัญญาณ RSSI ที่วัดได้ มีความแม่นยำเทียบเท่ากับ Raspberry Pi เกือบ 100%**
  * แนะนำ Channel Hopping: ควรสลับช่อง 1, 6, 11 โดยแช่เวลาที่ **0.5 – 1.5 วินาที** ต่อช่อง

---

### 📱 ฉบับที่ 4 (เพิ่มใหม่): Julien Freudiger (ACM WiSec 2015 - PARC Xerox)
* **ชื่อบทความ:** *How Talkative is your Mobile Device? An Experimental Study of Wi-Fi Probe Requests*
* **ไฟล์ต้นฉบับ:** [`How talkative is your mobile device.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/How%20talkative%20is%20your%20mobile%20device.pdf)
* **สาระสำคัญทางเทคนิคและข้อค้นพบใหม่ (Key Breakthroughs):**
  1. **การเปรียบเทียบสถาปัตยกรรมสายอากาศภาครับ (6 Antenna Configurations):**
     * ทดสอบเปรียบเทียบการใช้เสาอากาศ 1 เสาสลับช่อง (`1.dynamic`), 1 เสาแช่ช่องเดียว (`1.static`), และ 3 เสาฟิกซ์ช่อง 1, 6, 11 (`3.static`)
     * **ผลลัพธ์ชัดเจน:** การใช้ 3 เสาแช่ช่อง 1, 6, 11 (`3.static`) เก็บแพ็กเก็ตได้มากที่สุด (2,100 โพรบ/ชม.) ขณะที่การใช้เสาเดียวสลับช่อง (`1.dynamic`) **พลาดแพ็กเก็ตไปถึง 43%** (เก็บได้เพียง 57% ของแพ็กเก็ตจริง)
  2. **พฤติกรรมการยิงเป็นชุด (Probing Bursts Dynamics):**
     * โทรศัพท์มือถือไม่ได้ส่งโพรบกระจายสม่ำเสมอ แต่ส่งเป็น "ชุดพัลส์หนาแน่น (Bursts)"
     * **Android 4.4 / 5.0:** ส่งโพรบหนาแน่นถึง **50 แพ็กเก็ตภายใน 1 วินาทีเดียว!** และส่งชุด Burst เฉลี่ย **55 ครั้งต่อชั่วโมง (ทุกๆ 66–72 วินาที)**
     * **iOS 8.1.3:** ส่งหน่วงกว่า เฉลี่ยเพียง **11 ครั้งต่อชั่วโมง (ทุกๆ 330 วินาที หรือ 5.5 นาที)**
  3. **การทลายการสุ่ม MAC Address ด้วย Sequence Number (Defeating MAC Randomization):**
     * ผู้วิจัยได้พิสูจน์ด้วยการแคปเจอร์ Wireshark จริง: เมื่อ iOS สลับเข้าโหมดประหยัดพลังงานแล้วเริ่มใช้ MAC สุ่ม **ตัวนับ Sequence Number (SEQ 12-bit) ที่สร้างจากระดับฮาร์ดแวร์ไม่ได้ถูกรีเซ็ต!**
     * ตัวอย่างหลักฐานจริง: อุปกรณ์ `5a:e3:24:ea:35:4a` (สุ่ม MAC) ยิงโพรบด้วย `SEQ=1039` และเสี้ยววินาทีต่อมา อุปกรณ์ `Apple_51:2d:db` (Real MAC) ยิงโพรบตามมาด้วย `SEQ=1040` $\rightarrow$ พิสูจน์ทางคณิตศาสตร์ได้ 100% ว่าเป็นเครื่องเดียวกัน!
  4. **ปัจจัยสถานะเครื่อง (Device Triggers):**
     * **ScreenOn (การปลดล็อกหน้าจอ):** เป็นตัวกระตุ้นให้เครื่องยิง Probe Request พุ่งสูงขึ้นอย่างมหาศาลทั้งใน Android และ iOS
     * **Android 5.0 (Lollipop):** แม้จะเชื่อมต่อกับ Wi-Fi อยู่แล้ว เครื่องก็ยังคงยิง Probe Request ออกมาต่อเนื่องเรื่อยๆ (เพื่อเตรียม Hand-over)

---

### 👥 ฉบับที่ 5 (เพิ่มใหม่): Barbera et al. (ACM IMC 2013 - Sapienza University of Rome)
* **ชื่อบทความ:** *Signals from the Crowd: Uncovering Social Relationships through Smartphone Probes*
* **ไฟล์ต้นฉบับ:** [`Signals from the Crowd.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/Signals%20from%20the%20Crowd.pdf)
* **สาระสำคัญทางเทคนิคและข้อค้นพบใหม่ (Key Breakthroughs):**
  1. **ชุดข้อมูลขนาดมหึมา (Massive Empirical Dataset):**
     * ทำการดักจับสัญญาณต่อเนื่องนาน **3 เดือน** ในงานระดับชาติและนานาชาติ ณ กรุงโรม (ชุมนุมทางการเมือง, สุนทรพจน์พระสันตะปาปาที่วาติกัน, ห้างสรรพสินค้า, สถานีรถไฟหลัก, มหาวิทยาลัย)
     * รวบรวม Probe Requests ได้สูงถึง **11,136,711 แพ็กเก็ต** จากสมาร์ตโฟนไม่ซ้ำกัน **164,740 เครื่อง**
  2. **สัดส่วน Directed Probes vs Broadcast Probes:**
     * **48.0%** ของแพ็กเก็ตที่ลอยอยู่ในอากาศเป็น Directed Probe Requests (ระบุชื่อ SSID เครือข่ายเดิมที่เคยต่อ)
     * มีสมาร์ตโฟนถึง **36.2% ที่แผ่รายชื่อ Preferred Network List (PNL) ออกสู่อากาศ**
  3. **การค้นพบเพดาน PNL Limit ของ Android:**
     * จากการวิเคราะห์กราฟความยาว PNL พบว่ามีความถี่สูงและตัดตกอย่างรวดเร็วที่ **16 SSIDs**
     * ผู้วิจัยได้ไล่โค้ดในเคอร์เนล `wpa_supplicant` ของ Android และพบว่า **ระบบแอนดรอยด์ฮาร์ดโค้ดค่าคงที่จำกัดการส่ง Directed Probe ไว้สูงสุดไม่เกิน 16 รายชื่อ** ในไฟล์ `driver.h`
  4. **การสร้างกราฟเครือข่ายความสัมพันธ์ทางสังคม (Social Graph Extraction):**
     * เสนอโมเดลแปลงความสัมพันธ์สองมิติ (Bipartite Affiliation Graph) ระหว่าง "อุปกรณ์" กับ "SSID ที่แชร์ร่วมกัน"
     * ให้น้ำหนักแบบผกผันกับความถี่คล้าย TF-IDF / Adamic-Adar: หากอุปกรณ์ 2 เครื่องปล่อยชื่อ Wi-Fi ส่วนตัวที่หายากตรงกัน (เช่น `Home_Smith_Family`) แสดงว่าบุคคลทั้งสองมีความสัมพันธ์ทางสังคมใกล้ชิดกัน

---

### 📕 เอกสารมาตรฐานแม่บท: IEEE Std 802.11-2024 (ฉบับสมบูรณ์ 5,956 หน้า)
* **ไฟล์ต้นฉบับ:** [`802.11-2024.pdf`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/802.11-2024.pdf)
* **คุณค่าของเอกสาร:**
  * นี่คือเอกสารมาตรฐานอย่างเป็นทางการฉบับล่าสุด (อนุมัติธันวาคม 2024) รวบรวมข้อกำหนดทางวิศวกรรมของระบบแลนไร้สายทั้งหมด
  * เป็นแหล่งอ้างอิงระดับปฐมภูมิ (Primary Normative Reference) สำหรับเขียน **บทที่ 2 ของเล่มโครงงาน** ในหัวข้อ:
    * **Clause 10.1.4:** Scanning Procedures (Active vs Passive Scanning, MLME Primitives, MinChannelTime, MaxChannelTime)
    * **Clause 9.3.3.9:** Probe Request Frame Format & Element IDs
    * **Clause 9.4.2:** โครงสร้างไบนารีของ Information Elements (IEs)
    * **Clause 12:** Enhanced Privacy Mechanisms และมาตรฐาน Randomized MAC Address

---

## 📊 ตารางเปรียบเทียบเชิงวิเคราะห์ครอบคลุม 5 งานวิจัย

| มิติการเปรียบเทียบ | 🚁 Wang (2013) | 👥 Barbera (2013) | 📱 Freudiger (2015) | 🔬 Li (2020) | 🧠 Pérez-Hernández (2024) | 📡 **Project Co-op ของเรา** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **เป้าหมายหลัก** | ค้นหาคนหลงป่าด้วยโดรน | ถอดรหัสโครงสร้างสังคมจาก SSID | วัดความถี่โพรบ & เจาะสุ่ม MAC | วัดสมรรถนะ RPi vs ESP32 | ทลาย MAC Randomization ด้วย ML | **ค้นหาคนหลงป่าไร้แอป ดักจับ 2 คลื่น** |
| **ขนาดการทดลอง** | มือถือไม่กี่เครื่องบนสนามบิน | 164,740 เครื่อง (11M โพรบ) | มือถือ 4 แบรนด์หลัก (iOS/Android) | รวบรวมในแล็บ & มหาวิทยาลัย | มือถือหลายร้อยเครื่องในแคมปัส | **Field Test ในป่าเปิด & จุดอับสัญญาณ** |
| **เทคโนโลยีคลื่น** | Wi-Fi 2.4 GHz | Wi-Fi 2.4 GHz | Wi-Fi 2.4 / 5 GHz | Wi-Fi 2.4 GHz | Wi-Fi 5 GHz (และ 2.4 GHz) | **Wi-Fi 2.4 GHz + BLE 2.4 GHz (Dual-RAT)** |
| **เทคนิคแก้ MAC สุ่ม** | ❌ ไม่แก้ (ต้องรู้ MAC จริง) | ❌ ไม่แก้ (วิเคราะห์ Real MAC) | ✅ **ใช้ Sequence Counter (SEQ 12-bit)** | ❌ ตัดทิ้ง 87% (ไม่วิเคราะห์) | ✅ **IEs (64-bit Hash) + RSSI Clustering** | ✅ **IEs Fingerprint + Hardware SEQ Tracking** |
| **ข้อสรุปเรื่องฮาร์ดแวร์** | บอร์ด x86 กินไฟสูง | โน้ตบุ๊ก 5 เครื่อง + การ์ด Atheros | 3 เสาแช่ช่อง 1,6,11 ดีกว่าเสาเดียว 2 เท่า | ESP32 สกัด RSSI & Unique แม่นเท่า RPi | ต้องใช้ AP Enterprise หลายตัว | **ESP32 โหนดประหยัดพลังงานภาคสนาม** |
| **สภาวะหน้าจอดับ** | สัญญาณหาย ต้องลงแอปปลุก | ไม่ได้ศึกษาเชิงลึก | ศึกษาละเอียด (โพรบหน่วงลงแบบ Exponent) | ไม่ได้ศึกษา Doze Mode | ไม่ได้ศึกษา Doze Mode | **ใช้ BLE Beacons (Apple Find My) ดักตอนจอดับ** |

---

## 💡 สิ่งที่โครงงานของเราได้ประโยชน์เพิ่มขึ้นอย่างมหาศาล (Key Actionable Takeaways)

1. **ยืนยันการใช้ Sequence Number ควบคู่กับ IEs Fingerprint:** งานวิจัยของ Freudiger (2015) เป็นหลักฐานอ้างอิงชั้นยอดว่า ตัวนับลำดับแพ็กเก็ต 12 บิตในฮาร์ดแวร์ไม่ถูกรีเซ็ตเมื่อสุ่ม MAC ทำให้เราสามารถเขียนอัลกอริทึมติดตามเครื่องบน ESP32 ได้ง่ายขึ้นมาก
2. **ข้อพิสูจน์เรื่องการจัดวางสายอากาศ (Antenna Advantage):** ผลการทดลองของ Freudiger พิสูจน์ว่าเสาอากาศสลับช่อง (Single Hopping Antenna) พลาดแพ็กเก็ตไปถึง 43% ทำให้เราสามารถเสนอเป็นข้อแนะนำทางวิศวกรรมในเล่มรายงานได้ว่า การตั้งโหนดตรวจจับที่ดีควรมีโหนดประจำช่อง 1, 6, 11
3. **การใช้ประโยชน์จาก PNL Limit 16 SSIDs:** งานวิจัยของ Barbera (2013) ช่วยยืนยันพฤติกรรมเบื้องลึกของ Android ทำให้เข้าใจโครงสร้างของเฟรม Probe Request อย่างถ่องแท้
4. **ความสมบูรณ์ของบทที่ 2 ด้วย 802.11-2024:** มีมาตรฐานตัวเต็ม 5,956 หน้าเป็นเกราะกำบังทางวิชาการ ทำให้การเขียนคำนิยาม ไทมิ่ง และโครงสร้างไบนารีถูกต้องตามมาตรฐานฉบับล่าสุด 100%

# Bluetooth จาก IEEE 802.15.1 ถึง Core 7.0

บันทึกอ่านประกอบโครงการ Project Co-op · ตรวจสอบแหล่งข้อมูลเมื่อ 27 พฤศจิกายน 2026

**IEEE 802.15.1 ฉบับสุดท้ายอิง Bluetooth 1.2 ส่วน Bluetooth 4.0 เป็นรุ่นที่เพิ่ม Bluetooth Low Energy (BLE) ควบคู่กับ Classic** ทั้งสองแบบยังอยู่ใน Bluetooth Core 7.0 และมีโครงสร้างแพ็กเก็ตบนอากาศต่างกัน ส่วน BLE เองก็มีหลายรูปแบบตามงาน เช่น ประกาศตัว ส่งข้อมูลหลังเชื่อมต่อ และส่งเสียง [IEEE Task Group 1](https://www.ieee802.org/15/pub/TG1.html) · [บันทึก IEEE เรื่องฉบับปี 2005](https://www.ieee802.org/minutes/2010-July/20100716-closing-minutes-v1.pdf) · [Bluetooth LE Primer](https://www.bluetooth.com/bluetooth-le-primer/) · [Core 7.0 Vol. 1 Part A](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/architecture--arch-.html)

## 1. ใครเริ่ม Bluetooth และทำไมต้องมี Bluetooth SIG

ปี 1994 ทีม Ericsson Mobile Communications ที่เมือง Lund ประเทศสวีเดน เริ่มศึกษาวิธีใช้วิทยุระยะสั้นแทนสายที่เชื่อมโทรศัพท์มือถือกับอุปกรณ์เสริม บริษัทเชิญ IBM, Intel, Nokia และ Toshiba เข้าร่วมพัฒนาในปี 1997 แล้วทั้งห้าบริษัทร่วมก่อตั้ง **Bluetooth Special Interest Group (Bluetooth SIG)** เมื่อวันที่ 20 พฤษภาคม 1998 [เอกสาร Ericsson ปี 1999](https://www.ericsson.com/4ac5e9/assets/local/about-ericsson/ericsson-history/contact-kontakten/documents/1990s/ericsson_contact_1999_08.pdf) · [ประวัติจาก Bluetooth SIG](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/20year/default.html)

| ผู้ก่อตั้ง | รู้จักบริษัทผ่านอะไรได้บ้าง |
|---|---|
| **Ericsson (อีริคสัน)** | บริษัทโทรคมนาคมสวีเดน ทำระบบเครือข่ายโทรศัพท์และเคยผลิตมือถือเอง เอกสารบริษัทระบุว่า Jaap Haartsen และ Sven Mattisson เป็นวิศวกรสองคนในทีมยุคเริ่มต้นของ Bluetooth [ประวัติบริษัท](https://www.ericsson.com/en/about-us/history) · [ต้นกำเนิด Bluetooth ที่ Ericsson](https://www.ericsson.com/en/blog/north-america/2022/ericsson-bluetooth) |
| **IBM** | บริษัทคอมพิวเตอร์อเมริกัน ทำระบบคอมพิวเตอร์สำหรับองค์กรและเคยทำคอมพิวเตอร์ส่วนบุคคล ตัวอย่างคือ IBM PC และ ThinkPad ยุคแรก [IBM PC](https://www.ibm.com/history/personal-computer) · [ThinkPad](https://www.ibm.com/history/thinkpad) |
| **Intel** | บริษัทเซมิคอนดักเตอร์อเมริกัน ตัวอย่างงานที่คนคุ้นคือชิปประมวลผลตระกูล Pentium สำหรับพีซี [รายงานประจำปี Intel 1996](https://www.intel.com/content/dam/www/central-libraries/us/en/documents/2025-05/history-1996-annual-report.pdf) |
| **Nokia** | บริษัทฟินแลนด์ที่ทำมือถือและอุปกรณ์เครือข่าย ตัวอย่างในยุคนั้นคือมือถือ GSM [ประวัติจาก Nokia](https://www.nokia.com/blog/thirty-years-on-from-the-call-that-transformed-how-we-communicate/) |
| **Toshiba (โตชิบา)** | บริษัทญี่ปุ่นด้านอิเล็กทรอนิกส์และเทคโนโลยี ตัวอย่างฝั่งคอมพิวเตอร์คือแล็ปท็อป T1100 ที่ออกในปี 1985 [ประวัติผลิตภัณฑ์จาก Toshiba](https://www.global.toshiba/ww/news/corporate/2013/10/pr2901.html) |

คำว่า **สมาคมอุตสาหกรรมไม่แสวงกำไร** ในที่นี้หมายถึงองค์กรที่บริษัทสมาชิกใช้ร่วมกันเพื่อกำหนดวิธีให้อุปกรณ์ข้ามยี่ห้อทำงานร่วมกัน SIG ดูแลการพัฒนาข้อกำหนด โปรแกรม **Bluetooth Qualification** สำหรับผลิตภัณฑ์ และการใช้เครื่องหมายการค้า Bluetooth บริษัทสมาชิกยังผลิตและขายสินค้าของตัวเองตามปกติ ลองนึกถึงมือถือกับหูฟังคนละยี่ห้อ ทั้งคู่ต้องทำตามวิธีสื่อสารและโปรไฟล์ที่สอดคล้องกันจึงใช้งานร่วมกันได้ [บทบาทของ Bluetooth SIG](https://www.bluetooth.com/wp-content/uploads/2020/03/2020_Market_Update-EN.pdf) · [ตัวอย่าง Classic Audio กับ LE Audio](https://www.bluetooth.com/media/le-audio/le-audio-faqs/)

## 2. IEEE 802.15.1 หยิบส่วนไหนของ Bluetooth ไปใช้

IEEE ได้สิทธิ์จาก Bluetooth SIG ให้นำข้อกำหนดบางส่วนมาดัดแปลงเป็นมาตรฐานเครือข่ายไร้สายระยะใกล้ หรือ WPAN ฉบับ **IEEE 802.15.1-2002** อิง Bluetooth **1.1** ส่วนฉบับปรับปรุง **IEEE 802.15.1-2005** อิง Bluetooth **1.2** และ IEEE ถอนฉบับปี 2005 ออกจากสถานะใช้งานเมื่อวันที่ 7 พฤษภาคม 2018 [ประกาศ IEEE ปี 2002](https://www.ieee802.org/15/pub/802.15%20Archives/802.15%20Archive%20pre-802WWW/2002/May02/02168r1P802-15_TG1-Press-Release.pdf) · [ขอบเขตโครงการปรับปรุง IEEE](https://ieee802.org/secmail/pdf00141.pdf) · [หน้ามาตรฐาน IEEE 802.15.1-2005](https://standards.ieee.org/ieee/802.15.1/3513/)

| ส่วนที่ IEEE ระบุในขอบเขตฉบับปี 2002 | ทำหน้าที่อะไร |
|---|---|
| **Radio / PHY** | กำหนดวิธีส่งสัญญาณจริง เช่น ความถี่และการมอดูเลต (วิธีแทนข้อมูลด้วยสัญญาณวิทยุ) |
| **Baseband** | จัดรูปแพ็กเก็ตและจังหวะส่งรับในลิงก์ Classic |
| **LMP** | โปรโตคอลจัดการลิงก์ Classic เช่น การเจรจาความสามารถระหว่างอุปกรณ์ |
| **L2CAP** | จัดช่องข้อมูลให้โปรโตคอลด้านบนและช่วยแบ่งหรือประกอบข้อมูลตามชนิดช่อง |

IEEE เรียกขอบเขตนี้รวมว่า **MAC/PHY** และเพิ่มจุดเชื่อมกับชั้น LLC ของตระกูล IEEE 802 ด้วย ส่วนบริการสตรีมเสียงหรือการทำงานระดับแอปต้องอ่านโปรไฟล์ที่เกี่ยวข้องของ SIG เพิ่ม ตัวอย่างการเปลี่ยนแปลงใน Bluetooth 1.2 คือ **Adaptive Frequency Hopping** ซึ่งปรับชุดช่องความถี่ที่ใช้เพื่อลดผลจากช่องที่มีสัญญาณรบกวน และปรับขั้นตอนเชื่อมต่อให้เร็วขึ้น [ขอบเขตจาก IEEE Task Group 1](https://www.ieee802.org/15/pub/TG1.html) · [Core Version History สำหรับ v1.2](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core-60/out/en/consolidated-table-of-contents%2C-acknowledgments%2C---core-configurations/version-history-and-acknowledgments.html)

## 3. เวอร์ชันต่าง ๆ เปลี่ยนอะไรข้างใน

**เลข Core คือรุ่นของชุดข้อกำหนด** ส่วน **Classic / BLE คือชนิดวิทยุ** และ **Dual-mode คืออุปกรณ์ที่รองรับทั้งคู่** ตัวอย่างเช่น อุปกรณ์ Core 7.0 แบบ Dual-mode ยังใช้ Classic กับลำโพงที่รองรับโปรไฟล์เสียงแบบเดียวกันได้ ฟีเจอร์เสริม เช่น LE Audio, Channel Sounding และ HDT ต้องตรวจการรองรับตามงานที่ใช้จริง [Core Configurations](https://qualification.support.bluetooth.com/hc/en-us/articles/26704878828813-Understanding-Core-Configurations) · [วิธีระบุฟีเจอร์ที่รองรับ](https://www.bluetooth.com/communicating-supported-bluetooth-functionality/)

ตารางนี้เลือกฟีเจอร์ที่ช่วยเห็นว่ารุ่นนั้นเปลี่ยนที่วิทยุ รูปแพ็กเก็ต ขั้นตอนสื่อสาร หรือชั้นซอฟต์แวร์ ดูรายการครบตามเลขหัวข้อในเอกสารต้นฉบับ [Core 7.0 Vol. 1 Part C §§2–18](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/core-specification-change-history.html) · [วันที่และประวัติแต่ละรุ่น](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/consolidated-table-of-contents%2C-acknowledgments%2C---core-configurations/version-history-and-acknowledgments.html)

| Core | ฟีเจอร์ตัวอย่าง | เปลี่ยนข้างในตรงไหน |
|---|---|---|
| **1.0 / 1.1 / 1.2** | Basic Rate ยุคแรก และการปรับการกระโดดความถี่ใน 1.2 | วิทยุและการจัดการลิงก์ของ Bluetooth รุ่นแรก |
| **2.0 + EDR** | Enhanced Data Rate เพิ่มอัตราบิตระดับวิทยุเป็น 2 หรือ 3 Mb/s ตามรูปแบบที่ใช้ | แพ็กเก็ต EDR ยังใช้ access code และ header แบบ Basic Rate จากนั้นเพิ่มช่วงคั่นและซิงก์ก่อน payload ที่ใช้การมอดูเลตอีกแบบ |
| **2.1 + EDR** | Secure Simple Pairing และ Extended Inquiry Response | ปรับวิธีจับคู่ และเพิ่มข้อมูลที่อุปกรณ์ตอบระหว่างการค้นหา |
| **3.0 + HS** | High Speed ผ่าน Alternate MAC/PHY | ใช้ 802.11 เป็นช่องทางเสริมสำหรับส่งข้อมูลในอุปกรณ์ที่รองรับ ฟีเจอร์นี้ถูกถอดจาก Core ใน 5.3 |
| **4.0** | เพิ่ม BLE รวมถึง ATT, GATT และ Security Manager ที่ใช้กับ BLE | เพิ่ม PHY และ Link Layer ฝั่ง LE พร้อม advertising และการเชื่อมต่อแบบ LE |
| **4.1** | เพิ่มการทำหลายบทบาทฝั่ง LE พร้อมกัน เช่น เป็น Central กับลิงก์หนึ่งและ Peripheral กับอีกลิงก์ | เปลี่ยนความสามารถด้านบทบาทและการจัดการลิงก์ |
| **4.2** | Data Length Extension, LE Secure Connections, Link Layer Privacy | ขยายความยาวข้อมูลของแพ็กเก็ตหลังเชื่อมต่อ เพิ่มวิธีจับคู่และจัดการที่อยู่เพื่อความเป็นส่วนตัว |
| **5.0** | LE 2M, LE Coded และ Extended Advertising | เพิ่มวิธีส่งทางวิทยุและชนิด PDU สำหรับการประกาศแบบขยาย |
| **5.1** | Direction Finding แบบ AoA/AoD (หามุมรับหรือมุมส่ง) | ใช้ส่วนสัญญาณเสริม CTE ในแพ็กเก็ตที่รองรับ ร่วมกับการสลับเสาอากาศและวัดสัญญาณ |
| **5.2** | LE Isochronous Channels, Enhanced ATT, LE Power Control | เพิ่มช่องขนส่งที่คุมจังหวะเวลาเพื่อรองรับ LE Audio และปรับโปรโตคอลข้อมูลกับการควบคุมกำลังส่ง |
| **5.3** | Connection Subrating และ LE Channel Classification | ปรับความถี่ของเหตุการณ์เชื่อมต่อที่ใช้งานและเพิ่มข้อมูลช่วยประเมินช่องสัญญาณ |
| **5.4** | Periodic Advertising with Responses (PAwR) และ Encrypted Advertising Data | ให้ผู้รับตอบกลับในช่องเวลาที่จัดไว้ และเพิ่มวิธีเข้ารหัสเนื้อหาโฆษณา |
| **6.0** | Channel Sounding และ Decision-Based Advertising Filtering | เพิ่มขั้นตอนแลกสัญญาณเพื่อวัดระยะ และให้ตัวสแกนเลือกว่าจะติดตามโฆษณาต่อหรือไม่ตามข้อมูลกำกับ |
| **6.1** | Randomized RPA Updates | สุ่มจังหวะเปลี่ยนที่อยู่ส่วนตัวแบบที่คู่ซึ่งมีคีย์สามารถตรวจสอบได้ |
| **6.2** | Shorter Connection Intervals และการปรับปรุง Channel Sounding | เพิ่มตัวเลือกช่วงเชื่อมต่อที่สั้นลงและเพิ่มความทนทานของการวัดระยะ |
| **6.3** | Channel Sounding รองรับข้อกำหนดความแม่นยำของเวลาไปกลับแยกตาม PHY และเพิ่มการส่งข้อมูลประกอบการแก้เฟส | ปรับวิธีรายงานความสามารถและข้อมูลที่ใช้ประกอบการวัดระยะ |
| **7.0** | High Data Throughput (HDT), การอัปเดตพารามิเตอร์สตรีมแบบ isochronous และกลไกคีย์ใหม่ | HDT เป็นทางเลือกเพิ่ม PHY และรูปแพ็กเก็ตใหม่ อัตราบิตระดับวิทยุสูงสุด 7.5 Mb/s [ภาพรวม Core 7.0](https://www.bluetooth.com/bluetooth-core-7-0-feature-overview/) |

## 4. เปิดดูแพ็กเก็ตข้างใน

### Classic กับ BLE วางฟิลด์กันอย่างไร

| แพ็กเก็ตบนอากาศ | ภาพย่อเรียงตามลำดับส่ง |
|---|---|
| **Classic Basic Rate** | `Access Code → Header → Payload` เป็นรูปทั่วไป บางชนิดมีเฉพาะ access code หรือมีแค่ access code กับ header |
| **Classic EDR** | `Access Code → Header → Guard → Sync → Payload → Trailer` ส่วนต้นยังใช้รูปแบบและการมอดูเลตของ Basic Rate |
| **BLE LE 1M / LE 2M** | `Preamble → Access Address → PDU → CRC → [CTE เมื่อใช้]` Preamble ยาว 1 ไบต์บน LE 1M และ 2 ไบต์บน LE 2M |
| **BLE LE Coded** | `Preamble → Access Address → Coding Indicator → TERM1 → PDU → CRC → TERM2` มีฟิลด์กำกับและปิดช่วงการเข้ารหัสแก้ความผิดพลาด (Forward Error Correction) เพื่อเพิ่มโอกาสรับได้ที่สัญญาณอ่อน |
| **BLE HDT ใน Core 7.0** | `Preamble → Control Header → [PDU Header Zone] → [Payload Zone]` ส่วนในวงเล็บขึ้นกับชนิดแพ็กเก็ต เช่น แบบสั้นสำหรับตอบรับ หรือแบบที่รวมหลาย payload |

ในฝั่ง BLE แบบ LE 1M/2M นั้น **Preamble** ช่วยให้ตัวรับจับจังหวะ, **Access Address** ช่วยแยกว่าแพ็กเก็ตอยู่ในบริบทใด เช่น advertising หรือลิงก์ที่เชื่อมอยู่, **PDU** คือหน่วยข้อมูลของโปรโตคอลที่มี header และส่วนข้อมูลตามชนิดของมัน และ **CRC** ใช้ตรวจความผิดพลาดของบิต ส่วน **CTE** คือช่วงสัญญาณคงที่ที่ใช้กับ Direction Finding เมื่อเฟรมรองรับ [Core 7.0 Vol. 2 Part B §6](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/br-edr-controller/baseband--bb--specification.html) · [Core 7.0 Vol. 6 Part B §2](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/low-energy-controller/linklayer-ll--specification.html) · [Core 7.0 Feature Overview §3.2.3](https://www.bluetooth.com/bluetooth-core-7-0-feature-overview/)

### ตัวอย่างที่ 1 เปิดซอง BLE Legacy Advertising

สมมติอุปกรณ์ประกาศด้วยแพ็กเก็ตชนิด **ADV_IND** บน LE 1M ในกรณีนี้ PDU มี header 2 ไบต์ ส่วน payload ของ PDU ประกอบด้วย **AdvA** (ที่อยู่ผู้ประกาศ 6 ไบต์) และ **AdvData** (ข้อมูลที่ประกาศ 0–31 ไบต์) โดยทั่วไปสแกนเนอร์ฟัง legacy advertising บนช่องหลัก 37, 38 และ 39 [Core 7.0 Link Layer §§2.1–2.3](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/low-energy-controller/linklayer-ll--specification.html) · [Bluetooth LE Primer §7.8.2](https://www.bluetooth.com/bluetooth-le-primer/)

```mermaid
flowchart LR
    P["Preamble · 1 ไบต์"] --> A["Access Address · 4 ไบต์"]
    A --> H["PDU Header · 2 ไบต์"]
    H --> D["AdvA · 6 ไบต์"]
    D --> V["AdvData · 0–31 ไบต์"]
    V --> C["CRC · 3 ไบต์"]
```

ใน **AdvData** ข้อมูลแต่ละรายการวางเป็น `Length → AD Type → AD Data` โดย Length นับไบต์ของ Type และ Data ตัวอย่างข้างล่างยกสองรายการจากข้อกำหนดของ SIG [GAP ว่าด้วย AD structures](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core-60/out/en/host/generic-access-profile.html) · [Core Supplement, Data Types Specification ตาราง 2.2](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/CSS_v14/out/en/core-supplementary-features/data-types-specification.html)

| ไบต์เลขฐานสิบหก | วิธีอ่าน |
|---|---|
| `02 01 01` | `02` คือขนาด 2 ไบต์ถัดไป, `01` แรกคือ AD Type **Flags**, `01` ตัวท้ายตั้งค่า LE Limited Discoverable ตามตัวอย่างของ SIG |
| `0A 09 50 65 64 6F 6D 65 74 65 72` | `0A` คือขนาด 10 ไบต์ถัดไป, `09` คือ AD Type **Complete Local Name**, อีก 9 ไบต์อ่านเป็น `Pedometer` ตามเลขฐานสิบหกในตารางต้นฉบับ |

### ตัวอย่างที่ 2 ทำไมเห็นทั้ง 27, 31 และ 251 ไบต์

| ตัวเลข | นับส่วนไหน |
|---|---|
| **27 ไบต์** | ข้อมูลในแพ็กเก็ต LE ฝั่งส่งข้อมูลหลังเชื่อมต่อแบบเดิม ก่อน Data Length Extension หรือเมื่อใช้ความยาวเดิม |
| **31 ไบต์** | ขนาดสูงสุดของ **AdvData** ใน legacy `ADV_IND` แยกจากที่อยู่ผู้ประกาศ 6 ไบต์ |
| **251 ไบต์** | ขนาดข้อมูลสูงสุดในแพ็กเก็ต LE แบบเชื่อมต่อทั่วไปเมื่อใช้ **Data Length Extension** ที่เพิ่มใน Core 4.2 ตัวเลขนี้ยังมี overhead ของโปรโตคอลชั้นบน เช่น L2CAP และ ATT ตามข้อมูลที่ส่ง |

ดังนั้น 4.2 ทำให้ **แพ็กเก็ตข้อมูลหลังเชื่อมต่อ** ยาวขึ้นได้ ส่วน 5.0 เพิ่ม **Extended Advertising** ซึ่งให้แพ็กเก็ตบนช่องหลักชี้ไปยังแพ็กเก็ตบนช่องรองเพื่อขนข้อมูลโฆษณาเพิ่ม อุปกรณ์รุ่นใหม่ยังใช้ Legacy Advertising ได้ ตัวอย่างนี้แสดงว่าการเพิ่มฟีเจอร์อาจเปลี่ยนเพียงความยาวของข้อมูล เพิ่มชนิดแพ็กเก็ตอีกชุด หรือเพิ่มรูปแบบวิทยุแบบที่ HDT ทำ [คำอธิบาย Data Length จาก SIG](https://www.bluetooth.com/blog/exploring-bluetooth-5-how-fast-can-it-be/) · [Extended Advertising จาก SIG](https://www.bluetooth.com/blog/exploring-bluetooth5-whats-new-in-advertising/) · [HDT จาก SIG](https://www.bluetooth.com/bluetooth-core-7-0-feature-overview/)

### ตัวอย่างที่ 3 ค่าจากเซนเซอร์เดินทางผ่านชั้นไหนบ้าง

สมมติว่าเซนเซอร์เชื่อมต่อแล้วและส่งค่าอุณหภูมิ 24.56 °C แอปกำหนดให้เก็บค่าเป็นจำนวนเต็ม 2456 แล้วส่งไบต์ `98 09` แบบ little-endian (ไบต์ต่ำมาก่อน) นี่เป็นตัวอย่างสมมติเพื่ออธิบายการซ้อนชั้น โดยใช้ Notification ผ่านช่อง ATT แบบคงที่ตามข้อกำหนด [Core 7.0 ATT §3.4.7.1](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/host/attributeprotocol-att--specification.html) · [Core 6.0 L2CAP §§2.1, 3.1](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core-60/out/en/host/logical-link-control-and-adaptation-protocol-specification.html) · [Core 7.0 Link Layer §2.4](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/low-energy-controller/linklayer-ll--specification.html)

| ชั้น | สิ่งที่เพิ่มเข้าไป |
|---|---|
| **GATT / แอป** | GATT จัดโครงสร้างบริการและ characteristic (ค่าข้อมูลภายในบริการ) ตัวอย่างนี้สมมติให้ handle ของค่าเป็น `0x0025` และให้แอปรู้วิธีตีความ `98 09` |
| **ATT** | Notification เป็น `1B 25 00 98 09` รวม 5 ไบต์ โดย `1B` คือรหัส Notification และ `25 00` คือ handle |
| **L2CAP** | เติม `05 00 04 00` ด้านหน้า โดย `05 00` ระบุความยาวข้อมูล ATT 5 ไบต์ และ `04 00` ระบุช่อง ATT แบบคงที่ `0x0004` |
| **Link Layer / PHY** | เมื่อนำชุด 9 ไบต์นี้ลง LE Data PDU เดียว จะอยู่ในส่วน payload ของลิงก์ แล้วส่งตามรูปแพ็กเก็ตของ PHY ที่เลือก |

ตัวเลข **ATT_MTU** จำกัดขนาดข้อความที่ชั้น ATT ส่วน **Data Length Extension** เกี่ยวกับขนาดข้อมูลต่อแพ็กเก็ตที่ Link Layer ถ้าข้อมูลชั้นบนยาวกว่าที่แพ็กเก็ตวิทยุหนึ่งชุดรองรับได้ ลิงก์สามารถแบ่งส่งหลายชุดแล้วประกอบคืน [Core 7.0 ATT §§3.2.8, 3.4.7.1](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/host/attributeprotocol-att--specification.html) · [Core 7.0 Architecture §3.5.5.2.2](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/architecture--arch-.html)

## 5. เอาไปอ่านล็อก ESP32 อย่างไร

- **REPORT** คือรายงานที่ BLE stack ส่งให้โปรแกรม logger มีเวลา ที่อยู่และชนิดที่อยู่ RSSI ชนิดเหตุการณ์ และ payload ดิบตามที่โปรแกรมได้รับ ส่วน **AD_DETAILS** แยกเนื้อหาใน payload เช่น ชื่อ Flags, Service UUID, Manufacturer ID และ TX Power ที่ประกาศ เฟรมบนอากาศในแผนภาพมีฟิลด์มากกว่ารายงานที่ logger เก็บ [คู่มือ BLE Behavior Logger](../experiments/firmware/ble_behavior_logger/README.md)
- **RSSI** เป็นค่าความแรงสัญญาณที่ฝั่งรับวัด ส่วน **TX Power** ใน AD คือค่าที่ฝั่งส่งประกาศเมื่อมีฟิลด์นั้น และ **Channel Sounding** เป็นขั้นตอนวัดระยะที่อุปกรณ์คู่ทดลองต้องรองรับ ใช้ชื่อแต่ละค่าแยกกันในรายงาน [ขอบเขตโครงการ](../docs/CURRENT_RESEARCH_SCOPE.md) · [Channel Sounding จาก SIG](https://www.bluetooth.com/channel-sounding-tech-overview/)
- **Passive Scan** ฟังข้อมูลที่ออกอากาศ ส่วน **Active Scan** ส่งคำขอข้อมูลเพิ่มเติมได้เมื่อโฆษณารองรับการตอบ จึงอาจเห็นชื่อหรือข้อมูลเพิ่มจาก Scan Response ให้เทียบผลแยกตามโหมด และใช้ UUID เป็นหลักฐานประกอบตามบริบท เพราะโทรศัพท์หลายเครื่องอาจประกาศบริการเดียวกัน [Bluetooth LE Primer](https://www.bluetooth.com/bluetooth-le-primer/) · [ขอบเขตโครงการ](../docs/CURRENT_RESEARCH_SCOPE.md)

สำหรับ PDF ภาษาไทยใน `nay/` เริ่มจาก **Vol. 1 Part A (Architecture)** แล้วไป **Vol. 6 Part B (Link Layer)** สำหรับโครงแพ็กเก็ต และ **Vol. 3 Part C (GAP)** สำหรับการค้นหาและรูปแบบข้อมูลประกาศ ถ้าตามเรื่องวัดระยะจึงเปิด **Vol. 6 Part H (Channel Sounding)** เพิ่ม ตรวจหน้าแรกของ Architecture กับ Link Layer ฉบับ §1–4.3 แล้ว ทั้งสองระบุว่าต้นฉบับคือ Bluetooth SIG Core 7.0 ลงวันที่ 2 พฤศจิกายน 2026 เวลาต้องอ้างข้อกำหนดหรือเลขฟิลด์ในรายงานให้ผูกกับ Volume, Part และ Section ของต้นฉบับ SIG [Architecture ภาษาไทย](../nay/Core_v7.0_Vol1_PartA_Architecture-TH.pdf) · [Link Layer ภาษาไทย ฉบับ §1–4.3](../nay/Core_v7.0_Vol6_PartB_LinkLayer-TH_partial_s1-4.3.pdf) · [Bluetooth Core 7.0 ฉบับทางการ](https://www.bluetooth.com/specifications/specs/core-specification-7-0/)

## 6. เทคโนโลยีแต่ละยุคทำให้อุปกรณ์ทำอะไรได้เพิ่ม

ตารางนี้อ่านคู่กับหัวข้อ 3 ได้ครับ หัวข้อนั้นไล่การเปลี่ยนแปลงทีละ Core ส่วนตารางนี้ผูกเทคโนโลยีกับอุปกรณ์และแยกปีของข้อกำหนดออกจากปีที่มีตัวอย่างผลิตภัณฑ์

| ช่วงเวลา | เทคโนโลยีและตัวอย่างอุปกรณ์ |
|---|---|
| **1994–1998 · เริ่มจากการแทนสาย** | Ericsson เริ่มศึกษาวิทยุระยะสั้นสำหรับมือถือและอุปกรณ์เสริม ก่อนร่วมก่อตั้ง Bluetooth SIG กับบริษัทอีกสี่รายในปี 1998 [เอกสาร Ericsson ปี 1999](https://www.ericsson.com/4ac5e9/assets/local/about-ericsson/ericsson-history/contact-kontakten/documents/1990s/ericsson_contact_1999_08.pdf) · [ประวัติ Bluetooth SIG](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/20year/default.html) |
| **1999–2003 · มือถือ พีซี และแฮนด์ฟรี** | ผู้ก่อตั้งประกาศ Bluetooth 1.0 ในเดือนกรกฎาคม 1999 SIG บันทึกว่ามี PC Card และมือถือ Bluetooth ออกสู่ตลาดในปี 2000 ตามด้วยชุดแฮนด์ฟรีรถในปี 2001 ต่อมามีโปรไฟล์เฉพาะงาน เช่น A2DP สำหรับส่งเสียงไปยังลำโพงหรือหูฟัง [ประกาศ 1.0 จาก Intel](https://www.intel.com/pressroom/archive/releases/1999/Mp072699.htm) · [ประวัติผลิตภัณฑ์จาก SIG](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/20year/default.html) · [ข้อกำหนด A2DP](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/a2dp_v1-4-1_1752513648/A2DP_v1.4.1/out/en/index-en.html) |
| **2004–2009 · ส่งข้อมูลเร็วและจับคู่ง่ายขึ้น** | 2.0 + EDR เพิ่มอัตราบิตของวิทยุ Classic, 2.1 + EDR เพิ่ม Secure Simple Pairing และ 3.0 + HS เพิ่มทางเลือกให้ลิงก์ส่งข้อมูลผ่าน 802.11 สำหรับอุปกรณ์ที่รองรับ [Core Change History §§3–5](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/core-specification-change-history.html) |
| **2001–2010 · จาก Wibree สู่ BLE** | Nokia เริ่มวิจัยการเชื่อมต่อพลังงานต่ำในปี 2001 นำ Wibree เข้าพัฒนาร่วมใน SIG ในปี 2007 และ BLE เข้า Core 4.0 ในปี 2010 ทำให้อุปกรณ์อย่างสายคาดวัดชีพจรและเซนเซอร์แบตเตอรี่ขนาดเล็กมีทางเลือกประหยัดพลังงาน [ประวัติ Wibree จาก Nokia](https://www.nokia.com/blog/how-nokia-triggered-the-global-rise-of-bluetooth-le/) · [ประกาศการรวมกลุ่ม Wibree](https://www.globenewswire.com/de/news-release/2007/06/12/1848406/0/en/wibree-forum-merges-with-bluetooth-sig.html) · [Bluetooth LE Primer](https://www.bluetooth.com/bluetooth-le-primer/) |
| **ยุค BLE Advertising · ประกาศข้อมูลโดยไม่ต้องเปิดการเชื่อมต่อ** | บีคอนส่งข้อมูลให้เครื่องที่กำลังสแกน เช่น Apple iBeacon ส่งตัวระบุผ่าน BLE Advertising ต่อมา Core 5.0 เพิ่ม Extended Advertising, LE 2M และ LE Coded จึงมีทางเลือกทั้งด้านขนาดข้อมูลประกาศ อัตราบิต และความทนทานเมื่อสัญญาณอ่อน [เอกสาร iBeacon จาก Apple](https://developer.apple.com/documentation/corelocation/turning-an-ios-device-into-an-ibeacon-device?language=objc) · [Core Change History §9](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/core-specification-change-history.html) |
| **2017 · Bluetooth Mesh** | เพิ่มข้อกำหนดเครือข่ายบน BLE ให้อุปกรณ์บางตัวส่งข้อความต่อได้หลายทอด ตัวอย่างคือสวิตช์ที่ควบคุมไฟทั้งโถงผ่านที่อยู่กลุ่ม Mesh มีชุดข้อกำหนดของตัวเอง [บทนำจาก SIG ปี 2017](https://www.bluetooth.com/blog/an-intro-to-bluetooth-mesh-part1/) · [Mesh Networking Primer](https://www.bluetooth.com/bluetooth-mesh-networking-primer/) |
| **2019–2024 · การหามุมและการวัดระยะ** | Core 5.1 เพิ่ม Direction Finding แบบ AoA/AoD เพื่อประมาณมุม ส่วน Core 6.0 เพิ่ม Channel Sounding เพื่อประมาณระยะระหว่างอุปกรณ์ ตัวอย่างฮาร์ดแวร์สำหรับทดลอง Channel Sounding คือชุด Nordic nRF54L15 DK สองชุด [Bluetooth LE Primer](https://www.bluetooth.com/bluetooth-le-primer/) · [Core 6.0 จาก SIG](https://www.bluetooth.com/core-specification-6-feature-overview/) · [ชุดทดลองจาก Nordic](https://www.nordicsemi.com/Products/Wireless/Bluetooth-Low-Energy/Channel-Sounding) |
| **2019–2022 · LE Audio และ Auracast** | Core 5.2 วางพื้นฐานการส่งเสียงแบบกำหนดจังหวะเวลา และ SIG ทำชุดข้อกำหนด LE Audio ครบในปี 2022 ตัวอย่างคือหูฟัง เครื่องช่วยฟัง และ Auracast ที่ให้เครื่องรับหลายเครื่องฟังเสียงจากแหล่งเดียวกัน เช่น เสียงทีวีในพื้นที่สาธารณะ [ข้อกำหนด LE Audio](https://www.bluetooth.com/learn-about-bluetooth/feature-enhancements/le-audio/le-audio-specifications/) · [ประกาศปี 2022](https://www.bluetooth.com/press/completion-le-audio/) |
| **2023 · ป้ายราคาดิจิทัล** | Core 5.4 เพิ่ม PAwR ให้ตัวรับตอบกลับตามช่องเวลาที่จัดไว้ โปรไฟล์ Electronic Shelf Label ใช้กลไกนี้สื่อสารกับป้ายราคาจำนวนมาก [Bluetooth ESL จาก SIG](https://www.bluetooth.com/blog/bluetooth-esl-the-global-standard-for-the-electronic-shelf-label-market/) |
| **2026 · Core 7.0 และ HDT** | HDT เพิ่มอัตราบิตระดับวิทยุของ BLE ได้ถึง 7.5 Mb/s บนอุปกรณ์ที่รองรับ SIG ยกการซิงก์ข้อมูลจากนาฬิกาและอัปเดตเฟิร์มแวร์เป็นตัวอย่างการใช้งาน ตัวอย่างเหล่านี้เป็นกรณีที่ SIG เสนอไว้ ส่วนการรองรับในผลิตภัณฑ์ให้ตรวจเอกสารของแต่ละรุ่น [ประกาศ HDT](https://www.bluetooth.com/press/bluetooth-high-data-throughput-opens-a-new-chapter-in-wireless-performance/) · [รายละเอียดทางเทคนิค HDT](https://www.bluetooth.com/bluetooth-high-data-throughput-technical-overview/) |

**ดูโปรไฟล์ควบคู่กับเลข Core** โปรไฟล์คือข้อกำหนดวิธีทำงานสำหรับอุปกรณ์ตามประเภทงาน เช่น ลำโพง Classic ใช้ A2DP สำหรับเสียง ส่วนคีย์บอร์ดหรือเมาส์ที่ใช้ BLE สามารถใช้ HID over GATT ส่วนระบบไฟ Bluetooth Mesh ใช้ Mesh Model สำหรับสวิตช์ หลอดไฟ และเซนเซอร์ [A2DP](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/a2dp_v1-4-1_1752513648/A2DP_v1.4.1/out/en/index-en.html) · [HID over GATT](https://www.bluetooth.com/specifications/specs/hid-over-gatt-profile/) · [Mesh Model](https://www.bluetooth.com/specifications/specs/mesh-model-1-0-1/)

## 7. อ่าน IEEE 802 เฉพาะส่วนที่โยงกับ Bluetooth

**IEEE 802** เป็นตระกูลมาตรฐานเครือข่าย และมีเอกสาร Overview & Architecture เป็นกรอบอธิบายชั้นโปรโตคอล ใต้ตระกูลนี้มี **802.15** เป็นคณะทำงานด้าน Wireless Specialty Networks (เอกสารยุค Bluetooth เรียกว่า WPAN Working Group) และ **802.11** เป็นคณะทำงานด้าน Wireless LAN [คณะกรรมการ IEEE 802](https://ieee802.org/) · [802 Overview & Architecture](https://www.ieee802.org/1/pages/802.html) · [ขอบเขตคณะทำงาน 802.15](https://www.ieee802.org/15/about.html)

| มาตรฐานหรือคณะทำงาน | ส่วนที่เกี่ยวและควรอ่าน |
|---|---|
| **IEEE 802.15.1-2002 และ -2005** | เกี่ยวโดยตรงกับ Bluetooth 1.1/1.2 อ่านขอบเขต MAC/PHY และประวัติการปรับจากฉบับแรกเป็นฉบับหลัง สำหรับแพ็กเก็ต BLE ให้อ่าน Core ของ SIG Vol. 6 Part B [IEEE Task Group 1](https://www.ieee802.org/15/pub/TG1.html) · [IEEE 802.15.1-2005](https://standards.ieee.org/ieee/802.15.1/3513/) · [LE Link Layer](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/low-energy-controller/linklayer-ll--specification.html) |
| **IEEE 802.15.2-2003** | แนวปฏิบัติการอยู่ร่วมกันในย่านไม่ต้องขอใบอนุญาต กรณีที่เอกสารระบุโดยตรงคือ WLAN แบบ 802.11b-1999 กับ WPAN แบบ 802.15.1-2002 เหมาะอ่านเรื่องการรบกวนและที่มาของ Adaptive Frequency Hopping (AFH) ฉบับนี้ถูกถอนจากสถานะใช้งานเมื่อ 7 พฤษภาคม 2018 [IEEE 802.15.2](https://standards.ieee.org/ieee/802.15.2/3293/) · [บทเรียนเรื่อง coexistence จาก IEEE](https://www.ieee802.org/Communications/14_11/19-14-0080-02-0000-Coexistence-Lessons-Learned.pdf) |
| **IEEE 802.11** | มาตรฐานวิทยุและ MAC สำหรับ WLAN ที่เป็นฐานของ Wi‑Fi เกี่ยวกับ Bluetooth สองทางคือการใช้คลื่น 2.4 GHz ร่วมกัน และบทบาทเฉพาะใน Bluetooth 3.0 + HS ซึ่งใช้ 802.11 เป็นช่องส่งข้อมูลเสริม [IEEE 802.11-2024](https://standards.ieee.org/ieee/802.11/10548/) · [Core Change History §5](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/core-specification-change-history.html) |
| **IEEE 802.15.4** | ใช้เป็นคู่เทียบเมื่ออ่านเรื่องวิทยุพลังงานต่ำหรือเครือข่าย Mesh เพราะเป็น PHY/MAC ที่ Thread และ Zigbee ใช้ ส่วน Bluetooth Mesh ใช้วิทยุและโปรโตคอล LE ของ SIG [IEEE 802.15.4-2024](https://standards.ieee.org/ieee/802.15.4/11041/) · [Thread Group](https://www.threadgroup.org/Portals/0/documents/support/ThreadBorderRouterWhitePaper_07192022_4001_1.pdf) · [Espressif เรื่อง Thread/Zigbee](https://docs.espressif.com/projects/esp-thread-br/en/latest/hardware_platforms.html) · [Bluetooth Mesh Primer](https://www.bluetooth.com/bluetooth-mesh-networking-primer/) |
| **IEEE 802.19** | คณะทำงานเรื่องการใช้คลื่นร่วมกัน แหล่งที่ตรงกับโจทย์คือบันทึกสรุปประสบการณ์เรื่อง Bluetooth กับ WLAN และจดหมายของ Bluetooth SIG เรื่อง Wi‑Fi 802.11n ช่องกว้าง 40 MHz ในย่าน 2.4 GHz [เอกสารบทเรียนจาก IEEE 802.19](https://www.ieee802.org/Communications/14_11/19-14-0080-02-0000-Coexistence-Lessons-Learned.pdf) · [จดหมายในคลัง IEEE](https://www.ieee802.org/19/arc/stds-802-19list/msg01028.html) |

**ตัวอย่างในโครงการ** Arduino Nano ESP32 ใช้ชิป ESP32-S3 ซึ่งรองรับ BLE และ Wi‑Fi 2.4 GHz บนชิปเดียว ฝั่ง Wi‑Fi และ BLE แชร์ทรัพยากรวิทยุโดยแบ่งเวลากันตามคู่มือของ Espressif ถ้าทดลองเปิด Wi‑Fi ระหว่างเก็บผล BLE ให้บันทึกโหมด Wi‑Fi และโหมดสแกนไว้ด้วย เพื่อแยกบริบทของการแชร์วิทยุบนบอร์ดออกจากการรบกวนในสภาพแวดล้อม [Arduino Nano ESP32](https://docs.arduino.cc/hardware/nano-esp32) · [ESP32-S3 Datasheet](https://documentation.espressif.com/esp32-s3_datasheet_en.pdf) · [คู่มือ RF Coexistence](https://docs.espressif.com/projects/esp-idf/en/v5.2.1/esp32s3/api-guides/coexist.html)

ถ้าเจอเลข **IEEE 802.15.3** ระหว่างค้นเอกสาร ให้ค้นด้วยชื่อ *High Data Rate Wireless Multi-Media Networks* ส่วนคำว่า **Bluetooth 3.0 + HS** ให้ค้นในประวัติ Core ของ SIG ซึ่งอ้างถึง 802.11 ในส่วน High Speed [ขอบเขต IEEE 802.15.3](https://standards.ieee.org/ieee/802.15.3/6211/) · [Core Change History §5](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/architecture%2C-change-history%2C-and-conventions/core-specification-change-history.html)

## 8. หนังสือและเอกสารอ่านต่อ

รายการนี้คัดตามหัวข้อในโครงการ ตั้งแต่ประวัติ Classic, สถาปัตยกรรม BLE, การทำอุปกรณ์จริง ไปจนถึงเสียงและ Mesh ลิงก์หนังสือชี้ไปยังสำนักพิมพ์ ผู้เขียน หรือ Bluetooth SIG เป็นหลัก ปีที่ระบุเป็นปีของฉบับที่อ้างถึง โดยบางเล่มมีปีฉบับพิมพ์และออนไลน์ต่างกัน

### หนังสือภาษาไทย

| หนังสือ | บทที่เกี่ยวและแหล่งอ่าน |
|---|---|
| **IoT สถาปัตยกรรมการสื่อสาร Internet of Things** — ชัชชัย คุณบัว, ซีเอ็ดยูเคชั่น | เปิดบทที่ 5 ตั้งแต่หน้า 91 โดย §5.2 ว่าด้วยสถาปัตยกรรม Classic และ §5.3 ว่าด้วย BLE เหมาะใช้ทวนศัพท์ไทยก่อนเปิดข้อกำหนดภาษาอังกฤษ [SE-ED E-Library](https://se-ed.belibcloud.com/book-detail/12856) · [ตัวอย่างและสารบัญ](https://bundanjai-static.reeeed.com/book/cka2de1dz1hcr0789u74xrc0p/preview/9786160835010_PDF.pdf?supportedpurview=project) |
| **พัฒนา IoT บนไมโครคอนโทรลเลอร์ ESP32 ด้วยภาษาไมโครไพทอน** — ชัชชัย คุณบัว, ซีเอ็ดยูเคชั่น | อ่านส่วน Bluetooth สำหรับลองทำอุปกรณ์ด้วย MicroPython และใช้คู่กับเอกสารฮาร์ดแวร์ของบอร์ด [รายการจากมหาวิทยาลัยสวนดุสิต](https://repository.dusit.ac.th/items/f910fd2b-974a-44c2-b88b-6450755c2521) · [ตัวอย่าง eBook](https://bundanjai-static.reeeed.com/book/clo5jjsw4k99b0789x6lofbs0/preview/9786160850563PDF.pdf?supportedpurview=project) |

### หนังสือยุค Bluetooth Classic

| หนังสือ | อ่านเพื่ออะไรและเข้าอ่านที่ไหน |
|---|---|
| **Bluetooth Revealed** — Brent A. Miller และ Chatschik Bisdikian, Pearson, 2000 | ที่มาของข้อกำหนด วิทยุ Bluetooth ยุคแรก และแนวคิดโปรไฟล์ อ่านสารบัญหรือตัวอย่างจากสำนักพิมพ์ [Pearson / InformIT](https://www.informit.com/store/bluetooth-revealed-the-insiders-guide-to-an-open-specification-9780130902948) |
| **Bluetooth 1.1: Connect Without Cables**, ฉบับ 2 — Jennifer Bray และ Charles F. Sturman, Pearson, ออกปี 2001 / ลิขสิทธิ์ 2002 | อ่านคู่กับ IEEE 802.15.1-2002 เพื่อเข้าใจ stack ฝั่ง Host/Controller และโปรไฟล์ยุค 1.1 [Pearson / InformIT](https://www.informit.com/store/bluetooth-1.1-connect-without-cables-9780130661067) |
| **Bluetooth: Operation and Use** — Robert Morrow, McGraw Hill, 2002 | พื้นฐานการทำงานของวิทยุและระบบ Bluetooth ยุคแรกสำหรับใช้ประกอบเรื่อง Classic [McGraw Hill](https://www.mheducation.com/highered/mhp/product/bluetooth-operation-use.html) |
| **Bluetooth Application Developer’s Guide** — Syngress, 2002 | ตัวอย่างการออกแบบแอปพลิเคชันและอุปกรณ์ฝังตัวในระบบ Bluetooth ยุคต้น [Elsevier / Syngress](https://shop.elsevier.com/books/bluetooth-application-developers-guide/syngress/978-1-928994-42-8) |
| **Bluetooth For Java** — Bruce Hopkins และ Ranjith Antony, Apress, 2003 | แหล่งเฉพาะทางสำหรับศึกษาการเขียนแอป Bluetooth บน Java/J2ME และ OBEX ในยุคนั้น [Springer / Apress](https://link.springer.com/book/10.1007/978-1-4302-0763-4) |
| **Bluetooth Essentials for Programmers** — Albert S. Huang และ Larry Rudolph, Cambridge University Press, พิมพ์ 2007 / ออนไลน์ 2009 | ตัวอย่าง Python, C, Java และ Linux ยุค Classic ผู้เขียนมีบทนำ GNU/Linux ให้เปิดอ่านฟรีด้วย [Cambridge](https://www.cambridge.org/core/books/bluetooth-essentials-for-programmers/B5A95969902955FDDF5601520ECAE611) · [บทนำจากผู้เขียน](https://people.csail.mit.edu/albert/bluez-intro/) |
| **Bluetooth Application Programming with the Java APIs Essentials Edition** — Timothy J. Thompson, C Bala Kumar และ Paul J. Kline, Morgan Kaufmann, 2008 | อ้างวิธีเรียกใช้ JSR-82 และโปรโตคอลอย่าง RFCOMM, OBEX และ L2CAP จากมุมของแอป Java [Elsevier / Morgan Kaufmann](https://shop.elsevier.com/books/bluetooth-application-programming-with-the-java-apis-essentials-edition/thompson/978-0-12-374342-8) |

### หนังสือ BLE และการทำอุปกรณ์

| หนังสือ | อ่านเพื่ออะไรและเข้าอ่านที่ไหน |
|---|---|
| **Bluetooth Low Energy: The Developer’s Handbook** — Robin Heydon, Pearson, ออกปี 2012 / ลิขสิทธิ์ 2013 | เหมาะไล่ตั้งแต่ PHY, Link Layer และ HCI ขึ้นมาถึง GAP/ATT/GATT ใน BLE ยุคแรก [Pearson / InformIT](https://www.informit.com/store/bluetooth-low-energy-the-developers-handbook-9780132888424) |
| **Getting Started with Bluetooth Low Energy** — Kevin Townsend, Carles Cufí, Akiba และ Robert Davidson, O’Reilly, 2014 | อ่านเริ่มต้นเรื่อง advertising, บทบาทอุปกรณ์ บริการและ characteristic พร้อมตัวอย่างพัฒนาอุปกรณ์ [O’Reilly](https://www.oreilly.com/library/view/getting-started-with/9781491900550/) |
| **Intro to Bluetooth Low Energy** — Mohammad Afaneh, Novel Bits | ผู้เขียนมี eBook ฉบับแรกให้ดาวน์โหลดฟรีและมีฉบับ 2 สำหรับผู้ต้องการอ่านเพิ่ม ครอบคลุมพื้นฐานโปรโตคอล บทบาท และการสื่อสาร BLE [หน้า eBook ของผู้เขียน](https://novelbits.io/intro-to-ble-book/) |
| **Develop your own Bluetooth Low Energy Applications** — Koen Vervloesem, Elektor | เหมาะกับโจทย์ทำอุปกรณ์จริง ใช้ Raspberry Pi กับ Python/Bleak, ESP32 กับ Arduino/NimBLE และ nRF52 กับ Zephyr [Elektor](https://www.elektor.com/products/develop-your-own-bluetooth-low-energy-applications) |
| **Introducing Bluetooth LE Audio**, ฉบับ 2 — Nick Hunn, เผยแพร่ผ่าน SIG ปี 2025 | อ่านพัฒนาการจาก Classic Audio ไปถึง LE Audio, LC3, การส่งเสียงที่คุมจังหวะ และ Auracast [หน้าหนังสือของ SIG](https://www.bluetooth.com/bluetooth-resources/le-audio-book-v2/) · [ดาวน์โหลด PDF ฉบับเต็มฟรี](https://www.bluetooth.com/wp-content/uploads/2025/01/Introduction-to-LE-Audio-2.0.pdf) |
| **Bluetooth LE Audio: Fundamental to Recent Advances** — Himanshu Bhalla และ Oren Haggai, Chapman & Hall, ลิขสิทธิ์ 2026 | ลงรายละเอียดชั้นแอป ชั้นควบคุม ชั้นขนส่ง และ codec LC3 สำหรับผู้ทำอุปกรณ์เสียง [Taylor & Francis](https://www.routledge.com/Bluetooth-LE-Audio-Fundamental-to-Recent-Advances/Bhalla-Haggai/p/book/9781032966199) |

### หนังสือเทียบ Bluetooth กับวิทยุอื่น

| หนังสือ | บทที่เกี่ยว |
|---|---|
| **Essentials of Short-Range Wireless** — Nick Hunn, Cambridge University Press, 2010 | ใช้เปรียบเทียบ Bluetooth, Wi‑Fi และ Zigbee ในภาพรวมของวิทยุระยะสั้น [Cambridge](https://www.cambridge.org/core/books/essentials-of-shortrange-wireless/D0E916CA9780D957F9CFE3481F47F085) |
| **5G/5G-Advanced, Wi-Fi 6/7, and Bluetooth 5/6: A Primer on Smartphone Wireless Technologies**, ฉบับ 2 — Douglas H. Morais, Springer, 2025 | อ่านบท *Bluetooth 5/6 Overview* หน้า 181–204 แล้วเปิดบท Wi‑Fi ประกอบเมื่อต้องอธิบายวิทยุต่างชนิดในโทรศัพท์เครื่องเดียว [Springer](https://link.springer.com/book/10.1007/978-3-031-82830-0) |

### เอกสารทางการที่ใช้คู่กับหนังสือ

| โจทย์ที่อยากอ่าน | แหล่งอ้างอิง |
|---|---|
| **อ่าน BLE ตั้งแต่วิทยุไปจนถึงข้อมูลในแอป** | [Bluetooth LE Primer ฉบับเว็บ](https://www.bluetooth.com/bluetooth-le-primer/) · [หน้าดาวน์โหลดจาก SIG](https://www.bluetooth.com/bluetooth-resources/the-bluetooth-low-energy-primer/) |
| **ตรวจฟิลด์แพ็กเก็ต ชนิดข้อมูลประกาศ และรหัสมาตรฐาน** | [Bluetooth Core Specification 7.0](https://www.bluetooth.com/specifications/specs/core-specification-7-0/) · [Core Specification Supplement หมวด Data Types](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/CSS_v14/out/en/core-supplementary-features/data-types-specification.html) · [Assigned Numbers](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/index-en.html) |
| **อ่าน Mesh, การวัดระยะ และ HDT** | [Mesh Networking Primer](https://www.bluetooth.com/bluetooth-mesh-networking-primer/) · [Channel Sounding Technical Overview](https://www.bluetooth.com/channel-sounding-tech-overview/) · [HDT Technical Overview](https://www.bluetooth.com/bluetooth-high-data-throughput-technical-overview/) |
| **หาแหล่งสำหรับบททบทวนวรรณกรรม** | งานเขียนเรื่องวิทยุ Bluetooth ของทีม Ericsson และงานทดลอง BLE ยุค 4.0 ที่รายงานวิธีวัดและผลทดลอง [Haartsen, The Bluetooth Radio System, 2000](https://doi.org/10.1109/98.824570) · [Haartsen และ Mattisson, Proceedings of the IEEE, 2000](https://research.utwente.nl/en/publications/bluetooth-a-new-low-power-radio-interface-providing-short-range-c/) · [Gómez, Oller และ Paradells, Sensors, 2012](https://pmc.ncbi.nlm.nih.gov/articles/PMC3478807/) |
| **หาเอกสาร IEEE ตามเลขมาตรฐาน** | เริ่มจากหน้ารายการและเส้นทางเข้าถึงของ IEEE เอกสารบางฉบับมีใน Get IEEE 802 ส่วนการเข้าถึงฉบับประวัติศาสตร์ขึ้นกับหน้ารายการของแต่ละฉบับ [IEEE 802 LMSC](https://ieee802.org/) · [IEEE 802.15.1-2005](https://standards.ieee.org/ieee/802.15.1/3513/) · [IEEE 802.15.2-2003](https://standards.ieee.org/ieee/802.15.2/3293/) |

**ลำดับอ่านสำหรับ Project Co-op** เริ่มที่ BLE Primer เรื่อง Advertising และ Scanning แล้วเปิด Core Vol. 6 Part B §2–3 ควบคู่กับตัวอย่างแพ็กเก็ตในหัวข้อ 4 พออ่านล็อกแล้วอยากค้นความหมายของ Flags หรือ Service UUID ให้เปิด Core Supplement กับ Assigned Numbers ส่วน IEEE 802.15.1 และ 802.15.2 ใช้ประกอบส่วนประวัติและเรื่องการใช้คลื่นร่วมกับ Wi‑Fi ครับ [BLE Primer](https://www.bluetooth.com/bluetooth-le-primer/) · [LE Link Layer](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v7.0/out/en/low-energy-controller/linklayer-ll--specification.html) · [คู่มือ BLE Behavior Logger](../experiments/firmware/ble_behavior_logger/README.md)

## คำถามทบทวน

1. **Bluetooth 3.0 เป็น IEEE 802.15.1 หรือเปล่า** — IEEE ฉบับสุดท้ายอิง Bluetooth 1.2 ส่วน 3.0 + HS เป็นรุ่น Core ของ SIG ที่เพิ่มทางเลือกใช้ 802.11 ส่งข้อมูล
2. **เห็นเลข 7.0 แล้วจะใช้ HDT ได้เลยไหม** — ต้องดูการรองรับ HDT ของอุปกรณ์ทั้งสองฝั่งและโหมดที่ใช้ เพราะ HDT เป็นฟีเจอร์เสริม
3. **ในล็อกเห็นชื่อเครื่องแล้วแปลว่าอ่าน GATT สำเร็จไหม** — ชื่ออาจมาจาก Advertising หรือ Scan Response ที่ตัวสแกนรับได้ก่อนเชื่อมต่อ ส่วนการอ่าน characteristic ผ่าน GATT เป็นอีกขั้นตอนหนึ่ง

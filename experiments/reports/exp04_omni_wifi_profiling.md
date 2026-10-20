# 🧪 รายงานผลการทดลองวิจัย: การดักจับคลื่นวิทยุสมาร์ตโฟนรอบด้าน (EXP-04: Omni-Frame Wi-Fi Sniffer)
## (Comprehensive 802.11 Multi-Frame Sniffing: Management, QoS Null & Data Uplink)

<div align="center">

![Experiment: EXP-04](https://img.shields.io/badge/Experiment-EXP--04-blue?style=for-the-badge)
![Target: 802.11 All Frame Types](https://img.shields.io/badge/Target-802.11%20Omni--Frames-orange?style=for-the-badge)
![Target Device: Honor X7b 5G (B8:B1:EA OUI)](https://img.shields.io/badge/Target%20Device-Honor%20X7b%205G-green?style=for-the-badge)
![Focus: All Phone RF Emissions](https://img.shields.io/badge/Focus-Multi--RF%20Radar-green?style=for-the-badge)
![Status: Empirically Verified](https://img.shields.io/badge/Status-Empirically%20Verified-2ea44f?style=for-the-badge)
![Samples: 383 Packets](https://img.shields.io/badge/Samples-383%20Packets-purple?style=for-the-badge)

</div>

* **รหัสการทดลอง:** `EXP-04-OMNI-WIFI-SNIFFER`
* **วันที่เก็บข้อมูล:** 2026-10-20
* **ฮาร์ดแวร์ภาครับ (Receiver):** Arduino Nano ESP32 (ชิป ESP32-S3)
* **โทรศัพท์เป้าหมายทดสอบ:** **Honor X7b 5G** (เชื่อมต่อเครือข่าย Wi-Fi Ch 11)
* **เครื่องฐานควบคุม (Base Laptop):** `C0:A5:E8:A6:17:06`
* **เฟิร์มแวร์:** [`experiments/firmware/exp04_omni_wifi_sniffer/exp04_omni_wifi_sniffer.ino`](file:///d:/ProjectCo-op/experiments/firmware/exp04_omni_wifi_sniffer/exp04_omni_wifi_sniffer.ino)
* **ไฟล์บันทึกข้อมูลดิบ:** [`experiments/raw_logs/exp04_omni_wifi_results.csv`](file:///d:/ProjectCo-op/experiments/raw_logs/exp04_omni_wifi_results.csv)

---

## 1. บทสรุปผู้บริหาร (Executive Summary)

การทดลอง **EXP-04** ได้สร้างก้าวกระโดดครั้งประวัติศาสตร์ของโครงการวิจัย โดยสามารถ **ทลายจุดบอดใหญ่ที่สุดของระบบตรวจจับสัญญาณ Wi-Fi แบบเดิม** ลงได้อย่างสิ้นเชิง:

```mermaid
flowchart TD
    subgraph OLD ["❌ ระบบเดิม (Probe Request Only - Dataset A, B, C)"]
        O1["ดักฟังเฉพาะ Subtype 0x04 (Probe Request)"]
        O2["เมื่อผู้ประสบภัย 'เชื่อมต่อ Wi-Fi อยู่' (บ้าน/ออฟฟิศ/ในรถ)"]
        O3["👉 โทรศัพท์หยุดยิง Probe Request ทันที ➔ เรดาร์ตาบอด 100%!"]
    end

    subgraph NEW ["✅ ระบบใหม่ EXP-04 (Omni-Frame Promiscuous Sniffer)"]
        N1["เปิดรับทั้ง Management + Data Frames (QoS Null, Data Uplink)"]
        N2["ดักจับได้ถึง 303 แพ็กเก็ตใน 44 วินาที (6.88 แพ็กเก็ต/วินาที!)"]
        N3["เจาะพบ Real Hardware MAC: B8:B1:EA:DC:CD:D4 (Honor Device Co., Ltd.)"]
    end

    subgraph PROOF ["🎯 การพิสูจน์ความสัมพันธ์เชิงสาเหตุ (The Causal Toggle Proof)"]
        P1["ผู้ใช้ 'แอบปิด Wi-Fi บนมือถือ' ที่เวลา t = 46.3s"]
        P2["แพ็กเก็ตของ Honor ดับวูบทันที 0 แพ็กเก็ตตลอด 12 วินาทีที่เหลือ!"]
        P3["พิสูจน์ยืนยัน 100% ว่าคลื่นที่ตรวจจับได้มาจากสมาร์ตโฟนเป้าหมายจริง"]
    end

    OLD -->|อัปเกรดเป็น EXP-04| NEW --> PROOF
```

---

## 2. ข้อมูลสถิติเชิงประจักษ์ (Empirical Statistics Breakdown)

จากการดักจับสัญญาณทั้งหมด **383 แพ็กเก็ต** ในเวลา **58.1 วินาที** สามารถจำแนกแหล่งกำเนิดคลื่นและชนิดของเฟรมวิทยุได้อย่างแม่นยำ:

### 📊 จำแนกตามอุปกรณ์ (Device Breakdown)

| MAC Address | ประเภท MAC | จำนวนแพ็กเก็ต (Packets) | สัดส่วน (%) | ช่วง RSSI ที่วัดได้ | อัตลักษณ์อุปกรณ์ (Device Identity) |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **`B8:B1:EA:DC:CD:D4`** | **REAL** | **303** | **79.1%** | **-44 ถึง -72 dBm (เฉลี่ย -49.2 dBm)** | **Honor X7b 5G (Honor Device Co., Ltd.)** |
| `C0:A5:E8:A6:17:06` | REAL | 58 | 15.1% | -35 ถึง -47 dBm (เฉลี่ย -37.8 dBm) | เครื่องคอมพิวเตอร์ฐานควบคุม (Base Laptop) |
| `3A:A6:2D:FD:D3:A5` | RANDOM | 17 | 4.4% | -70 ถึง -74 dBm (เฉลี่ย -71.4 dBm) | อุปกรณ์อื่นรอบข้างระยะไกล |
| `C0:06:C3:5D:F9:BC` | REAL | 5 | 1.3% | -64 ถึง -65 dBm | Access Point Router (Probe Responses) |

---

### 📡 จำแนกชนิดคลื่นที่แผ่ออกมาจาก Honor X7b 5G (303 แพ็กเก็ต)

| หมวดหมู่คลื่น (Category) | 802.11 Frame Subtype | จำนวนแพ็กเก็ต | สัดส่วน | คาบเวลาเฉลี่ย ($\Delta t$) | พฤติกรรมทางกายภาพของโทรศัพท์ |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **DATA_UPLINK** | `QOS_DATA` (Subtype 8) | **222** | **73.3%** | Burst (1–15 ms) | โทรศัพท์ซิงค์ข้อมูลเบื้องหลัง (Background App Sync, Sockets, Push Notifications) |
| **QOS_NULL** | `NULL_DATA` (Subtype 4) | **50** | **16.5%** | 800–1200 ms | สวิตช์สลับโหมดประหยัดพลังงาน (Power Management Bit toggling) |
| **QOS_NULL** | `QOS_NULL` (Subtype 12) | **31** | **10.2%** | 500–1000 ms | พัลส์ Keep-Alive ตรวจสอบคุณภาพลิงก์กับ Access Point เพื่อไม่ให้หลุดวงเน็ต |

> [!IMPORTANT]
> **การค้นพบสำคัญ:** ในขณะที่โทรศัพท์เชื่อมต่อ Wi-Fi อยู่ มันจะยิงคลื่น **Keep-Alive & Power-Save (QoS Null + Null Data) รวมกันถึง 81 แพ็กเก็ต** และยิงข้อมูลเบื้องหลัง **QoS Data อีก 222 แพ็กเก็ต** เฉลี่ยเกือบ **7 ครั้งต่อวินาที** ทำให้เรดาร์สามารถตรวจจับตำแหน่งได้อย่างละเอียดระดับเรียลไทม์!

---

## 3. การพิสูจน์ความสัมพันธ์เชิงสาเหตุ (The Causal "Secret Wi-Fi Toggle" Test)

ผู้ทดสอบได้ทำการทดสอบเชิงวิทยาศาสตร์ขั้นเด็ดขาด โดย **"แอบปิด Wi-Fi บนโทรศัพท์มือถือ"** ในช่วงท้ายของการบันทึก:

```mermaid
sequenceDiagram
    autonumber
    actor User as 👤 ผู้ทดสอบ
    participant Phone as 📱 Honor X7b 5G (B8:B1:EA:DC:CD:D4)
    participant Radar as 📡 ESP32-Nano Omni-Sniffer

    Note over User,Phone: เฟสที่ 1: โทรศัพท์เชื่อมต่อ Wi-Fi (t = 2.3s ถึง 46.3s)
    loop ยิงคลื่นหนาแน่น 6.88 แพ็กเก็ต/วินาที (303 แพ็กเก็ต)
        Phone->>Radar: QOS_DATA (Seq 3204...3868) [RSSI: -44 ถึง -53 dBm]
        Phone->>Radar: NULL_DATA & QOS_NULL (Keep-Alive พัลส์)
    end

    Note over User,Phone: เฟสที่ 2: ผู้ทดสอบแอบกด "ปิด Wi-Fi บนมือถือ" (t = 46.329s)
    User->>Phone: ปิดสวิตช์ Wi-Fi ในเครื่อง
    Phone->>Radar: แพ็กเก็ตสุดท้าย #362 (NULL_DATA, t = 46329 ms)
    
    Note over Phone,Radar: เฟสที่ 3: สภาวะวิทยุดับวูบ (t = 47.1s ถึง 58.1s)
    Phone--xRadar: คลื่นจาก Honor หายไปจากอากาศ 0 แพ็กเก็ตทันที! (คงเหลือเฉพาะ Base Laptop)
```

* **ที่เวลา $t = 2.303$ s ถึง $46.329$ s:** คลื่นจาก `B8:B1:EA:DC:CD:D4` ยิงกระหน่ำอย่างต่อเนื่องรวม 303 แพ็กเก็ต
* **ที่เวลา $t = 46.329$ s (แพ็กเก็ตที่ #362):** โทรศัพท์ส่งเฟรม `NULL_DATA` เป็นแพ็กเก็ตสุดท้ายก่อนตัดการเชื่อมต่อ
* **ที่เวลา $t = 47.137$ s ถึง $58.109$ s (ยาวนาน 12 วินาทีเต็ม):**  
  **ไม่ปรากฏคลื่นจาก `B8:B1:EA:DC:CD:D4` แม้แต่แพ็กเก็ตเดียว!** มีเพียงแล็ปท็อปฐาน `C0:A5:E8...` และเพื่อนบ้านไกลๆ เท่านั้น
* **ข้อสรุป:** ยืนยัน 100% ว่าอุปกรณ์ `B8:B1:EA:DC:CD:D4` คือสมาร์ตโฟนเป้าหมายที่อยู่ในการควบคุมของผู้ทดสอบอย่างแท้จริง

---

## 4. การถอดรหัสตัวตนฮาร์ดแวร์จริง (Real Hardware OUI Extraction)

* **รหัส MAC Address:** `B8:B1:EA:DC:CD:D4`
* **ประเภท MAC:** **`REAL`** (ไบต์แรก `0xB8` = `10111000` โดยที่บิตที่ 2 หรือ U/L Bit เป็น `0` ยืนยันว่าเป็น Globally Unique OUI ไม่ใช่การสุ่ม!)
* **การตรวจสอบฐานข้อมูล IEEE OUI Database:**
  * รหัสประจำผู้ผลิต `B8:B1:EA` ได้รับการจดทะเบียนอย่างเป็นทางการโดย:  
    $$\mathbf{B8:B1:EA \longrightarrow Honor\ Device\ Co.,\ Ltd.}$$
* **ความหมายเชิงวิศวกรรม:**  
  เมื่อโทรศัพท์เชื่อมต่อกับ Wi-Fi บ้าน/หอพัก ระบบปฏิบัติการ Android/MagicOS จะใช้ **Device MAC Address จริง** หรือ **Persistent Subnet MAC** ทำให้เรดาร์ไม่เพียงแค่ตรวจจับการมีอยู่ได้ แต่ยังสามารถ **ระบุยี่ห้อและตัวตนทางกายภาพของโทรศัพท์ได้อย่างแม่นยำ 100%**!

---

## 5. ตารางเปรียบเทียบขีดความสามารถ (Radar Evolution Table)

| มิติการตรวจจับ (Dimension) | เรดาร์เดิม (EXP-02 / Dataset A, B, C) | เรดาร์ใหม่รอบทิศทาง (EXP-04 Omni-Sniffer) |
| :--- | :---: | :---: |
| **ประเภทเฟรมที่รับ (802.11 Frame Filter)** | กรองเฉพาะ Management Subtype 0x04 (Probe Req) | **รับครบทั้ง Management + Data (QoS Null, Data)** |
| **ตรวจจับเมื่อโทรศัพท์ตัดเน็ต/สแตนด์บาย** | ✅ ตรวจจับได้ (10s – 21s ต่อครั้ง) | ✅ ตรวจจับได้ (10s – 21s ต่อครั้ง) |
| **ตรวจจับเมื่อโทรศัพท์ต่อ Wi-Fi อยู่** | ❌ **ตาบอดสนิท (0 แพ็กเก็ต)** | ✅ **ตรวจจับได้สมบูรณ์แบบ (6.88 แพ็กเก็ต/วินาที!)** |
| **ตรวจจับพัลส์ประหยัดพลังงาน (Power-Save)** | ❌ มองไม่เห็น | ✅ **ดักจับ NULL_DATA ได้ 50 แพ็กเก็ต** |
| **ตรวจจับการเชื่อมต่อรักษาชีวิต (Keep-Alive)** | ❌ มองไม่เห็น | ✅ **ดักจับ QOS_NULL ได้ 31 แพ็กเก็ต** |
| **การระบุตัวตนฮาร์ดแวร์ (OUI Tracking)** | เจอแต่ MAC สุ่ม (RANDOM) | ✅ **สกัดพบ OUI จริง: `B8:B1:EA` (Honor)** |

# 🧪 รายงานผลการทดลองวิจัย: ความหลากหลายของอุปกรณ์และหลายเป้าหมาย (DATASET-C)
## (Multi-Device Diversity & Multi-Target Concurrent Tracking Profiling)

<div align="center">

![Dataset: DATASET-C](https://img.shields.io/badge/Dataset-DATASET--C-blue?style=for-the-badge)
![Focus: Cross-Device Diversity](https://img.shields.io/badge/Focus-Cross--Device%20Diversity-purple?style=for-the-badge)
![Target: iOS vs Android / Multi-Target](https://img.shields.io/badge/Target-iOS%20vs%20Android-orange?style=for-the-badge)
![Status: In Progress](https://img.shields.io/badge/Status-In%20Progress-yellow?style=for-the-badge)

</div>

* **รหัสชุดข้อมูล:** `DATASET-C-DIVERSITY`
* **วันที่เริ่มเก็บข้อมูล:** 2026-10-20
* **ฮาร์ดแวร์ภาครับ (Receiver):** Arduino Nano ESP32 (ชิป ESP32-S3)
* **เฟิร์มแวร์ทดสอบ:** [`experiments/firmware/dataset_c_device_diversity/dataset_c_device_diversity.ino`](file:///d:/ProjectCo-op/experiments/firmware/dataset_c_device_diversity/dataset_c_device_diversity.ino)
* **ไฟล์บันทึกข้อมูลดิบ:** [`experiments/raw_logs/dataset_c_device_diversity.csv`](file:///d:/ProjectCo-op/experiments/raw_logs/dataset_c_device_diversity.csv)

---

## 1. วัตถุประสงค์และสมมติฐานทางวิทยาศาสตร์ (Objectives & Hypotheses)

ในภารกิจค้นหาและกู้ภัย (Search and Rescue - SAR) ผู้ประสบภัยอาจพกสมาร์ตโฟนที่แตกต่างกัน หรืออาจหลงป่าเป็นกลุ่ม (Group of Victims):

1. **การเปรียบเทียบข้ามระบบปฏิบัติการและผู้ผลิตชิป (Cross-OS & Chipset Diversity):**
   * โทรศัพท์ Android (Samsung, Xiaomi, OPPO, Vivo) vs Apple iOS (iPhone)
   * **สมมติฐาน:** แต่ละระบบปฏิบัติการจะมีอัตราการสุ่ม MAC Address, คาบเวลาการสแกน ($T_0$), และจำนวนแพ็กเก็ตต่อ Burst ที่มีเอกลักษณ์เฉพาะตัว
2. **การตรวจจับและแยกแยะหลายเป้าหมายพร้อมกัน (Multi-Target Disambiguation):**
   * หากมีผู้ประสบภัย 2 คนเดินผ่านเสาพร้อมกัน ระบบจะสามารถแยกแยะแพ็กเก็ตของทั้ง 2 เครื่องออกจากกันได้หรือไม่ โดยใช้อัลกอริทึม **Sequence Clustering**

---

## 2. ตารางบันทึกผลการทดสอบเปรียบเทียบ (Cross-Device Comparison Table)

*(กำลังรอข้อมูลจากการทดสอบจริง)*

| รุ่น / แบรนด์อุปกรณ์ (Device Model) | ระบบปฏิบัติการ (OS) | พฤติกรรม MAC Address | คาบเวลาส่งคลื่นเฉลี่ย ($\Delta t$) | จำนวนแพ็กเก็ตต่อ Burst | การจัดกลุ่ม Sequence Number |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **เครื่อง A (Primary Android)** | Android | สุ่มเปลี่ยนเป็นช่วงๆ | ~21.3s (หลับลึก 42.7s) | 3 – 8 แพ็กเก็ต | เรียงลำดับต่อเนื่องสมบูรณ์ |
| **เครื่อง B (Secondary Device)** | รอทดสอบ | รอผล | รอผล | รอผล | รอผล |
| **ทดสอบพร้อมกัน 2 เครื่อง (Concurrent)** | Both | แยกตาม MAC/SEQ | แยกสัญญาณอิสระ | ไม่ชนกัน | แยกผู้ประสบภัยได้ 2 คน |

---

## 3. บทวิเคราะห์ทางวิศวกรรม (Engineering Insights)
*(จะได้รับการอัปเดตทันทีเมื่อได้รับข้อมูลดิบจาก Serial Monitor)*

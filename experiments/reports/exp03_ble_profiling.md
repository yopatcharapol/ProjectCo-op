# 🧪 รายงานผลการทดลองวิจัย: การดักจับคลื่นบลูทูธพลังงานต่ำ (EXP-03: BLE Sniffer)
## (Smartphone Bluetooth Low Energy RF Emission & Beacon Profiling)

<div align="center">

![Experiment: EXP-03](https://img.shields.io/badge/Experiment-EXP--03-blue?style=for-the-badge)
![Target: BLE 2.4GHz Advertising](https://img.shields.io/badge/Target-BLE%20Advertising-purple?style=for-the-badge)
![Focus: All Phone RF Emissions](https://img.shields.io/badge/Focus-Multi--RF%20Radar-green?style=for-the-badge)
![Status: Ready for Testing](https://img.shields.io/badge/Status-Ready%20for%20Testing-yellow?style=for-the-badge)

</div>

* **รหัสการทดลอง:** `EXP-03-BLE-SNIFFER`
* **วันที่เริ่มเก็บข้อมูล:** 2026-10-20
* **ฮาร์ดแวร์ภาครับ (Receiver):** Arduino Nano ESP32 (ชิป ESP32-S3, BLE 5.0)
* **เฟิร์มแวร์:** [`experiments/firmware/exp03_ble_sniffer/exp03_ble_sniffer.ino`](file:///d:/ProjectCo-op/experiments/firmware/exp03_ble_sniffer/exp03_ble_sniffer.ino)
* **ไฟล์บันทึกข้อมูลดิบ:** [`experiments/raw_logs/exp03_ble_results.csv`](file:///d:/ProjectCo-op/experiments/raw_logs/exp03_ble_results.csv)

---

## 1. วัตถุประสงค์และสมมติฐานทางวิทยาศาสตร์ (Objectives & Hypotheses)

การสร้าง "ดราก้อนเรดาร์" ตรวจจับโทรศัพท์ที่สมบูรณ์แบบ ต้องไม่พึ่งพาคลื่นเพียงย่านเดียว:
1. **การค้นพบชีพจรคลื่นความถี่สูง (High-Frequency BLE Pulse):**
   * ในขณะที่ Wi-Fi Probe Request ยิงทุกๆ 10 ถึง 21 วินาที
   * **สมมติฐาน:** คลื่น Bluetooth Low Energy (BLE) แผ่สัญญาณออกจากสมาร์ตโฟน **ถี่กว่ามาก (ทุกๆ 200 ms ถึง 2 วินาที)** ผ่านโปรโตคอล Google Fast Pair, Nearby Discovery, หรือ Find My
2. **การระบุเอกลักษณ์ด้วย Manufacturer ID:**
   * ตรวจจับและแยกแยะรหัสค่าย เช่น `0x038F` (Xiaomi), `0x00E0` (Google), `0x004C` (Apple), `0x0075` (Samsung)

---

## 2. ตารางบันทึกผลการทดสอบคลื่น BLE (BLE Empirical Table)

*(กำลังรอข้อมูลจากการทดสอบจริง)*

| ค่าย / รุ่นโทรศัพท์ | คลื่น BLE ที่แผ่ออกมา | Company Identifier | ช่วงเวลาส่งคลื่น ($\Delta t$) | ช่วง RSSI ที่ตรวจพบ | ข้อสรุปเชิงการใช้งานเรดาร์ |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **POCO X3 Pro (Xiaomi)** | รอทดสอบ | รอผล | รอผล | รอผล | รอผล |
| **Honor X7b 5G** | รอทดสอบ | รอผล | รอผล | รอผล | รอผล |
| **อุปกรณ์รอบข้าง / อื่นๆ** | รอทดสอบ | รอผล | รอผล | รอผล | รอผล |

---

## 3. บทวิเคราะห์ทางวิศวกรรม (Engineering Insights)
*(จะได้รับการอัปเดตทันทีเมื่อได้รับข้อมูลจาก Serial Monitor)*

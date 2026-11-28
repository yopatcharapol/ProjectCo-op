# Project Co-op: Bluetooth Behavior Study with ESP32

**เป้าหมายปัจจุบัน:** ใช้ ESP32 เก็บล็อกเพื่อศึกษาพฤติกรรม Bluetooth และเงื่อนไขที่ตรวจพบอุปกรณ์ ชุดถัดไปใช้แท็บเล็ต Redmi Pad Pro แล้วค่อยขยายไปหูฟัง นาฬิกา และอุปกรณ์อื่น

อัปเดตทิศทาง: 17 พฤศจิกายน 2026 ตามคำแนะนำอาจารย์ที่ผู้ทำโครงการแจ้ง

## เริ่มอ่านที่นี่

1. [ขอบเขตวิจัยปัจจุบัน](docs/CURRENT_RESEARCH_SCOPE.md) — คำถามวิจัย ข้อจำกัด และบทบาทของเอกสารเก่า
2. [Project Roadmap](process_and_workflow/project_roadmap.md) — ขั้นตอนทดลองและงานที่ต้องทำถัดไป
3. [ประวัติการเปลี่ยนทิศทาง](PROJECT_EVOLUTION.md) — จาก LoRa Mesh และ Wi-Fi/BLE มาสู่การศึกษา Bluetooth

## ระบบที่กำลังจะศึกษา

อุปกรณ์เป้าหมาย → สัญญาณ BLE Advertising → ESP32 สแกนรับ → USB Serial → ไฟล์ล็อกพร้อมสถานะทดลอง → สรุปว่าตรวจพบเมื่อไรและมีหลักฐานระบุเครื่องอย่างไร

ใช้ Nano ESP32 / ESP32-S3 สแกน BLE จดรุ่นย่อยและเวอร์ชันระบบของแท็บเล็ตก่อนเก็บชุดใหม่ การเปิด Bluetooth ไม่รับรองว่าจะสแกนพบ และยังไม่ถือว่า RSSI บอกระยะจริงหรือจุดบนเรดาร์บอกทิศทางได้

## สถานะงาน

- มี [firmware BLE EXP-03 เดิม](experiments/firmware/exp03_ble_sniffer/exp03_ble_sniffer.ino) และ [ข้อมูลทดลองเดิม](experiments/raw_logs/) ใช้เป็นฐานศึกษา
- ใช้ [ตัวเก็บล็อก BLE และคู่มือทดลองเองผ่าน Arduino](experiments/firmware/ble_behavior_logger/README.md) กับ Nano ESP32 + Redmi Pad Pro สำหรับรอบถัดไป
- [ผลนำร่อง 5 เงื่อนไขชุดที่ 1](experiments/reports/ble_pilot_01_honor_states.md) มาจาก Honor X7b 5G รวม 2,344 REPORT จากรอบที่จบครบ ใช้อ้างอิงวิธีทดลองและข้อจำกัดของการระบุเครื่อง
- LoRa Mesh, Wi-Fi/BLE fusion และระบบเรดาร์กู้ภัยเป็นประวัติ/ทางเลือกอนาคต ไม่ใช่แผนส่งมอบปัจจุบัน

## โครงสร้าง Repo

| ตำแหน่ง | เนื้อหา |
| :--- | :--- |
| `process_and_workflow/project_roadmap.md` | แผนปัจจุบันด้าน Bluetooth |
| `docs/CURRENT_RESEARCH_SCOPE.md` | ขอบเขตหลักและผลตรวจความสอดคล้องเอกสาร |
| `experiments/firmware/` | โค้ดทดลองเดิม BLE, Wi-Fi และ dual-radio |
| `experiments/raw_logs/`, `experiments/reports/` | ข้อมูลดิบและรายงานตามเงื่อนไขเดิม |
| `experiments/dashboard/` | ต้นแบบจอเรดาร์เดิม |
| `docs/08`–`docs/12` และ phase report | ความรู้และรายงานวิจัยเดิม อ่านประกอบโดยตรวจข้อจำกัด |
| `reports_and_notes/` | บันทึกและข้อเสนอในช่วงก่อนปรับแผน |
| `process_and_workflow/conversation_history.md` | ประวัติการสนทนาเดิม |
| `archive_initial_research/` | งานศึกษา LoRa ตั้งต้นและ Roadmap ฉบับเดิม |

ดู [สารบัญการทดลองเดิม](experiments/README.md) เพื่อค้นโค้ดและผลย้อนหลัง เอกสารที่กล่าวว่าเสร็จสมบูรณ์หรือเสนอ KPI ของระบบกู้ภัยต้องอ่านตามบริบทในเวลานั้น ไม่ใช่สถานะของแผนใหม่

`share_friend/` เป็นคลังส่วนตัวที่ Git ไม่ติดตาม จึงไม่ใช้เป็นจุดเริ่มอ่านหรือแหล่งแผนหลักของ Repo นี้

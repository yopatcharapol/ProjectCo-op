# BLE Behavior Logger — ชุดทดลองนำร่อง

ชุดถัดไปใช้ Arduino Nano ESP32 ตัวเดิมและแท็บเล็ต Redmi Pad Pro จดรุ่นย่อย เวอร์ชัน Android/HyperOS จากเครื่อง และ core ที่เลือกใช้จริงก่อนเก็บข้อมูล

เป็น sketch ใหม่เพื่อรักษา EXP-03 และข้อมูลเก่าไว้ ใช้ BLE GAP legacy scan reports ผ่าน Bluedroid โดยไม่รวม MAC เป็นเครื่องเดียว ไม่ประมาณระยะ และไม่แปล Manufacturer ID เป็นฟีเจอร์โดยอัตโนมัติ

## ทดลองเองผ่าน Arduino Serial Monitor

ใช้ `ble_behavior_logger.ino` ล่าสุดพร้อมไฟล์ `BleAdParser.h` ในโฟลเดอร์เดียวกัน แล้ว Verify/Upload ใหม่เพื่อใช้การแยกค่าบนบอร์ด เปิด Serial Monitor ที่ 115200 baud และเปลี่ยนสถานะบนแท็บเล็ตก่อนส่งคำสั่ง

| คำสั่ง | สถานะที่คุณจัดเตรียมบนแท็บเล็ต | ระยะสแกน |
| :--- | :--- | :--- |
| `off` | ปิด Bluetooth | 30 วินาที |
| `on` | เปิด Bluetooth อยู่หน้าโฮม | 30 วินาที |
| `share` | เปิดหน้า Quick Share พร้อมรับ ให้ทุกคนมองเห็น | 30 วินาที |
| `lock` | คง Bluetooth/Quick Share เดิมแล้วล็อกจอ | 30 วินาที |
| `unlock` | ปลดล็อกกลับหน้า Quick Share | 30 วินาที |
| `stop` | ขอหยุดก่อนครบเวลา | หยุดเมื่อ controller ยืนยัน |
| `status` / `help` | ตรวจสถานะ / ดูคำสั่ง | ไม่เริ่มสแกน |

เลือกทำรอบไหนก่อนก็ได้ โปรแกรมไม่บังคับลำดับ แต่รอรอบปัจจุบันจบก่อนเริ่มรอบใหม่ แต่ละรอบแยก trial ID และติด condition ตามคำสั่งเพื่อไม่ให้รอบ Quick Share/ล็อกจอมีป้ายเหมือนหน้าโฮม

`REPORT` เก็บเวลา, MAC, address type, RSSI, event type, ความยาวและ payload ดิบตามเดิม ส่วน `AD_DETAILS` เป็นแถว EVENT ที่เพิ่มการแยกชื่อ, Manufacturer ID, Service UUID 16/32/128 บิต, TX power ที่ประกาศ, Flags และการพบ Service UUID 0xFEF3 จาก Advertising และ Scan Response บนบอร์ดโดยตรง

จะแสดง `AD_DETAILS` เมื่อพบ MAC/payload ชุดใหม่ในรอบนั้น เพื่อลดข้อความซ้ำ ใช้ cache 24 รายการแบบวนทับ จึงอาจมีรายละเอียดซ้ำอีกเมื่อรายการเก่าถูกแทนที่ ข้อมูลที่ไม่มีประกาศใช้ `not_advertised` ไม่เดาค่าเพิ่ม `decode_warnings` บอก AD ที่ยาวเกินขอบเขต/ผิดรูป หรือช่องแสดงผลเต็ม; payload ดิบยังเก็บใน REPORT

ใช้ชื่อและรหัสบริการเป็นเบาะแสประกอบรอบควบคุม `fef3_service=yes` ระบุว่าพบรหัสที่ตามอยู่ ส่วน TX power มาจากข้อมูลที่เครื่องส่งประกาศ

ไฟล์ CMD ตัวเปิดถูกลบตามคำขอแล้ว เครื่องมือ PowerShell เดิมยังเก็บไว้เป็นทางเลือก แต่ไม่จำเป็นสำหรับการทดลองผ่าน Serial Monitor ส่วนรูปแบบ CSV เดิมยังคง 20 คอลัมน์ เพื่อให้เครื่องมือเก่าอ่านได้

## เตรียมบอร์ด

1. เปิด `ble_behavior_logger.ino` ใน Arduino IDE เข้า Boards Manager ตรวจแพ็กเกจ **Arduino ESP32 Boards by Arduino** เวอร์ชัน **2.0.18-arduino.5** (มีติดตั้งอยู่แล้วในเครื่องที่ตรวจ)
2. เลือก **Tools → Board → Arduino ESP32 Boards → Arduino Nano ESP32** และพอร์ตของบอร์ด ใช้ FQBN `arduino:esp32:nano_nora` ไม่ใช่ Nano ESP32 ในหมวด `esp32 by Espressif Systems` ซึ่งมีชื่อบอร์ดซ้ำกัน ชุดที่รองรับมี Bluedroid และ BLEDevice ติดมาด้วย ไม่ต้องเพิ่มไลบรารีภายนอก จดชื่อแพ็กเกจและเวอร์ชันจริง
3. เปิด USB CDC ตามการตั้งค่าบอร์ด และปิด Core Debug Level เพื่อไม่ให้ข้อความ debug ปะปน
4. Verify ก่อน Upload แล้วเปิด Serial Monitor ที่ 115200 baud, Newline ควรเห็น `READY` หรือสั่ง `status` เพื่อขอสถานะ
5. ยังไม่ถือว่าใช้งานกับฮาร์ดแวร์ผ่านแล้วจนกว่าจะเห็น `SCAN_STARTED`, รายงาน/heartbeat และ `TRIAL_END` ในไฟล์จริง

### หากเจอ `esp_gap_ble_api.h: No such file or directory`

ทำซ้ำ Error นี้ได้ด้วย FQBN `esp32:esp32:nano_nora` บนแพ็กเกจ Espressif 3.3.12 ที่ติดตั้งในเครื่อง: SDK ของ ESP32-S3 ชุดนี้ใช้ NimBLE และไม่มี header Bluedroid ที่ logger ต้องใช้ ให้เลือกแพ็กเกจ Arduino ตามข้อ 1–2 แล้ว Verify ใหม่ ไม่ต้องลบแพ็กเกจ Espressif ไม่ต้องคัดลอก header ข้าม core และไม่แก้ `sdkconfig.h` ของแพ็กเกจ

โค้ดจะแจ้งชื่อแพ็กเกจที่ต้องเลือกโดยตรงหากเลือก core ที่ไม่ใช่ Bluedroid การรองรับ NimBLE ต้องพอร์ต API แยก ไม่ได้แก้ด้วยการเปลี่ยนชื่อ include เพียงอย่างเดียว

## ใช้งานด้วย Serial Monitor

### แบบสั้น: เริ่มจากวิธีนี้

Upload firmware ล่าสุดก่อน ใช้ Serial Monitor ที่ 115200 baud:

1. ปิด Bluetooth บนแท็บเล็ต วางที่เดิม ส่ง `off` แล้วปล่อยไว้ บอร์ดสแกน 30 วินาทีและหยุดเอง เก็บข้อความรอบนี้เป็น `off.txt`
2. เปิด Bluetooth บนแท็บเล็ต กลับหน้าโฮม ส่ง `on` แล้วรอหยุดเอง เก็บข้อความรอบนี้เป็น `on.txt`

เปลี่ยนสถานะ Bluetooth บนเครื่องเป้าหมายก่อนส่งคำสั่งสั้น คำสั่งสั้นของ firmware รุ่นนี้ติดป้าย `REDMI_PAD_PRO` หากยังใช้ firmware ที่ติดป้าย Honor ให้อัปโหลดรุ่นนี้หรือใช้คำสั่ง `start` ที่ระบุป้ายกำกับเอง

`on`, `off`, `share`, `lock`, `unlock`, `stop`, `status`, `help` ใช้ได้ทั้ง Newline, Carriage return, Both NL & CR และ No line ending (แบบสุดท้ายรอรับคำสั่งหลังไม่มีอักษรเพิ่ม 300 ms) ส่วนคำสั่ง `start ...` แบบยาวยังต้องมี Newline หรือ Carriage return

ส่ง `stop` จะมี `COMMAND_RECEIVED` ยืนยันก่อน ถ้ากำลังสแกนจะตามด้วย `STOP_REQUESTED` และเมื่อ controller หยุดแล้วจึงมี `TRIAL_END` หากไม่ได้สแกนจะตอบ `STOPPED;no scan is running` ไม่ต้องรอจบรอบที่ไม่มีอยู่

หากส่งแล้วไม่มีการตอบสนองทุกคำสั่ง ยังสรุปไม่ได้ว่าเป็นปัญหาที่ตัวคั่นบรรทัด ต้องตรวจพอร์ต การ Upload และการเชื่อมต่อเพิ่มเติม ไม่ต้องเก็บข้อมูลซ้ำโดยไม่ตรวจสถานะก่อน

### แบบกำหนดชื่อรอบและเงื่อนไขเอง

ส่งคำสั่งทีละบรรทัด ใช้ label ภาษาอังกฤษ ตัวเลข `_` หรือ `-` ยาวไม่เกิน 40 ตัว:

```text
start S01 trial01 REDMI_PAD_PRO BACKGROUND passive
status
stop
```

รอ `TRIAL_END` ก่อนเริ่มรอบใหม่ เปลี่ยน `passive` เป็น `active` สำหรับการทดลองอีกชุด สแกนจำกัดรอบละ 600 วินาทีหากไม่สั่งหยุด ไม่เริ่มเองหลัง reset และไม่เปลี่ยนป้ายกำกับกลางรอบ

## บันทึกไฟล์ด้วย PowerShell

ปิด Serial Monitor ก่อน เพราะเปิดพอร์ตพร้อมกันไม่ได้ ดูพอร์ตจาก Arduino IDE แล้วแทน `COM7` ด้วยค่าจริง รันจากโฟลเดอร์ราก Repo:

```powershell
powershell -NoProfile -ExecutionPolicy RemoteSigned -File "experiments/tools/capture_ble_trial.ps1" -Port "COM7" -DeviceLabel "REDMI_PAD_PRO" -Condition "BACKGROUND" -DurationSeconds 300 -TrialId "trial01" -PhoneOs "UNKNOWN" -CoreVersion "UNKNOWN"
```

แทน `UNKNOWN` ด้วยค่าจริงที่ทราบ ใช้ `-PhoneOs` ใส่เวอร์ชัน Android/HyperOS ของแท็บเล็ตตามชื่อพารามิเตอร์เดิม `-ExecutionPolicy RemoteSigned` ใช้เฉพาะ process นี้เพื่อให้รันสคริปต์ที่สร้างในเครื่องได้ ไม่เปลี่ยนนโยบายถาวร สคริปต์บันทึก CSV, ไฟล์ Serial ดิบ และ metadata JSON ใน `experiments/raw_logs/ble_behavior/` ชื่อไฟล์แยกรอบอัตโนมัติ ไม่เขียนทับชุดเดิม รอข้อความ `Completed` หากผิดพลาด/ถูกหยุด metadata จะคง `completed=false` ไว้ ต้องตรวจไฟล์ก่อนนำไปสรุป

  ## นำร่อง 3 สถานะ

ใช้ Passive Scan ก่อน รอบละ 5 นาที วางแท็บเล็ตห่าง 1 เมตรเมื่ออยู่ในพื้นที่และคงทิศวางเดิม ไม่เปิด Quick Share ระหว่างรอบควบคุม จดอุปกรณ์อื่น ระยะจริง เวลาเปลี่ยนสถานะ และเวลารอใน `-Notes`

| ลำดับ | Condition | ทำอะไร |
| :--- | :--- | :--- |
| 1 | `BACKGROUND` | นำ Redmi Pad Pro ออกจากพื้นที่รับเท่าที่ทำได้และจดตำแหน่งจริง |
| 2 | `BT_OFF` | นำแท็บเล็ตมาวางที่จุดทดสอบ ปิด Bluetooth ใน Settings จดการตั้งค่าสแกนเบื้องหลัง |
| 3 | `BT_ON_HOME` | เปิด Bluetooth กลับหน้าโฮม คงสถานะหน้าจอให้เหมือนกันตลอดรอบ |

เปลี่ยน `-Condition` ตามตาราง เมื่อผ่านนำร่องแล้วจึงทำซ้ำอย่างน้อย 3 รอบโดยแยก `-TrialId` และทดลอง Active Scan เป็นชุดแยก `-Mode active` ไม่สรุปว่าเปิด Bluetooth แล้วต้องพบ หรือว่าไม่พบแปลว่าไม่มีสัญญาณ Bluetooth

ก่อนใช้ชุดอัตโนมัติ 5 สถานะ ให้ตรวจว่าแท็บเล็ตมีหน้า Quick Share โหมดรับ รวมถึงตัวเลือกการมองเห็นที่ใช้ได้จริง ถ้าเครื่องใช้ฟีเจอร์แชร์คนละแบบ ให้เก็บรอบ `SHARING_ON` แยกและจดชื่อฟีเจอร์กับการตั้งค่าใน `-Notes` [คู่มือ Quick Share ของ Android](https://support.google.com/android/answer/15728591?hl=en)

## ความหมายข้อมูล

ทุกแถวมี 20 คอลัมน์ตามหัว CSV `REPORT` เป็นผลจาก BLE stack ส่วน `EVENT` เป็นสถานะและเหตุการณ์ ตัวเลขเวลาเป็น `millis()` ที่บอร์ดนับตั้งแต่ boot

- `session_id`, `trial_id`, `device_label`, `condition`, `scan_mode`: ป้ายกำกับการทดลอง ไม่ยืนยันว่าทุกผลรับมาจากเครื่องที่ระบุ
- `report_id`: ลำดับผลรับของ callback ต่อรอบ ช่องว่างอาจเกิดจากรายงานที่ตัวเก็บล็อกทิ้ง
- `mac`, `addr_type`, `event_type`: ค่าที่ GAP รายงาน ไม่ใช้บิต MAC ของ Wi-Fi มาจัดประเภท BLE
- `rssi_dbm`: ความแรงดิบ ไม่ได้แปลงเป็นเมตร
- `adv_len`, `scan_rsp_len`, `adv_hex`, `scan_rsp_hex`: แยกส่วนตามความยาวที่ GAP รายงาน ไม่ใช่ raw radio frame พร้อม channel/CRC และอาจมีข้อมูลโฆษณาที่ stack เก็บร่วมกับ scan response
- `event`, `detail`: `BOOT`, `READY`, `TRIAL_BEGIN`, `SCAN_CONFIG`, `SCAN_STARTED`, `HEARTBEAT`, `STATUS`, `TRIAL_END` หรือข้อผิดพลาด
- `dropped_reports`: จำนวนผลที่รับ callback แล้วแต่คิวเต็มหรือความยาวเกินขอบเขตที่ logger รองรับ สะสมต่อรอบ ไม่รวมแพ็กเก็ตที่รับไม่ถึงบอร์ดหรือ USB ฝั่งคอมพิวเตอร์ทำหาย

ตั้ง scan interval 100 ms, window 80 ms, ปิด duplicate filtering และคิว 128 รายงาน เก็บชื่อ Manufacturer Data และ Service UUID ในรูป payload ดิบก่อน ยังไม่มีคอลัมน์แปลข้อมูลเหล่านี้ ต้องตรวจภายหลังโดยเก็บค่าเดิมไว้

ข้อจำกัด: ไม่รองรับ Bluetooth Classic หรือ extended advertising ใน logger รุ่นนี้; รายงานจาก stack ไม่เท่ากับทุกแพ็กเก็ตบนอากาศ; ช่วงห่างผลรับไม่ใช่ advertising interval จริงโดยอัตโนมัติ; ในพื้นที่หนาแน่น Serial อาจระบายไม่ทัน ต้องตรวจ `dropped_reports` และความสมบูรณ์ไฟล์ทุกครั้ง

อ้างอิงการสแกนและโครงสร้างรายงาน: [Espressif BLE GAP API](https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/bluetooth/esp_gap_ble.html)

## ผลตรวจซอฟต์แวร์ก่อนทดลอง

17 พฤศจิกายน 2026: firmware ที่เพิ่ม `AD_DETAILS` และคำสั่งแยกสถานะคอมไพล์ผ่านด้วย Arduino CLI 1.6.0, FQBN `arduino:esp32:nano_nora`, core `arduino:esp32` 2.0.18-arduino.5 ใช้ flash 923,557 bytes และ global RAM 58,276 bytes ต้อง Verify/Upload รุ่นนี้ใหม่ก่อนใช้ค่าที่เพิ่ม ส่วนล็อกที่ผู้ทำโครงการส่งจากรุ่นก่อนหน้ายืนยันการเริ่ม รับรายงาน และหยุดอัตโนมัติครบ 30 วินาทีแล้ว ไม่ใช่การยืนยันค่าที่ถอดใหม่ การหยุดด้วยมือ หรือการระบุตัว Honor อย่างถาวร แพ็กเกจ Espressif 3.3.12 ที่ติดตั้งอยู่เป็น NimBLE-only และไม่รองรับ logger นี้

ตัวช่วยเก็บเป็นชุดผ่านการตรวจ syntax และทดสอบแบบไม่ใช้บอร์ด: ลำดับ 5 สถานะ/รหัสรอบซ้ำ, การสร้าง ZIP, การหยุดเมื่อรอบไม่สมบูรณ์, การออกก่อนรอบแรก, การอ่านชื่อ/Manufacturer ID/Service UUID/TX power, การแยก UUID ใน Scan Response, การไม่สับสน bytes ภายใน Manufacturer Data กับ Service UUID และการแสดงรอบว่าง/ข้อมูลผิดรูป ยังไม่ได้ทดสอบตัวช่วย PowerShell แบบครบชุดกับพอร์ตบอร์ดจริง

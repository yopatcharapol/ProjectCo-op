---
name: conversation-logger
description: >-
  Trigger with English commands: 'log conversation', 'update log', 'record progress', 'sync log', or 'log decisions'.
  Summarizes and records project progress, discussions, decisions, and tables into documentation files.
  All outputs, interactions, and questions to the user must be in Thai.
---

# Conversation Logger & Progress Tracker Skill

ทักษะสำหรับการสรุปเนื้อหา อัปเดตประเด็นการสนทนา และบันทึกประวัติการพัฒนาโครงการ (Project Decision & Progress Log) อย่างเป็นระบบ

---

## 🎯 English Activation Commands (คำสั่งเรียกใช้งานเป็นภาษาอังกฤษ)
เรียกใช้งาน Skill นี้เมื่อผู้ใช้พิมพ์คำสั่งภาษาอังกฤษ:
* `log conversation` (คำสั่งหลักที่แนะนำ)
* `update log`
* `record progress`
* `sync log`
* `log decisions`

> 🌐 **Language Protocol (กฎการใช้ภาษา):**
> * **คำสั่งสั่งการ (User Input Trigger):** ใช้ภาษาอังกฤษ เช่น `log conversation`
> * **การตอบกลับและการถามผู้ใช้ (Interaction & Questions):** **ต้องเป็นภาษาไทยเสมอ**
> * **การสร้างบันทึกผลลัพธ์ (Generated Output):** เขียนบันทึกเป็น **ภาษาไทย** สั้น กระชับ ชัดเจน พร้อมตารางสรุป

1. สกัดประเด็นสำคัญ (Key Discussion Points), คำถามหลัก, ข้อตกลง/การตัดสินใจ (Decisions Made)
2. นำไปบันทึกประวัติอย่างเป็นระเบียบลงใน [`process_and_workflow/conversation_history.md`](file:///d:/ProjectCo-op/process_and_workflow/conversation_history.md) พร้อม Timestamp
3. **[สำคัญ - ฝั่ง Research & yo_research]:** หากการพูดคุยเกี่ยวข้องกับงานวิจัย, ทฤษฎี, โปรโตคอลวิทยุ หรือไฟล์ในโฟลเดอร์ `share_friend` ให้นำข้อสรุปไปบันทึกต่อท้ายลงใน [`share_friend/Research/yo_research/yo_research_log.md`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/yo_research_log.md) (และปรับปรุง [`share_friend/Research/สรุป/yopaper.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/yopaper.md) หากเป็นการวิเคราะห์เปเปอร์) เสมอ เพื่อป้องกันไม่ให้ข้อมูลสูญหาย
4. อัปเดตไฟล์เอกสารที่เกี่ยวข้องโดยอัตโนมัติ (เช่น Roadmap, Comparison Matrix, Advisor Report หรือ README)

---

## 📋 ขั้นตอนการทำงาน (Step-by-Step Procedure)

### ขั้นตอนที่ 1: สกัดใจความสำคัญจากการสนทนาล่าสุด (Information Extraction)
แยกแยะข้อมูลออกเป็น 5 ส่วน:
1. **หัวข้อหลัก (Topic):** ประเด็นที่กำลังพูดคุยหรือปรึกษา
2. **ปัญหา / คำถาม (Problem / Question):** ปัญหาที่พบ หรือคำถามที่ต้องการคำตอบ
3. **ข้อสรุปและข้อตัดสินใจ (Decisions & Solutions):** แนวทางที่เห็นชอบร่วมกัน ตัวเลือกสถาปัตยกรรม หรืออัลกอริทึมที่เลือก
4. **ตารางเปรียบเทียบและข้อมูลสำคัญ (Tables & Comparative Matrices):** ⚠️ **สำคัญ:** หากในการพูดคุยมีตารางเปรียบเทียบ ตารางสรุป หรือตารางวิเคราะห์ที่สร้างขึ้น ให้ดึงตาราง Markdown ฉบับเต็มมาบันทึกแสดงด้วยเสมอ ห้ามย่อเหลือแค่ข้อความสั้น
5. **สิ่งที่จะทำต่อไป (Action Items / Next Steps):** แผนงานขั้นถัดไปที่ต้องลงมือทำ

### ขั้นตอนที่ 2: บันทึกลงใน `process_and_workflow/conversation_history.md`
เขียนบันทึกประวัติการพูดคุยต่อท้าย (Append) ไฟล์ประวัติ โดยใช้รูปแบบมาตรฐาน:

```markdown
## 📅 [mm:HH ddd-MMM-YYYY] - หัวข้อการพูดคุย

### 1. สรุปประเด็นการสนทนา
* ...

### 2. ข้อตกลงและการตัดสินใจ (Decisions Made)
* [x] **การตัดสินใจ 1:** ...
* [x] **การตัดสินใจ 2:** ...

### 3. ตารางประกอบการวิเคราะห์และการเปรียบเทียบ (Analysis & Comparison Tables)
*(หากมีการสร้างตารางในการสนทนา ให้ยกตาราง Markdown ฉบับเต็มมาแสดงไว้ที่นี่)*

### 4. ผลกระทบต่อโครงสร้างงาน (Artifacts & Files Updated)
* อัปเดตไฟล์: [`path/to/file.md`](file:///d:/ProjectCo-op/path/to/file.md)

### 5. แผนงานขั้นต่อไป (Next Action Items)
* [ ] งานที่ต้องทำในสเต็ปถัดไป...
```

### ขั้นตอนที่ 3: ตรวจสอบและอัปเดตเอกสารโครงการที่เกี่ยวข้อง (Cross-File Synchronization)
* **หากเป็นการสนทนาเกี่ยวกับงานวิจัย ทฤษฎี หรือการวิเคราะห์เปเปอร์ใน `share_friend` / `yo_research`:** ให้อัปเดตและบันทึกลงใน [`share_friend/Research/yo_research/yo_research_log.md`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/yo_research_log.md) และ [`share_friend/Research/สรุป/yopaper.md`](file:///d:/ProjectCo-op/share_friend/Research/%E0%B8%AA%E0%B8%A3%E0%B8%B8%E0%B8%9B/yopaper.md) เสมอ เพื่อให้มั่นใจว่าข้อมูลที่แชร์กับเพื่อนไม่สูญหาย
* **หากมีข้อตกลงเรื่องแผนงาน/ระยะเวลา:** ให้อัปเดต [`process_and_workflow/project_roadmap.md`](file:///d:/ProjectCo-op/process_and_workflow/project_roadmap.md)
* **หากมีข้อสรุปที่ต้องรายงานอาจารย์:** ให้อัปเดต [`ADVISOR_REPORT.md`](file:///d:/ProjectCo-op/ADVISOR_REPORT.md)
* **หากมีการเปรียบเทียบหรือสถาปัตยกรรมใหม่:** ให้อัปเดต [`research_notes/comparison_matrix.md`](file:///d:/ProjectCo-op/research_notes/comparison_matrix.md)
* **หากมีการสร้างไฟล์ใหม่:** ให้อัปเดตสารบัญใน [`README.md`](file:///d:/ProjectCo-op/README.md) เสมอ

### ขั้นตอนที่ 4: สรุปผลให้ผู้ใช้ทราบ
รายงานให้ผู้ใช้ทราบสั้นๆ ว่า:
* สรุปประเด็นอะไรบ้าง
* ได้บันทึกและอัปเดตไฟล์ใดไปแล้วบ้าง (พร้อมใส่ลิงก์คลิกเปิดไฟล์ได้ทันที)

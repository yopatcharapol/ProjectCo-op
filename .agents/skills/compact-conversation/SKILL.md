---
name: compact-conversation
description: >-
  Trigger with English commands: 'compact conversation', 'compress conversation', 'conversation checkpoint', 'compact summary', or 'compress context'.
  Generates a high-density, token-efficient 6-pillar conversation compaction summary.
  All outputs, interactions, and questions to the user must be presented in clear, concise Thai with an English Vocabulary Bank.
---

# ⚡ Compact Conversation Skill (High-Density Context Preservation)

Skill สำหรับการสังเคราะห์และบีบอัดประวัติการสนทนา (Conversation Compaction) ให้มีความกระชับขั้นสุด ประหยัดโทเคน (Token-Efficient) แต่เก็บรักษา **"บริบทและกฎเหล็กที่สำคัญยิ่งยวดครบ 6 เสาหลัก"** เพื่อป้องกันไม่ให้ AI เกิดอาการหลงทาง (Context Drift) หรือลืมข้อตกลงเดิมเมื่อการสนทนายาวขึ้น

---

## 🎯 English Activation Commands (คำสั่งเรียกใช้งานเป็นภาษาอังกฤษ)

เรียกใช้งาน Skill นี้เมื่อผู้ใช้พิมพ์คำสั่งภาษาอังกฤษดังนี้:
* `compact conversation` (คำสั่งหลักที่แนะนำ)
* `compress conversation`
* `conversation checkpoint`
* `compact summary`
* `compress context`

> 🌐 **Language Protocol (กฎการใช้ภาษา):**
> * **คำสั่งสั่งการ (User Input Trigger):** ใช้ภาษาอังกฤษสั้นๆ เช่น `compact conversation`
> * **การตอบกลับและการถามผู้ใช้ (Interaction & Questions):** **ต้องเป็นภาษาไทยเสมอ** (ภาษาพูดเข้าใจง่าย ชัดเจน ไม่เวิ่นเว้อ)
> * **การสร้างผลลัพธ์ (Generated Output):** เขียนสรุปเป็น **ภาษาไทย** โดยมีตาราง **คลังคำศัพท์ภาษาอังกฤษ (Vocab Bank)** และประโยคสำคัญพร้อมคำแปลประกอบเสมอ เพื่อประหยัดโทเคนและช่วยจำ

---

## 🏛️ โครงสร้าง 6 เสาหลักที่ต้องมีเสมอ (The 6 Essential Pillars)

ทุกครั้งที่ประมวลผล สรุปจะต้องครอบคลุม 6 มิติต่อไปนี้อย่างครบถ้วน:

```
┌────────────────────────────────────────────────────────────────────────────────────────┐
│                        COMPACT CONVERSATION ARCHITECTURE                               │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ 1. 🎯 Current Goal & Task          │ เป้าหมายหลักปัจจุบันและงานที่กำลังโฟกัส             │
│ 2. 🚫 Strict Constraints & Rules   │ กฎเหล็ก ข้อห้ามเด็ดขาด และกฎการจัดการไฟล์/โฟลเดอร์    │
│ 3. 📊 Progress & File Registry     │ สถานะ Kanban (Success/Doing) และสารบัญไฟล์พร้อมลิงก์ │
│ 4. 🧠 Key Technical Decisions      │ ข้อสรุปทางเทคนิค สาระสำคัญของงานวิจัย/ทฤษฎี          │
│ 5. 👤 User Preference & Style      │ ภาษาไทยเข้าใจง่าย + คลังคำศัพท์อังกฤษ (Vocab Bank)  │
│ 6. ⏭️ Next Action Item             │ ก้าวต่อไปที่ต้องลงมือทำทันทีโดยไม่ต้องรอสั่งซ้ำ       │
└────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 📋 แม่แบบมาตรฐานการแสดงผล (Output Template in Thai + Vocab Bank)

```markdown
# ⚡ สรุปย่อบริบทบทสนทนา (Compact Conversation Summary)

> **สถานะบันทึก:** Active Project Checkpoint  
> **วัน-เวลา:** [mm:HH ddd-MMM-YYYY]  
> **โปรเจกต์:** [ชื่อโปรเจกต์]  

---

### 1. 🎯 Current Goal & Task (เป้าหมายปัจจุบัน)
* **เป้าหมายหลัก:** [ระบุเป้าหมายใหญ่ของโปรเจกต์ใน 1 ประโยค]
* **งานที่กำลังทำอยู่ขณะนี้:** [ระบุหัวข้อหรือ Task ที่กำลังดำเนินการอย่างเจาะจง]

---

### 2. 🚫 Strict Constraints & Negative Rules (กฎเหล็กและข้อห้ามเด็ดขาด)
* ❌ **ข้อห้ามเด็ดขาด (Negative Constraints):** [เช่น ห้ามยุ่งกับ LoRa ในตอนนี้, ห้ามแก้ไขโค้ดส่วน X]
* 📂 **กฎการจัดการไฟล์ (Pathing Rules):** [เช่น ไฟล์ทั้งหมดต้องสร้างใน `share_friend/` เท่านั้น ห้ามสร้างโฟลเดอร์ซ้อน]

---

### 3. 📊 Progress & File Registry (สถานะงานและสารบัญไฟล์)
* **สถานะความคืบหน้า (Kanban):**
  * ✅ **Success:** [ระบุหัวข้อที่สำเร็จแล้ว เช่น 01, 02, 03]
  * 🔄 **Doing:** [ระบุหัวข้อที่กำลังทำ เช่น 04]
  * 📋 **Todo:** [ระบุหัวข้อที่รอทำลำดับถัดไป]
* **สารบัญไฟล์สำคัญ (File Registry):**
  * 📄 `[ชื่อไฟล์]`: [path/to/file.md](file:///d:/ProjectCo-op/...) — [คำอธิบายสั้นๆ 1 บรรทัด]

---

### 4. 🧠 Key Technical Decisions & Discoveries (ข้อสรุปทางเทคนิค)
* **ข้อตกลงสำคัญ:** [สรุปประเด็นทางวิศวกรรม/ทฤษฎีที่ตกลงกันแล้ว]
* **เอกสารอ้างอิงหลัก:** [งานวิจัยหรือมาตรฐานที่ใช้ เช่น IEEE Std 802.11-2024, เปเปอร์ใน yo_research]

---

### 5. 👤 User Preference & Communication Style (สไตล์ที่ผู้ใช้ต้องการ)
* **รูปแบบการตอบ:** ภาษาไทย เข้าใจง่าย สั้นกระชับ ตัดน้ำท่วมทุ่งออก
* **คลังคำศัพท์ภาษาอังกฤษ (Vocab Bank):**
  | คำศัพท์ (Vocab) | คำอ่าน | ความหมาย / หน้าที่ |
  | :--- | :--- | :--- |
  | [คำศัพท์] | [คำอ่าน] | [ความหมาย] |
* **ประโยคสำคัญ (Key Sentences):**
  * *"[English Sentence]"*  
    *([คำแปลภาษาไทย])*

---

### 6. ⏭️ Next Action Item (ก้าวต่อไปที่ต้องทำทันที)
* [ ] [ระบุงานชิ้นถัดไปที่ต้องทำทันที พร้อมเป้าหมายผลลัพธ์ที่ชัดเจน]
```

---

## 🛠️ ขั้นตอนการประมวลผลของ Agent (Execution Workflow)

เมื่อได้รับคำสั่งภาษาอังกฤษ (เช่น `compact conversation`):
1. **Analyze Context:** กวาดอ่านบริบททั้งหมด ดึง 6 เสาหลักออกมา
2. **Verify Constraints:** ยืนยันข้อห้าม (เช่น ห้าม LoRa, กฎโฟลเดอร์ `share_friend/`) ให้ชัดเจน
3. **Format in Thai:** เรียบเรียงเนื้อหาเป็น **ภาษาไทยที่กระชับที่สุด** บรรจุคลังคำศัพท์ภาษาอังกฤษ (Vocab Bank) พร้อมประโยคแปลไทย
4. **Save Target:** นำเนื้อหาไปบันทึกหรืออัปเดตลงใน [`share_friend/Research/yo_research/yo_research_log.md`](file:///d:/ProjectCo-op/share_friend/Research/yo_research/yo_research_log.md) หรือไฟล์ประวัติที่เกี่ยวข้อง
5. **Output to User:** ตอบกลับผู้ใช้ในหน้าต่างแชทด้วยภาษาไทยพร้อมผลสรุป 6 เสาหลักทันที

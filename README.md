# 🏫 NPIC Smart Attendance System (IoT) - Pro Edition

**Description | ការពិពណ៌នា:**
A professional IoT-based smart attendance management system. It integrates ESP32 hardware, NFC, and Fingerprint scanning with a real-time Cloud Database. The system features a unified Web Dashboard for seamless data analytics and remote device management.

ប្រព័ន្ធគ្រប់គ្រងវត្តមានឆ្លាតវៃ (IoT) កម្រិតអាជីព ដែលប្រើប្រាស់បច្ចេកវិទ្យា ESP32, ស្កេនកាត (NFC), និងក្រយៅដៃ (Fingerprint)។ ទិន្នន័យត្រូវបានផ្ទុកដោយស្វ័យប្រវត្តិទៅកាន់ Google Sheets ភ្ជាប់ជាមួយផ្ទាំង Dashboard សម្រាប់មើលរបាយការណ៍ និងគ្រប់គ្រងម៉ាស៊ីន (All-in-One)។

---

## 📁 Files Included (ឯកសារក្នុងគម្រោង)

1. **`Smart_Scanner.ino`** 
   * C++ Firmware សម្រាប់បញ្ជាម៉ាស៊ីន ESP32, សេនស័រក្រយៅដៃ (AS608), អេក្រង់ OLED និងភ្ជាប់ទៅកាន់ Cloud។
2. **`teacher.html`** 
   * **Unified Pro Dashboard:** ផ្ទាំងគ្រប់គ្រងរួម ដែលមានរបាយការណ៍វត្តមានសិស្ស (Analytics) និងផ្ទាំងគ្រប់គ្រងម៉ាស៊ីន (Admin Device Management) តាមរយៈ MQTT ក្នុងវិបសាយតែមួយ។
3. **`index.html`** 
   * **Student Web App:** ផ្ទាំងវិបសាយសម្រាប់សិស្សស្កេនវត្តមានដោយខ្លួនឯង ភ្ជាប់ជាមួយប្រព័ន្ធផ្ទៀងផ្ទាត់ទីតាំង GPS (កំណត់ត្រឹម ៥ គីឡូម៉ែត្រពីសាលា)។

---

## 🛠️ Technologies Used (បច្ចេកវិទ្យាដែលបានប្រើប្រាស់)

* **Hardware:** ESP32 DevKit V1, AS608 Fingerprint Sensor, NFC Reader, OLED SSD1306.
* **Frontend:** HTML5, CSS3 (Modern UI/UX), JavaScript, MQTT.js (HiveMQ Broker).
* **Backend & Database:** Google Apps Script, Google Sheets (Master Database).

---

## 👨‍💻 Authors (រៀបចំដោយ)

* **BEAV CHHUNLY** (បៀវ ឈុនលី)
* **SEN SEAKNANN** (សេន សៀកណាន់)
* **MOM SEYAROW** (មុំ សិយ៉ារ៉ូ)

**Class:** Year 3 (ឆ្នាំទី ៣) | **Group:** B.EcE.A  
**Institution:** វិទ្យាស្ថានជាតិពហុបច្ចេកទេសកម្ពុជា (NPIC)

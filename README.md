# 🏫 NPIC Smart Attendance System (IoT)

**Description:**
ប្រព័ន្ធគ្រប់គ្រងវត្តមានឆ្លាតវៃ ប្រើប្រាស់បច្ចេកវិទ្យា ESP32, ស្កេនកាត (NFC), និងក្រយៅដៃ (Fingerprint)។ ទិន្នន័យទាំងអស់ត្រូវបានផ្ទុកដោយស្វ័យប្រវត្តិទៅកាន់ Google Sheets (Master Database)។

**Files Included ឯកសាររួមមាន៖**
1. `Smart_Scanner.ino`: កូដ C++ សម្រាប់បញ្ជាម៉ាស៊ីន ESP32 និងគ្រឿងអេឡិចត្រូនិក។
2. `admin.html`: ផ្ទាំងគ្រប់គ្រងសម្រាប់ Admin ចុះឈ្មោះ ឬលុបក្រយៅដៃសិស្ស តាមរយៈ MQTT។
3. `index.html`: ផ្ទាំងវិបសាយសម្រាប់សិស្សស្កេនវត្តមាន ភ្ជាប់ជាមួយប្រព័ន្ធតាមដានទីតាំង GPS (៥ គីឡូម៉ែត្រ)។

**Technologies Used បច្ចេកវិទ្យាដែលបានប្រើប្រាស់៖**
- Hardware: ESP32, Adafruit Fingerprint Sensor, OLED SSD1306.
- Frontend: HTML, CSS, JavaScript, MQTT (HiveMQ Broker).
- Backend & Database: Google Apps Script, Google Sheets.

**Author:** [សេន សៀកណាន់	បៀវ ឈុនលី    មុំ សិយ៉ារ៉ូ]
**Class:** ឆ្នាំទី ៣, ក្រុម B.EcE.A
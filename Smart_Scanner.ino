#include <WiFi.h>
#include <WiFiManager.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h> 
#include <SPI.h>  
#include <SD.h>   

// ================= ការកំណត់បណ្តាញ =================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "classroom/checkin";

// 🔴 Link របស់ Google Script 🔴
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycby39WkYdxnf64jL95NiO5_2fu4XbgwA_a2O63i4O_SzKRa5gC9aZqSvLsaFBFYZGxIcow/exec";

// ================= ការកំណត់ម៉ោង =================
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600; 
const int   daylightOffset_sec = 0;
bool showReadyScreen = true; 
unsigned long lastClockUpdate = 0;
unsigned long lastRedLedBlink = 0; 
bool wasWiFiConnected = true;

WiFiClient espClient;
PubSubClient client(espClient);

// ================= ការកំណត់ Hardware =================
const int greenLED = 25; 
const int redLED = 26;   
const int buzzerPin = 27; 
const int SD_CS_PIN = 5;  

#define mySerial Serial2 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= បញ្ជីឈ្មោះសិស្ស (Offline Mode) =================
const char* studentNames[] = {
  "Unknown",              // ID 0
  "01. KEO RAVIN",        // ID 1
  "02. DOS UDOM",         // ID 2
  "03. THAV SIHOUNG",     // ID 3
  "04. THEN PONLEU",      // ID 4
  "05. TOUCH DONA",       // ID 5
  "06. THAT PHEAREAK",    // ID 6
  "07. BEAV CHHUNLY",     // ID 7
  "08. PHAL SOPHORN",     // ID 8
  "09. MON DAVIT",        // ID 9
  "10. MEAS SOPOL",       // ID 10
  "11. MOM SEYAROW",      // ID 11
  "12. MET SONEAT",       // ID 12
  "13. LORN NOEN",        // ID 13
  "14. VICHET VATTENA",   // ID 14
  "15. SAN KIMSAY",       // ID 15
  "16. SOEURN RAVY",      // ID 16
  "17. SEN SEAKNANN",     // ID 17
  "18. SEM SINOUN",       // ID 18
  "19. SREAT LIMHENG",    // ID 19
  "20. HOEUM SIKHEN",     // ID 20
  "21. PO CHANDARA"       // ID 21
};
const int totalStudents = sizeof(studentNames) / sizeof(studentNames[0]);

// ================= 🎵 អនុគមន៍សំឡេង =================
void playSuccessMelody() {
  tone(buzzerPin, 988, 100); delay(100); 
  tone(buzzerPin, 1319, 200); delay(200); 
  noTone(buzzerPin);
}

void playErrorMelody() {
  tone(buzzerPin, 300, 300); delay(300);
  tone(buzzerPin, 200, 500); delay(500);
  noTone(buzzerPin);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(WHITE);
  display.setCursor(0,0); display.println("No WiFi Connection!");
  display.setCursor(0, 15); display.println("Please scan QR code");
  display.setCursor(0, 25); display.println("to setup new WiFi.");
  display.setTextSize(2); display.setCursor(5, 45); display.println("NPIC_Setup"); 
  display.display();
}

void updateClockScreen() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    display.clearDisplay(); display.setTextSize(1); display.setCursor(0,0); display.println("System Ready"); display.display();
    return;
  }
  display.clearDisplay();
  display.setTextSize(2); display.setCursor(15, 10);
  display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  display.setTextSize(1); display.setCursor(30, 35);
  display.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(20, 52); display.println("- Scan Finger -");
  } else {
    display.setCursor(15, 52); display.println("[ OFFLINE MODE ]");
  }
  display.display();
}

void setup_wifi() {
  display.clearDisplay(); display.setTextSize(1); display.setCursor(0,0); display.print("Connecting WiFi..."); display.display();
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  bool res = wm.autoConnect("NPIC_Setup", "12345678"); 
  if(!res) Serial.println("Failed to connect, entering offline mode...");
  else Serial.println("\nWiFi Connected!");
}

// ================= 🚀 អនុគមន៍បញ្ជូន និងឆែក Already Scanned (Update ថ្មី!) =================
void sendToGoogleSheets(int id) {
  WiFiClientSecure secureClient; secureClient.setInsecure(); 
  HTTPClient http;
  String urlWithParams = String(GOOGLE_SCRIPT_URL) + "?source=fingerprint&id=" + String(id);
  
  // បង្ហាញពាក្យ Syncing លើអេក្រង់
  display.fillRect(0, 50, 128, 14, BLACK); 
  display.setCursor(5, 54); display.println("Syncing to Cloud...");
  display.display();

  http.begin(secureClient, urlWithParams);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Sent ID: " + String(id) + " | Server: " + response);
    
    display.clearDisplay();
    // ១. ឆែកបើ Google ប្រាប់ថាធ្លាប់ស្កេនរួចហើយ
    if (response.indexOf("Already Scanned") >= 0) {
      display.setTextSize(2); display.setCursor(15, 5); display.println("WARNING!");
      display.setTextSize(1); display.setCursor(5, 30); display.println("Already Scanned");
      display.setCursor(5, 45); display.println("via NFC or Web App!");
      display.display();
      
      digitalWrite(redLED, HIGH);
      playErrorMelody(); // លោតសំឡេង Error
      digitalWrite(redLED, LOW);
      delay(3000); // បង្ហាញសារព្រមាន ៣ វិនាទី
    } 
    // ២. ឆែកបើ Google ប្រាប់ថាជោគជ័យ
    else if (response.indexOf("Success") >= 0) {
      display.setTextSize(2); display.setCursor(15, 10); display.println("SUCCESS!");
      display.setTextSize(1); display.setCursor(15, 40); display.println("Saved to Database");
      display.display();
      delay(1500);
    } 
    // ៣. បើមានបញ្ហាផ្សេងៗ
    else {
      display.setCursor(0, 10); display.println("Server Error:"); display.println(response); display.display();
      delay(2000);
    }
  } else {
    Serial.println("HTTP Error: " + String(httpResponseCode));
    display.clearDisplay(); display.setCursor(0, 20); display.println("Cloud Sync Failed!"); display.display();
    delay(2000);
  }
  http.end();
}

// ================= អនុគមន៍ Offline (SD Card) =================
void saveToSDCard(int id) {
  File dataFile = SD.open("/data.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.println(id); dataFile.close();
    Serial.println("Saved ID " + String(id) + " to SD Card.");
  } else Serial.println("Error opening /data.txt");
}

void syncOfflineData() {
  if (!SD.exists("/data.txt")) return; 
  
  File dataFile = SD.open("/data.txt");
  if (dataFile) {
    int totalLines = 0;
    while (dataFile.available()) { if(dataFile.read() == '\n') totalLines++; } 
    dataFile.seek(0); 
    
    int currentLine = 0;
    while (dataFile.available()) {
      String idStr = dataFile.readStringUntil('\n'); idStr.trim();
      if (idStr.length() > 0) {
        currentLine++;
        int savedID = idStr.toInt();
        
        display.clearDisplay(); display.setTextSize(1); display.setCursor(5, 5); display.println("Syncing SD Card...");
        display.setCursor(5, 20); display.printf("Uploading: %d / %d", currentLine, totalLines);
        display.drawRect(5, 40, 118, 15, WHITE); 
        int progressBarWidth = map(currentLine, 0, totalLines, 0, 114);
        display.fillRect(7, 42, progressBarWidth, 11, WHITE); 
        display.display();
        
        sendToGoogleSheets(savedID);
        delay(500); 
      }
    }
    dataFile.close();
    SD.remove("/data.txt"); 
    playSuccessMelody(); 
  }
}

// ================= អនុគមន៍ចុះឈ្មោះក្រយៅដៃ =================
void enrollNewFingerprint(int id, String newName) {
  showReadyScreen = false;
  display.clearDisplay(); display.setTextSize(1); display.setCursor(0,0); display.println("--- ENROLL MODE ---");
  display.setCursor(0, 15); display.print("ID: "); display.println(id); display.print("Name: "); display.println(newName);
  display.setTextSize(2); display.setCursor(0, 40); display.println("Place Finger"); display.display();

  int p = -1;
  unsigned long timeout = millis();
  while (p != FINGERPRINT_OK && millis() - timeout < 10000) { p = finger.getImage(); } 
  
  if (p != FINGERPRINT_OK) { 
    display.clearDisplay(); display.setCursor(10,20); display.println("Timeout!"); display.display();
    delay(2000); showReadyScreen = true; return;
  }
  p = finger.image2Tz(1);
  
  display.clearDisplay(); display.setCursor(0, 20); display.println("Remove Finger"); display.display();
  delay(2000);
  p = 0; while (p != FINGERPRINT_NOFINGER) { p = finger.getImage(); }

  display.clearDisplay(); display.setCursor(0, 20); display.println("Place Again"); display.display();
  
  p = -1; timeout = millis();
  while (p != FINGERPRINT_OK && millis() - timeout < 10000) { p = finger.getImage(); }
  p = finger.image2Tz(2);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
      display.clearDisplay(); display.setCursor(10, 20); display.println("Success!"); display.display();
      digitalWrite(greenLED, HIGH); playSuccessMelody(); digitalWrite(greenLED, LOW);
    }
  } else {
    display.clearDisplay(); display.setCursor(10, 20); display.println("Failed!"); display.display();
    digitalWrite(redLED, HIGH); playErrorMelody(); digitalWrite(redLED, LOW);
  }
  delay(2000); showReadyScreen = true;
}

// ================= អនុគមន៍ MQTT (Check-in, Enroll, Delete) =================
void callback(char* topic, byte* payload, unsigned int length) {
  showReadyScreen = false; 
  String messageTemp;
  for (int i = 0; i < length; i++) messageTemp += (char)payload[i];
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, messageTemp)) return;
  
  const char* cmd = doc["cmd"]; 
  
  if (String(cmd) == "checkin") {
    const char* studentName = doc["name"];
    display.clearDisplay();
    display.setTextSize(2); display.setCursor(0,0); display.println("Checked In");
    display.setTextSize(1); display.setCursor(0, 30); display.println(studentName);
    display.display();
    
    digitalWrite(greenLED, HIGH); playSuccessMelody(); digitalWrite(greenLED, LOW);
    delay(3000); showReadyScreen = true; updateClockScreen();
  } 
  else if (String(cmd) == "enroll") {
    int newID = doc["id"];
    const char* newName = doc["name"];
    enrollNewFingerprint(newID, String(newName));
  }
  else if (String(cmd) == "delete") {
    int idToDelete = doc["id"];
    display.clearDisplay(); display.setTextSize(2);
    
    uint8_t p = finger.deleteModel(idToDelete);
    if (p == FINGERPRINT_OK) {
      display.setCursor(10, 10); display.println("Deleted!");
      display.setCursor(10, 40); display.print("ID: "); display.print(idToDelete);
      display.display();
      digitalWrite(greenLED, HIGH); playSuccessMelody(); digitalWrite(greenLED, LOW);
    } else {
      display.setCursor(10, 10); display.println("Del Fail!");
      display.setCursor(10, 40); display.print("ID: "); display.print(idToDelete);
      display.display();
      digitalWrite(redLED, HIGH); playErrorMelody(); digitalWrite(redLED, LOW);
    }
    delay(2500); 
    showReadyScreen = true; updateClockScreen();
  }
}

void reconnect() {
  if (WiFi.status() != WL_CONNECTED) return;
  while (!client.connected()) {
    String clientId = "ESP32Scanner-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) { client.subscribe(mqtt_topic); } 
    else { delay(5000); }
  }
}

// ================= Setup =================
void setup() {
  Serial.begin(115200); delay(1000); 
  pinMode(greenLED, OUTPUT); pinMode(redLED, OUTPUT); pinMode(buzzerPin, OUTPUT);
  Wire.begin(21, 22);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println(F("OLED failed")); for(;;); }
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(WHITE); display.setCursor(0,0); display.println("System Starting..."); display.display();
  
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Error!"); display.setCursor(0, 15); display.println("SD Error!"); display.display(); delay(2000);
  }

  setup_wifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  client.setServer(mqtt_server, mqtt_port); client.setCallback(callback);

  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);
  if (finger.verifyPassword()) Serial.println("Found sensor!");
  
  updateClockScreen();
}

// ================= Loop =================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (wasWiFiConnected) {
      wasWiFiConnected = false; digitalWrite(redLED, LOW); updateClockScreen();
    }
    unsigned long currentMillis = millis();
    if (currentMillis - lastRedLedBlink >= 10000) { lastRedLedBlink = currentMillis; digitalWrite(redLED, HIGH); }
    if (currentMillis - lastRedLedBlink >= 200) { digitalWrite(redLED, LOW); }
  } else {
    if (!wasWiFiConnected) {
      wasWiFiConnected = true; digitalWrite(redLED, LOW);
      syncOfflineData(); updateClockScreen();       
    }
    if (!client.connected()) reconnect();
    client.loop();
  }

  if (showReadyScreen && (millis() - lastClockUpdate > 1000)) {
    updateClockScreen(); lastClockUpdate = millis();
  }

  getFingerprintID();
  delay(50);
}

// ================= អនុគមន៍ស្កេនម្រាមដៃ (Update!) =================
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return p; 
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return p;
  p = finger.fingerSearch();
  
  if (p == FINGERPRINT_OK) {
    showReadyScreen = false; 
    String currentName = "Unknown ID";
    if (finger.fingerID < totalStudents) currentName = studentNames[finger.fingerID]; 

    // បង្ហាញពាក្យ Welcome ពេលផ្តិតត្រូវភ្លាមៗ
    display.clearDisplay(); display.setTextSize(2); display.setCursor(15,0); display.println("Welcome!");
    display.setTextSize(1); display.setCursor(0, 30); display.print("Name: "); display.println(currentName); 
    display.setCursor(0, 45); display.print("ID  : "); display.println(finger.fingerID); display.display();
    
    digitalWrite(greenLED, HIGH); playSuccessMelody(); digitalWrite(greenLED, LOW);
    
    // ចាប់ផ្តើមបញ្ជូនទិន្នន័យ (OLED នឹងដូរតាមចម្លើយ Google)
    if (WiFi.status() == WL_CONNECTED) { 
      sendToGoogleSheets(finger.fingerID); 
    } else { 
      saveToSDCard(finger.fingerID); delay(100); tone(buzzerPin, 800, 100); 
      delay(2000); // ទុកអេក្រង់ឱ្យមើលបន្តិចពេល Offline
    } 

    showReadyScreen = true; updateClockScreen();
    
  } else if (p == FINGERPRINT_NOTFOUND) {
    showReadyScreen = false;
    display.clearDisplay(); display.setTextSize(2); display.setCursor(10,20); display.println("Unknown!"); display.display();
    
    digitalWrite(redLED, HIGH); playErrorMelody(); digitalWrite(redLED, LOW);
    
    delay(1500); 
    showReadyScreen = true; updateClockScreen();
  }
  return finger.fingerID;
}
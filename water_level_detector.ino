#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // For HTTPS POST

#include "secrets.h"

#define BLYNK_TEMPLATE_ID BLYNK_TEMP_ID
#define BLYNK_TEMPLATE_NAME "Water Monitor"
#define BLYNK_AUTH_TOKEN BLYNK_AUTH
#define CF_WORKER_URL CF_WORKER_ENDPOINT
#define WORKER_AUTH CF_API_KEY

#define TRIG_PIN D5
#define ECHO_PIN D6
#define LED_PIN LED_BUILTIN

// --- Battery pins ---
#define BATTERY_PIN A0
#define CHARGING_PIN D7
#define FULL_PIN D8

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>


char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BlynkTimer timer;

// --- Tank calibration (in cm) ---
#define DMAX 117   // Distance when tank is empty (sensor → bottom)
#define DMIN 25    // Distance when tank is full (sensor → water)
float lowThreshold = 25.0;  // Below this % → motor ON alert
float fullThreshold = 90.0; // Above this % → motor OFF alert

// --- State tracking ---
bool lowNotified = false;
bool fullNotified = false;

bool otaMode = false; // Controlled via Blynk switch (V11)

WiFiClientSecure secureClient;

struct WorkerData {
  float distance;
  float level;
  String logMessage;
  bool pending;
} workerData = {0, 0, "", false};

// --- Ultrasonic Reading ---
float readDistance() {
  // Temporarily disable WiFi interrupts to prevent timing distortion
  noInterrupts();

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(30);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 500000); // 200ms timeout

  interrupts(); // Re-enable after reading
  
  if (duration == 0) {
    workerData = {-1, -1, "Ultrasonic sensor timeout - no echo received.", true};
    
    return -1; // indicates failure
  }

  float distance = duration * 0.034 / 2;          // Convert to cm
  return distance;
}

// Function to turn off LED
void turnOffLED() {
  digitalWrite(LED_PIN, HIGH); // OFF (built-in LED is active LOW)
}

// Button press from app
BLYNK_WRITE(V2) {
  int buttonState = param.asInt();
  if (buttonState) {
    digitalWrite(LED_PIN, LOW);  // LED ON
    timer.setTimeout(2000L, turnOffLED);   // Turn OFF after 2 seconds
  }
}

// --- Send data to Blynk ---
void sendDistance() {
  float distance = readDistance();

  if (distance < 0) {
    workerData = {-1, -1, "Sensor timeout", true};
    return;
  }

  // Calculate level %
  float level = ((DMAX - distance) / (DMAX - DMIN)) * 100.0;
  if (level < 0) level = 0;
  if (level > 100) level = 100;

  String debugMeasureMsg = "Distance: " + String(distance) + " cm | Water Level: " + String(level) + " %";

  // Send to Blynk
  Blynk.virtualWrite(V3, level);     // Water Level Gauge
  Blynk.virtualWrite(V4, distance);  // Distance Display (cm)

  workerData = {distance, level, "Distance update", true};

  // --- Notifications ---
  if (level <= lowThreshold && !lowNotified) {
    Blynk.logEvent("low_water", "Tank level is low. Please turn ON the motor.");
    workerData = {distance, level, "Low water alert", true};
    lowNotified = true;
    fullNotified = false;
  }

  if (level >= fullThreshold && !fullNotified) {
    Blynk.logEvent("tank_full", "Tank is full. Please turn OFF the motor.");
    workerData = {distance, level, "Tank full alert", true};
    fullNotified = true;
    lowNotified = false;
  }
}

BLYNK_WRITE(V10) {
  int state = param.asInt();
  if (state == 1) {
    workerData = {-1, -1, "Manual refresh triggered from Blynk", true};
    sendDistance(); // Call your function that measures and sends data
    Blynk.virtualWrite(V10, 0); // Reset the button back to OFF
  }
}

BLYNK_WRITE(V11) {
  otaMode = param.asInt(); // 1 = Stay awake, 0 = Allow sleep
  if (otaMode) {
      workerData = {-1, -1, "OTA Mode Enabled - Deep Sleep Disabled.", true};
  } else {
      workerData = {-1, -1, "OTA Mode Disabled - Device will sleep after update.", true};
  }
}

// --- OTA Setup ---
void setupOTA() {
  ArduinoOTA.setHostname("SmartWaterMonitor-ESP8266");
  ArduinoOTA.setPassword("WaterMonitor@2025");

  ArduinoOTA.onStart([]() {
    workerData = {-1, -1, "OTA update started", true};
    Blynk.logEvent("ota_start", "OTA update started for Smart Water Monitor.");
  });

  ArduinoOTA.onEnd([]() {
    workerData = {-1, -1, "OTA update completed", true};
    Blynk.logEvent("ota_complete", "OTA update completed successfully. Device is restarting.");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error [%u]\n", error);
    workerData = {-1, -1, "OTA error code: " + String(error), true};
  });

  ArduinoOTA.begin();
}

void processWorkerQueue() {
  if (!workerData.pending || WiFi.status() != WL_CONNECTED) return;

  Blynk.virtualWrite(V9, workerData.logMessage);

  String safeLog = jsonEscape(workerData.logMessage);

  // Build JSON
  String payload = "{";
  payload += "\"distance\":"    + String(workerData.distance, 2) + ",";
  payload += "\"water_level\":" + String(workerData.level, 2) + ",";
  payload += "\"log\":\""       + safeLog + "\",";
  payload += "\"timestamp\":"   + String(millis() / 1000);
  payload += "}";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setTimeout(7000);
  http.setReuse(false);                   // avoid weird keep-alive edge cases
  // http.useHTTP10();                    // uncomment if you still see issues

  if (!http.begin(client, CF_WORKER_URL)) {
    Serial.println("Worker begin() failed");
    return; // keep pending, will retry
  }

  // Match Postman-ish headers
  http.addHeader("Content-Type", "application/json");       // Postman has this
  http.addHeader("User-Agent", "PostmanRuntime/7.43.0");    // Safe UA
  http.addHeader("Accept", "*/*");                          // Postman sets this
  http.addHeader("Connection", "keep-alive");               // Postman sets this
  http.addHeader("Accept-Encoding", "identity");            // avoid gzip on response
  http.addHeader("Authorization", WORKER_AUTH);

  Serial.println("[Worker] Sending: " + payload);

  int code = http.POST(payload);
  String resp = http.getString();
  http.end();

  Serial.println("Worker POST → " + String(code));
  if (resp.length()) Serial.println("Worker RESP: " + resp);

  if (code >= 200 && code < 300) {
    workerData = {0, 0, "", false}; // clear on success
  } else {
    Serial.println("Worker upload failed, keeping pending to retry...");
    // keep pending=true so it retries later
  }
}


// Optional: tiny helper to JSON-escape quotes/newlines
String jsonEscape(const String& s) {
  String out; out.reserve(s.length()+8);
  for (size_t i=0;i<s.length();++i) {
    char c=s[i];
    if (c=='"') out += "\\\"";
    else if (c=='\\') out += "\\\\";
    else if (c=='\n') out += "\\n";
    else if (c=='\r') out += "\\r";
    else out += c;
  }
  return out;
}

// Deep Sleep Logic - call this inside loop()
void handleDeepSleep() {
  static unsigned long startMillis = millis();

  if (otaMode) {
    // OTA Mode active → Don't sleep
    // Optional: Reset timer so sleep doesn't trigger immediately when OTA is turned off later
    workerData = {-1, -1, "OTA Mode Enabled - Deep Sleep Disabled", true};

    startMillis = millis();
    return;
  }

  // If 60 seconds have passed and OTA mode is NOT active → go to sleep
  if (millis() - startMillis >= 60000) {  // 60 sec delay before sleep
    workerData = {-1, -1, "Device entering Deep Sleep...", true};

    delay(200);  // Allow logs/Blynk messages to send before sleep

    ESP.deepSleep(60e6, WAKE_RF_DEFAULT);   // Sleep for 60 seconds (test value)
    // ESP.deepSleep(4 * 60 * 60e6);  // 4 hours in real use
  }
}

void connectWiFiAndBlynk() {
  Serial.println("\n[Boot] Waking and connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.forceSleepWake();       // ensure RF is awake after deep sleep
  delay(100);
  WiFi.begin(ssid, pass);

  // Wait up to 15s for WiFi
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - t0) < 15000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi FAILED");
  }

  // Configure Blynk without managing WiFi (non-blocking style)
  Blynk.config(BLYNK_AUTH_TOKEN);
  // Try to connect to Blynk up to 10s
  if (Blynk.connect(10000)) {
    Serial.println("Blynk connected");
  } else {
    Serial.println("Blynk connect FAILED");
  }
}

void setup() {

  // ----- Serial (Debug enabled early) -----
  Serial.begin(115200);
  delay(100);
  Serial.println("\nBooting...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(CHARGING_PIN, INPUT_PULLUP);
  pinMode(FULL_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, HIGH); // OFF (active low LED)

  connectWiFiAndBlynk();

  setupOTA(); // Initialize OTA updates

  timer.setInterval(60000L, sendDistance); // For OTA testing (every 1 min)
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
  processWorkerQueue();
}
/******************************************************************************************
 * Project Name : Smart Gas Leakage Detection and Alert System
 * Author       : Abhay Kushwaha
 * Board        : ESP32 Dev Module
 * IDE          : Arduino IDE
 * Language     : C++
 *
 * Description:
 * This project detects combustible gases using the MQ2 Gas Sensor.
 * The ESP32 continuously monitors the gas concentration and indicates
 * the current status using LEDs and a buzzer.
 *
 * Features:
 * ✔ Green LED - Safe Status
 * ✔ Yellow LED - Warning Level
 * ✔ Red LED - Danger Level
 * ✔ Buzzer Alarm
 * ✔ WiFi Connectivity
 * ✔ Blynk IoT Monitoring
 * ✔ Real-Time Gas Value Display
 *
 * Hardware:
 * • ESP32 Dev Board
 * • MQ2 Gas Sensor
 * • Green LED
 * • Yellow LED
 * • Red LED
 * • Active Buzzer
 * • Breadboard
 * • Jumper Wires
 *
 * Connections:
 * MQ2 AO    -> GPIO34
 * Green LED -> GPIO14
 * Yellow LED-> GPIO25
 * Red LED   -> GPIO27
 * Buzzer    -> GPIO26
 *
 * Blynk:
 * V0 -> Gas Value
 * V1 -> Alarm LED
 * V2 -> Manual Buzzer
 *
 * License:
 * MIT License
 ******************************************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3K9A5kTA3"
#define BLYNK_TEMPLATE_NAME "Gas Detector"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// =================== Pin Configuration ===================

#define MQ2_PIN      34
#define GREEN_LED    14
#define YELLOW_LED   25
#define RED_LED      27
#define BUZZER_PIN   26

// =================== Threshold Levels ====================

int warningLevel = 2700;
int gasThreshold = 3100;

BlynkTimer timer;

// ==========================================================

void sendData() {

  // Average 20 readings for better stability

  long total = 0;

  for (int i = 0; i < 20; i++) {
    total += analogRead(MQ2_PIN);
    delay(5);
  }

  int gasValue = total / 20;

  Serial.print("Gas Value : ");
  Serial.println(gasValue);

  Blynk.virtualWrite(V0, gasValue);

  // ---------------- SAFE ----------------

  if (gasValue < warningLevel) {

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Blynk.virtualWrite(V1, 0);

  }

  // ---------------- WARNING ----------------

  else if (gasValue < gasThreshold) {

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Blynk.virtualWrite(V1, 0);

  }

  // ---------------- DANGER ----------------

  else {

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    digitalWrite(BUZZER_PIN, HIGH);
    delay(250);
    digitalWrite(BUZZER_PIN, LOW);
    delay(250);

    Blynk.virtualWrite(V1, 255);

    Blynk.logEvent("gas_alert", "⚠️ Gas Leakage Detected!");

  }
}

// ================= Manual Buzzer =================

BLYNK_WRITE(V2) {

  int state = param.asInt();
  digitalWrite(BUZZER_PIN, state);

}

// ================= Setup =================

void setup() {

  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(GREEN_LED, HIGH);

  WiFi.setSleep(false);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(500);

  }

  Serial.println();
  Serial.println("WiFi Connected");

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  timer.setInterval(2000L, sendData);

}

// ================= Loop =================

void loop() {

  Blynk.run();
  timer.run();

}

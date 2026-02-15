#include <stdio.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ---- CHANGE THESE ----
const char* ssid = "alaynas iPhone";
const char* password = "abcdefghij";

// ---- PINS ----
const int LED_PIN = 2;      // Built‑in LED on many ESP32 boards
const int BUTTON_PIN = 14;  // Change to whatever pin your button uses

WebServer server(80);

// ---- WEBPAGE ----
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Test</title>
</head>
<body>
  <h1>ESP32 is working!</h1>
  <p>If you can see this page, the web server works.</p>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ---- LED ----
  pinMode(LED_PIN, OUTPUT);

  // ---- BUTTON ----
  //pinMode(BUTTON_PIN, INPUT_PULLUP);

  // ---- WIFI ----
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // ---- WEB SERVER ----
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();

  // ---- LED BLINK TEST ----
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  /** 
  // ---- BUTTON TEST ----
  if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
    Serial.println("Button pressed!");
    delay(300); // Debounce
  }
    */
}



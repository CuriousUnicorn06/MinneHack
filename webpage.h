#include <WiFi.h>
#include <WebServer.h>

// Replace with your Wi-Fi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Pins
const int LED_PIN = 2;      // Built-in LED or external
const int BUTTON_PIN = 14;  // Change to your button pin

// State
String habitName = "Default Habit";
bool personA_done = false;
bool personB_done = false;
bool buttonPressed = false;

WebServer server(80);

// HTML page
const char MAIN_page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>HabitLink Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background: #f4f4f4; padding: 20px; text-align: center; }
    h1 { color: #333; }
    .status { font-size: 1.2em; margin: 10px 0; }
    .done { color: green; }
    .notdone { color: red; }
    input, button { padding: 10px; font-size: 1em; margin: 5px; }
  </style>
</head>
<body>
  <h1>🌱 HabitLink Dashboard</h1>
  <div>
    <input id="habitInput" placeholder="Enter habit name">
    <button onclick="setHabit()">Save Habit</button>
  </div>
  <h2 id="habitTitle">Habit: Loading...</h2>
  <div class="status" id="aStatus">Person A: ...</div>
  <div class="status" id="bStatus">Person B: ...</div>
  <script>
    async function updateStatus() {
      try {
        const res = await fetch('/status');
        const data = await res.json();
        document.getElementById('habitTitle').innerText = "Habit: " + data.habit;
        document.getElementById('aStatus').innerHTML = 
          "Person A: <span class='" + (data.personA_done ? "done" : "notdone") + "'>" +
          (data.personA_done ? "✔ Done" : "✘ Not done") + "</span>";
        document.getElementById('bStatus').innerHTML = 
          "Person B: <span class='" + (data.personB_done ? "done" : "notdone") + "'>" +
          (data.personB_done ? "✔ Done" : "✘ Not done") + "</span>";
      } catch (err) {
        console.error("Failed to fetch status:", err);
      }
    }
    async function setHabit() {
      const habit = document.getElementById('habitInput').value;
      await fetch('/set-habit', {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: habit
      });
      updateStatus();
    }
    setInterval(updateStatus, 1000);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";

// Handlers
void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleStatus() {
  String json = "{";
  json += "\"habit\":\"" + habitName + "\",";
  json += "\"personA_done\":" + String(personA_done ? "true" : "false") + ",";
  json += "\"personB_done\":" + String(personB_done ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetHabit() {
  habitName = server.arg("plain");
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set-habit", HTTP_POST, handleSetHabit);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  // Button logic
  if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed) {
    buttonPressed = true;
    personA_done = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Person A completed habit.");
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    buttonPressed = false;
  }
}
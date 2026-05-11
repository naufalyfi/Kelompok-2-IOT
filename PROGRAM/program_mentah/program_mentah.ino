#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

String serverName = "http://192.168.1.100:3000/data"; // ganti IP server kamu

int sensorPin = 34;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
}

void loop() {
  int soundValue = analogRead(sensorPin);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"sound\":" + String(soundValue) + "}";

    int httpResponseCode = http.POST(jsonData);

    Serial.print("Sound: ");
    Serial.print(soundValue);
    Serial.print(" | Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }

  delay(3000); // kirim tiap 3 detik
}
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
const char* ssid = "WIFI-UNP";
const char* password = "";

void setup () {
  Serial.begin (115200);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    }
  Serial.println ("");
  Serial.println ("WiFi terhubung.");
  Serial.println ("Alamat IP:");
  Serial.println (WiFi.localIP ());
  }

void loop () {}

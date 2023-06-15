#define led 2
#include <WiFi.h>
#include <FirebaseESP32.h>
//1. Change the following info
#define ssid "WIFI-UNP"
#define password ""
#define FIREBASE_HOST "https://ledproject2-54d59-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "44BgC6rBZerQjbeQY0ywV46UxesMfu0xukMspETC"
FirebaseData fbdo;
void setup()
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  
  Serial.begin(9600);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    }
  Serial.println ("");
  Serial.println ("WiFi terhubung.");
  Serial.println ("Alamat IP:");
  Serial.println (WiFi.localIP ());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}
void loop()
{
  if(Firebase.getString(fbdo, "/LED1"))
  {
    Serial.print("Get int data A success, str = ");
    Serial.println(fbdo.stringData());
    digitalWrite(led, fbdo.stringData().toInt());
  }else{
    Serial.print("Error in getString, ");
    Serial.println(fbdo.errorReason());
  }
}

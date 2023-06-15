#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "https://ledproject2-54d59-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "44BgC6rBZerQjbeQY0ywV46UxesMfu0xukMspETC"
#define ssid "WIFI-UNP"
#define password ""

unsigned long interval = 1000; // the time we need to wait
unsigned long previousMillis = 0; // millis() returns an unsigned long.

//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;
const int led = 2;
bool statusled = 0;

void setup()
{

  Serial.begin(115200);
  pinMode(led, OUTPUT);

  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println ("");
  Serial.println ("WiFi terhubung.");
  Serial.println ("Alamat IP:");
  Serial.println (WiFi.localIP ());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  Serial.println("------------------------------------");
  Serial.println("Connected...");

}

void loop()
{
  light();
  delay(100);
  json.set("/data", statusled);
  Firebase.updateNode(firebaseData, "/Sensor", json);
}

void light() {
  unsigned long currentMillis = millis(); // grab current time
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = millis();
    statusled = !statusled; // "toggles" the state
    digitalWrite(led, statusled); // sets the LED based on ledState
    Serial.println(statusled);
  }
}

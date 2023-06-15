//Memanggil Library
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include <FlowMeter.h>

//Memasukkan URL dan Token Firebase
#define FIREBASE_HOST "https://monitoringwater-29743-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "CurT2qYC9lC8dzIdjgvyjWi0q989mrocvVKAKnnG"

//Memasukkan Usernamae & PW wifi
#define ssid "WIFI-UNP"
#define password ""

//Deklarasi Waktu
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;

LiquidCrystal_I2C lcd(0x27, 16, 2);
FirebaseData firebaseData;
FlowMeter *Meter;
FlowMeter *Meter2;

//Deklarasi Variabel dan pin ESP-32
const unsigned long period = 500;
const int relay = 2;
String sistem;
//float deb, vol, deb2, vol2;
//unsigned int pay, paytot, pay2, paytot2;
float deb = 0; float vol = 0; float deb2 = 0; float vol2 = 0;
unsigned int pay = 0; unsigned int paytot = 0; unsigned int pay2 = 0; unsigned int paytot2 = 0;

//Fungsi untuk menghitung pulsa
void hitungpulsa() {
  Meter->count();
  Meter2->count();
}

void setup() {
  Serial.begin (115200); //set kecepatan pembacaan data pada 115200 bit per sekon
  pinMode (relay, OUTPUT);  //menyatakan relay sebagai ouput
  //memulai LCD
  lcd.init();
  lcd.backlight();

  //Mulai koneksi wifi
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println ("");
  Serial.println ("WiFi terhubung.");
  Serial.println ("Alamat IP:");
  Serial.println (WiFi.localIP ());

  //konfigurasi waktu
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //Memulai firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Deklarasi sensor water flow pada pin 15 dan pin 18 ESP-32 sebagai Interupt
  Meter = new FlowMeter(digitalPinToInterrupt(15), UncalibratedSensor, hitungpulsa, RISING);
  Meter2 = new FlowMeter(digitalPinToInterrupt(18), UncalibratedSensor, hitungpulsa, RISING);
}

void loop() {
  delay (period);
  //Menampilkan fungsi waktu
  printLocalTime();

  //Deklarasi rumus untuk menghitung debit, volume dan nilai bayar air
  deb = Meter->getCurrentFlowrate();
  vol = Meter->getTotalVolume();
  pay = (deb * 190);
  paytot = (vol * 190);
  deb2 = Meter2->getCurrentFlowrate();
  vol2 = Meter2->getTotalVolume();
  pay2 = (deb2 * 190);
  paytot2 = (vol2 * 0.190);
  Meter->tick(period);
  Meter2->tick(period);

  // Menampilkan data Debit, Volume dan Nilai Bayar di Serial Monitor
  Serial.println("Debit Rumah 1:" + String(deb) + " l/min, ");
  Serial.println("Bayar Per Debit Rumah 1: " + String(pay));
  Serial.println("Debit Rumah 2 " + String(deb2) + " l/min, ");
  Serial.println("Bayar Per Debit Rumah 2: " + String(pay2));
  Serial.println("Total Volume Rumah 1:" + String(vol) + " l total.");
  Serial.println("Bayar Per Volume Rumah 1: " + String(paytot));
  Serial.println("Total Volume Rumah 2:" + String(vol2) + " l total.");
  Serial.println("Bayar Per Volume Rumah 2: " + String(paytot2));

  // menampilkan karakter di LCD
  lcd.setCursor(0, 0);
  lcd.print("V="); lcd.print(vol); lcd.print("l");
  lcd.setCursor(0, 1);
  lcd.print("P="); lcd.print(pay);
  lcd.setCursor(8, 0);
  lcd.print("V="); lcd.print(vol2); lcd.print("l");
  lcd.setCursor(8, 1);
  lcd.print("p="); lcd.print(pay2);

  //Mengirim data Debit, VOlume dan Nilai Bayar ke Firebase
  Firebase.setString(firebaseData, "/WATER_MONITORING/Bayar Per Volume Rumah 1", paytot);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Bayar Per Debit Rumah 1", pay);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Volume Air Rumah 1", vol);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Debit Air Rumah 1", deb);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Bayar Per Volume Rumah 2", paytot2);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Bayar Per Debit Rumah 2", pay2);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Volume Air Rumah 2", vol2);
  Firebase.setString(firebaseData, "/WATER_MONITORING/Debit Air Rumah 2", deb2);

  //Mengambil data Sistem dari Firebase
  Firebase.getString(firebaseData, "/WATER_MONITORING/Sistem");
  sistem = firebaseData.stringData();
  Serial.println(sistem);

  //Fungsi Kontrol Sistem dari Android
  if (sistem == "1") {
    digitalWrite(relay, LOW);
    Serial.println("Alat Hidup");
  }
  else if (sistem == "0") {
    digitalWrite(relay, HIGH);
    Serial.println("Alat Mati");
  }
}

//Fungsi Waktu
void printLocalTime()
{
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println(&timeinfo, "%A, %d %B %Y %H:%M:%S");
    lcd.setCursor(0, 0);
  }
}

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "Tukang bagi hostpot"
#define WIFI_PASSWORD "password1"
#define DHTPIN D5
#define DHTTYPE DHT11
#define FIREBASE_HOST "sig-1-50fd3.firebaseio.com"
#define FIREBASE_AUTH "Wh7GASqAR0CU4cfREHUfSnNpUaVHe0T79JiUcymc"

WiFiServer server(80);
TinyGPSPlus gps;
SoftwareSerial ss(4, 5);
DHT dht(DHTPIN, DHTTYPE);

float latitude, longitude, kelembaban, suhu;
String lat_str, lng_str, desa;

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Wifi tidak tersambung!\n");
  }
  Serial.println("\nWifi tersambung");
  server.begin();
  Serial.print("\nServer berjalan di ");
  Serial.println(WiFi.localIP());
  dht.begin();
  pinMode(5, INPUT_PULLUP);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  while(ss.available() > 0) {
    if(gps.encode(ss.read())) {
      if(gps.location.isValid()) {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        Firebase.setString("/DHT11/1/latitude", lat_str);
        Firebase.setString("/DHT11/1/longitude", lng_str);
    
        Serial.print("titik latitude adalah ");
        Serial.println(lat_str);
        Serial.print("titik longitude adalah ");
        Serial.println(lng_str);
      }
    }
  }
  
  kelembaban = dht.readHumidity();
  suhu = dht.readTemperature();
  desa = "Cikereteg";

//  if(isnan(kelembaban) || isnan(suhu)) {
//    Serial.println("Suhu dan kelembaban tidak terdeteksi");
//    return;
//  } else {
    Firebase.setString("/DHT11/1/desa", desa);
    Firebase.setFloat("/DHT11/1/kelembaban", kelembaban);
    Firebase.setFloat("/DHT11/1/suhu", suhu);
  
    Serial.print(F("kelembaban Desa "));
    Serial.print(desa);
    Serial.print(" adalah ");
    Serial.print(kelembaban);
    Serial.println("%");
    Serial.print(F("suhu Desa "));
    Serial.print(desa);
    Serial.print(" adalah ");
    Serial.print(suhu);
    Serial.println("°C");
//  }

  WiFiClient client = server.available();
  if(!client) {
    return;
  }
  delay(2000);
}

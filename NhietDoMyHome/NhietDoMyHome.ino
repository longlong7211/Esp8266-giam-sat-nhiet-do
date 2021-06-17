#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define FIREBASE_HOST "nhietdophong-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "3tMXPQ1EKymoib1jNLIshdVLBTrzx2OkP76UGdBw"
#define WIFI_SSID "MaxSpeed"
#define WIFI_PASSWORD "5181917890"
FirebaseData firebaseData;

float h, hO = 0;
float t, tO = 0;
float hic;
unsigned long timer = millis();

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println("Khởi động cảm biến nhiệt độ");
  dht.begin();
}


void loop() {
  if ((unsigned long)(millis() - timer) > 1000) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    hic = dht.computeHeatIndex(t, h, false);
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print(F("Độ ẩm: "));
    Serial.print(h);
    Serial.print(F("%  Nhiệt độ: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F(" Cảm giác thực: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.println();
    timer = millis();
  }

  // đẩy giá trị lên firebase
  if (abs(t - tO) > 0.1 || abs(h - hO) > 0.5) {
    setDataMyHome(h, t, hic);
    hO = h;
    tO = t;
  }
}

void setDataMyHome(float h, float t, float hic) {
  if (Firebase.setFloat(firebaseData, "/H", h))
  {
    Serial.println("PASSED H");
    Serial.print("VALUE: ");
    Serial.print(firebaseData.floatData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/T", t))
  {
    Serial.println("PASSED T");
    Serial.print("VALUE: ");
    Serial.print(firebaseData.floatData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setFloat(firebaseData, "/HIC", hic))
  {
    Serial.println("PASSED HIC");
    Serial.print("VALUE: ");
    Serial.print(firebaseData.floatData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

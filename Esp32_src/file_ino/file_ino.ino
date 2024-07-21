#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "PT.DMH";
const char* password = "ptdmh2022";

const char* serverAddress = "http://192.168.1.181:5000/submit"; // Ganti dengan IP server yang benar

#define DHTPIN 4
#define DHTTYPE DHT11
#define FANPIN 5 // Pin untuk mengontrol kipas, ganti dengan pin yang sesuai

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(FANPIN, OUTPUT); // Set pin kipas sebagai output
  digitalWrite(FANPIN, LOW); // Memastikan kipas mati saat mulai

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity(); // Baca kelembaban
  int fan = 1;

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Logika kontrol kipas
  //if (temperature > 20.0) {
    digitalWrite(FANPIN, HIGH); // Menyalakan kipas
    fan = 1;
  //} else {
    //digitalWrite(FANPIN, LOW); // Mematikan kipas
    //fan = 0;
  //}

  HTTPClient http;
  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"temp\":" + String(temperature) + ", \"hum\":" + String(humidity) + ", \"fan\":" + String(fan) + "}";
  Serial.println("Sending data: " + jsonData);

  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);

    // Kalau error code 405 (Method Not Allowed), tampilkan metode yang diizinkan
    if (httpResponseCode == 405) {
      String allowedMethods = http.header("Allow");
      Serial.print("Allowed methods: ");
      Serial.println(allowedMethods);
    }
  }

  http.end();

  //delay(10000); // Tunggu 10 detik sebelum membaca lagi
  delay(300000); // 5 menit
}
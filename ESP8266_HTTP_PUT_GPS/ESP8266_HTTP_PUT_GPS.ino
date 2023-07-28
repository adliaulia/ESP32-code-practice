// LIBRARY
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// WIFI
const char* ssid = "proxmox.adliaulia.my.id";
const char* password = "P@ssw0rd";
String adli = "", aulia = "";
//Your Domain name with URL path or IP address with path
//String serverName = "http://192.168.10.101:3000/esp32";
String serverName = "http://103.150.196.45:12326/esp32";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 3000;


// GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// Choose two Arduino pins to use for software serial
int RXPin = D3;
int TXPin = D4;
int GPSBaud = 9600;
// Create a TinyGPS++ object
TinyGPSPlus gps;
// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(TXPin, RXPin);

String tahun = "", bulan = "", hari = "", jam = "", menit = "", detik = "";
String serverPath = "";
int validState = 0;
String nama_lengkap = "GPS NEO 6M V2";
String jenis_kelamin= "ESP8266";

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  // Start the software serial port at the GPS's default baud
  Serial.println("GPS Begin");
  gpsSerial.begin(GPSBaud);
}

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      //GPS
      while (gpsSerial.available() > 0)
      if (gps.encode(gpsSerial.read()))
      
      displayInfo();

      // If 5000 milliseconds pass and there are no characters coming in
      // over the software serial port, show a "No GPS detected" error
      if (millis() > 5000 && gps.charsProcessed() < 10) {
        Serial.println("No GPS detected");
        // exit loop
        while(true);
        }
        
      WiFiClient client;
      HTTPClient http;
      serverPath = serverName + "/64afe0a5e256526f13b8bdae";
      if (validState == 1){ 
         serverPath = serverName + "/64b0c25ff4ba7a5b735c09b7";
      }
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      http.addHeader("Content-Type", "application/json");

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Data dari sensor
      if (gps.date.day() < 10){ hari = "0" + String(gps.date.day()); } else { bulan = gps.date.day(); }
      if (gps.date.month() < 10){ bulan = "0" + String(gps.date.month()); } else { bulan = gps.date.month(); }
      if (gps.time.hour() < 10){ jam = "0" + String(gps.time.hour()); } else { jam = gps.time.hour(); }
      if (gps.time.minute() < 10){ menit = "0" + String(gps.time.minute()); } else { menit = gps.time.minute(); }
      if (gps.time.second() < 10){ detik = "0" + String(gps.time.second()); } else { detik = gps.time.minute(); }
      
      String tanggal_lahir= String(gps.date.year()) + "-" + String(bulan) + "-" + String(hari) + "T" + String(jam) + ":" + String(menit) + ":" + String(detik) + "Z";
      Serial.println(tanggal_lahir);
      String tempat_lahir = "Latitude, Longitude: " + String(gps.location.lat()) + ", " + String(gps.location.lng());
      String alamat = "Date, Time: " + String(gps.date.month()) + "/" + String(gps.date.day()) + "/" + String(gps.date.year());

      // Send HTTP PUT request
      int httpResponseCode = http.PUT("{\"nama_lengkap\": \"" + nama_lengkap + "\", \"jenis_kelamin\": \"" + jenis_kelamin + "\", \"tanggal_lahir\": \"" + tanggal_lahir + "\", \"tempat_lahir\": \"" + tempat_lahir + "\", \"alamat\": \"" + alamat + "\"}");
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void displayInfo(){
  Serial.println("Number of sattelites: " + String(gps.satellites.value())); 
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    validState = 1;
  } else {
    Serial.println("Location: Not Available");
    validState = 0;
  }

  Serial.print("Date: ");
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  } else {
    Serial.println("Not Available");
  }
  Serial.print("Time: ");
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  } else { 
    Serial.println("Not Available");
  }
  Serial.println();
  Serial.println();
  delay(100);
}
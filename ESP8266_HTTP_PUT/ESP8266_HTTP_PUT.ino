#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "proxmox.adliaulia.my.id";
const char* password = "P@ssw0rd";

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.10.101:3000/esp32";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

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
}

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "/64afe0a5e256526f13b8bdae";
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      http.addHeader("Content-Type", "application/json");

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Data dari sensor
      String nama_lengkap = "ESP";
      String jenis_kelamin= "E";
      String tanggal_lahir= "01-01-2023";
      String tempat_lahir = "Teras";
      String alamat = "Teras";



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
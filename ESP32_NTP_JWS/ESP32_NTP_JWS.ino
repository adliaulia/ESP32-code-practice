#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char *ssid     = "proxmox.adliaulia.my.id";
const char *password = "P@ssw0rd";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7*3600);
}

void loop() {
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  String weekDay = weekDays[timeClient.getDay()]; 

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;

  //Complete date
  String currentDate = currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  if (currentMonth < 10) { currentDate = String(currentYear) + "-0" + String(currentMonth) + "-" + String(monthDay); }
  if (monthDay < 10)     { currentDate = String(currentYear) + "-" + String(currentMonth) + "-0" + String(monthDay); }

  //API waktu solat
  StaticJsonDocument<1024> doc;
  WiFiClient client;
  HTTPClient http;
  String url = "http://api.banghasan.com/sholat/format/json/jadwal/kota/703/tanggal/";
  url += currentDate;
  
  http.begin(client, url.c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("?");
  String payload = http.getString();

  DeserializationError error = deserializeJson(doc, payload);

  JsonObject results = doc["jadwal"]["data"];
  String ashar = results["ashar"], dhuha = results["dhuha"], dzuhur = results["dzuhur"], imsak = results["imsak"], maghrib = results["maghrib"], isya = results["isya"], subuh = results["subuh"];

  Serial.println("");
  Serial.println("============================================");
  Serial.println("Time and Date Today:");
  Serial.println(weekDay + ", " + String(monthDay) + " " + String(currentMonthName) + " " + String (currentYear) + " " + formattedTime);
  Serial.println("============================================");
  Serial.println("Imsak    : " + imsak);
  Serial.println("Subuh    : " + subuh);
  Serial.println("Dhuha    : " + dhuha);
  Serial.println("Dzuhur   : " + dzuhur);
  Serial.println("Ashar    : " + ashar);
  Serial.println("Maghrib  : " + maghrib);
  Serial.println("Isya     : " + isya);
  Serial.println("============================================");
  Serial.println("");
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  delay(1000);
}

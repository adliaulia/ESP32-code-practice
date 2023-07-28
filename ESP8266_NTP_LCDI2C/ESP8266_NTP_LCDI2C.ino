#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// Replace with your network credentials
const char *ssid     = "proxmox.adliaulia.my.id";
const char *password = "P@ssw0rd";

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

//Month names
String months[12]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void setup(){
  // Initialize Serial Monitor, initialize LCD, turn on LCD backlight
  Serial.begin(115200);
  lcd.init();                 
  lcd.backlight();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Connecting to Wifi!");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    lcd.clear();
  }

  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7*3600);
}

void loop(){
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  if (currentMonth < 10) { currentDate = String(currentYear) + "-0" + String(currentMonth) + "-" + String(monthDay); }
  if (monthDay < 10)     { currentDate = String(currentYear) + "-" + String(currentMonth) + "-0" + String(monthDay); }
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");
  Serial.println("============================================");
  Serial.println("Time and Date Today:");
  Serial.println(weekDay + ", " + String(monthDay) + " " + String(currentMonthName) + " " + String (currentYear) + " " + formattedTime);
  Serial.println("============================================");

  lcd.setCursor(0, 0);
  lcd.print(weekDay);

  lcd.setCursor(8,0);
  lcd.print(formattedTime);

  lcd.setCursor(0, 1);
  lcd.print(String(monthDay) + " " + String(currentMonthName)  + " " + String(currentYear));
  
  delay(1000);
  lcd.clear(); 
}

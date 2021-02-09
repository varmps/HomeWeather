/*Last update 05.feb.2021
  Wifi KIT 8
  VCC - 3.3V
  GND - G
  DHT11 data - D3
  DS18B20 data - D7
 * You have to add your wifi credentials, GScriptId and the sheet/tab name.
 */

 //First time using ESP8266?, see here: https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/
#include <ESP8266WiFi.h>    
#include "HTTPSRedirect.h"    //https://github.com/electronicsguy/ESP8266/tree/master/HTTPSRedirect

#include <Wire.h>                


// Wifi kit 8 display https://github.com/HelTecAutomation/Heltec_ESP32
#include "heltec.h"
//defining the screen width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// DeepSleep time – send data every 60 seconds
const int sleepTimeS = 1;

const char* ssid = "YourWifiName";                //Put wifi ssid within the quotes
const char* password = "YourWifiPassword12%#";    //Put WiFi password within the quotes

const char* host = "script.google.com";
const char *GScriptId = "AKfycbxATESh7SomeRandomLetters-RTYvp-etuu7i"; // Replace with your own google script id
const int httpsPort = 443; //the https port is normally this

const char* fingerprint = "";

String url = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet("tab"/"subsheet" or whatever it is called - lower left in the sheet) name here. I have it "Data2" here.
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Data2\", \
                       \"values\": ";
String payload = "";
HTTPSRedirect* client = nullptr;

/* DHT11 */
#include "DHT.h"
#define DHTPIN 0  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
 
/* DS18B20 */
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);

  Wire.begin();

  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);  
  Heltec.display->setContrast(255);
   
  delay(100);
  Heltec.display->clear();

  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
  Heltec.display->init();
  //Heltec.display->flipScreenVertically(); //You can flip the screen
  
  Heltec.display->display();
  
}

// draw data on screen
void draw (float TempIN,float Humidity,float TempOUT){
  Heltec.display->setContrast(255);
   
  delay(100);
  Heltec.display->clear();

  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
  Heltec.display->init();
  //Heltec.display->flipScreenVertically(); //You can flip the screen

  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0,0, "IN");

  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0,10, String(int(TempIN)) + "C");
  
  Heltec.display->drawVerticalLine(35, 0, 30);
   
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(40,0, "HUM");

  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(40,10, String(int(Humidity)) + "%");

  Heltec.display->drawVerticalLine(80, 0, 30);

  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(85,0, "OUT");

  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(85,10, String(int(TempOUT)) + "C");
  
  Heltec.display->display();
  Serial.println("Draw finished");
}

// data gets pushed to the Google sheets
void postData(float TempIN, float Humidity, float TempOUT){
 
  payload = payload_base + "\"" + String(TempIN) + "," + String(Humidity) + "," + String(TempOUT) + "\"}";
 
  static bool flag = false;
  
  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
      client->POST(url, host, payload, false);
      Serial.print("Sent : ");  Serial.println("Temp and Humid");
    }
  }
  
  Serial.println("POST or SEND Sensor data to Google Spreadsheet:");
  client->POST(url, host, payload);
}

void loop() {
  
  TempIN = dht.readTemperature();
  Humidity = dht.readHumidity();
  
  // Sensor is sometimes unreliable, but usually not 2 times in a row (do not read more often than every 3s)
  if (isnan(Humidity) || isnan(TempIN)|| TempIN>90 || Humidity>90)   
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(4000);
      TempIN = dht.readTemperature();
      Humidity = dht.readHumidity();
  }
 
  Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  TempOUT = sensors.getTempCByIndex(0);
  Serial.print("Temperature is: ");
  Serial.print(TempOUT); 
    
  postData(TempIN,Humidity,TempOUT);
  
  draw(TempIN,Humidity,TempOUT);

  delay (60000*sleepTimeS); //1min delay
}

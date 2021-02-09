/*Last update 05.feb.2021
  Wemos NodeMCU v3
  VCC - 3.3V
  GND - G
  SCL - D1
  SDA - D2
 * You have to add your wifi credentials, GScriptId and the sheet/tab name.
 */

 //First time using ESP8266?, see here: https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/
#include <ESP8266WiFi.h>   
#include "HTTPSRedirect.h"      //https://github.com/electronicsguy/ESP8266/tree/master/HTTPSRedirect

#include <Wire.h>
#include <ccs811.h>             // https://github.com/maarten-pennings/CCS811
#include <ClosedCube_HDC1080.h> // https://github.com/closedcube/ClosedCube_HDC1080_Arduino

// DeepSleep time – send data every 60 seconds
const int sleepTimeS = 1;

//Global sensor objects
#define CCS811_WAK D3

CCS811 ccs811(CCS811_WAK);
ClosedCube_HDC1080 hdc1080;

//default values for testing 
float Temp1 = 21;
float Humidity = 66;
float neCO2 = 2100;
float nTVOC = 1200;
float Pressure = 999;
float Temp2 = 22;

const char* ssid = "YourWifiName";                //Put wifi ssid within the quotes
const char* password = "YourWifiPassword12%#";    //Put WiFi password within the quotes

const char* host = "script.google.com";
const char *GScriptId = "AKfycbxATESh7SomeRandomLetters-RTYvp-etuu7i"; // Replace with your own google script id
const int httpsPort = 443; //the https port is normally this

const char* fingerprint = "";

String url = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet("tab"/"subsheet" or whatever it is called - lower left in the sheet) name here. I have it "Data" here.
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Data\", \
                       \"values\": ";
String payload = "";
HTTPSRedirect* client = nullptr;

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  
// hdc1080 info
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  Wire.begin();

  Serial.println("CCS811 test");
  // Enable CCS811
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok = ccs811.begin();
  if ( !ok ) Serial.println("setup: CCS811 begin FAILED");

  // Print CCS811 versions
  Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(), HEX);
  Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(), HEX);
  Serial.print("setup: application version: "); Serial.println(ccs811.application_version(), HEX);

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok ) Serial.println("init: CCS811 start FAILED");
}

// This is the main method where data gets pushed to the Google sheets
void postData(float Temp1, float Humidity, float neCO2, float nTVOC, float Pressure, float Temp2){
 
  payload = payload_base + "\"" + String(Temp1) + "," + String(Humidity) + "," + String(neCO2) + "," + String(nTVOC) + "," + String(Pressure) + "," + String(Temp2) + "\"}";
 
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
  if (client->POST(url, host, payload)) {
    ;
  }
  
  
}

// Continue pushing data at a given interval
void loop() {
  uint16_t eco2, etvoc, errstat, raw;

  // Pass environmental data from ENS210 to CCS811
  ccs811.set_envdata210(float(hdc1080.readTemperature()), float(hdc1080.readHumidity()));

  ccs811.read(&eco2, &etvoc, &errstat, &raw);
  if ( errstat == CCS811_ERRSTAT_OK ) {

    Serial.print("\n\ntemperature: ");
    Serial.print(hdc1080.readTemperature());
    Serial.print(" C");

    Serial.print("\nhumidity: ");
    Serial.print(hdc1080.readHumidity());
    Serial.print(" %");

    Serial.print("\neCO2 concentration: ");
    Serial.print(eco2);
    Serial.print(" ppm");

    Serial.print("\nTVOC concentration: ");
    Serial.print(etvoc);
    Serial.print(" ppb");

  } else if ( errstat == CCS811_ERRSTAT_OK_NODATA ) {
    Serial.println("CCS811: waiting for (new) data");
  } else if ( errstat & CCS811_ERRSTAT_I2CFAIL ) {
    Serial.println("CCS811: I2C error");
  } else {
    Serial.print("CCS811: errstat="); Serial.print(errstat, HEX);
    Serial.print("="); Serial.println( ccs811.errstat_str(errstat) );
  }
  Serial.println();
  postData(hdc1080.readTemperature(),hdc1080.readHumidity(),eco2,etvoc,Pressure,Temp2);
  
  delay (60000*sleepTimeS); //1min delay
}

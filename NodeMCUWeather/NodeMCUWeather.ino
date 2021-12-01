/*Last update 01.dec.2021
  Wemos NodeMCU v3
  VCC - 3.3V
  GND - G
  SCL - D1
  SDA - D2
 * You have to add your wifi credentials, GScriptId and the sheet/tab name.
 */

 //First time using ESP8266?, see here: https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/
#include <ESP8266WiFi.h>        //Needed for connecting to internet
#include "HTTPSRedirect.h"      //Needed for sending data to web - google sheets'

#include <Wire.h>               // from arduino library
#include <ccs811.h>             // https://github.com/maarten-pennings/CCS811
#include <ClosedCube_HDC1080.h> // https://github.com/closedcube/ClosedCube_HDC1080_Arduino

#include "SSD1306Wire.h"        // from arduino library
#include <Adafruit_BME280.h>    // from arduino library

#define SEALEVELPRESSURE_HPA (1013.25)

//Global sensor objects
#define CCS811_WAK D3

CCS811 ccs811(CCS811_WAK);
ClosedCube_HDC1080 hdc1080;
SSD1306Wire display(0x3c, SDA, SCL);
Adafruit_BME280 bme;

#define RED D6
#define GREEN D7
#define BLUE D8

float Temp1 = 21;
float Humidity = 66;
float neCO2 = 2100;
float nTVOC = 1200;
float Pressure = 999;
float Temp2 = 22;

const char* ssid = "ourWifiName";                //Put wifi ssid within the quotes
const char* password = "YourWifiPassword12%#";      //Put WiFi password within the quotes

const char* host = "script.google.com";
const char *GScriptId = "AKfycbxATESh7SomeRandomLetters-RTYvp-etuu7i"; // Replace with your own google script id
const int httpsPort = 443; //the https port is normally this

const char* fingerprint = "";

String url = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Data\", \
                       \"values\": ";
String payload = "";
HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets upon instantiation

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  // hdc1080 info
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, nWAKE to D3, SDA to D2, SCL to D1]
  //Wire.begin(4, 5);
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

  display.init();
  display.flipScreenVertically();
}

// This is the main method where data gets pushed to the Google sheet
void postData(float hdc1080Temp, float hdc1080Hum, float neCO2, float nTVOC, float bme280Temp, float bme280Hum, float bme280Pres){
 
  payload = payload_base + "\"" + String(hdc1080Temp) + "," + String(hdc1080Hum) + "," + String(neCO2) + "," + String(nTVOC) + "," + String(bme280Temp) + "," + String(bme280Hum) + "," + String(bme280Pres) + "\"}";
 
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
  float hdc1080Temp, hdc1080Hum, bme280Temp, bme280Hum, bme280Pres;

  // Pass environmental data from ENS210 to CCS811
  ccs811.set_envdata210(float(hdc1080.readTemperature()), float(hdc1080.readHumidity()));

  ccs811.read(&eco2, &etvoc, &errstat, &raw);
  if ( errstat == CCS811_ERRSTAT_OK ) {
    
    //Correction factor -5.3 deg
    hdc1080Temp = hdc1080.readTemperature()-5.3;
    Serial.print("\n\ntemperature: ");
    Serial.print(hdc1080Temp);
    Serial.print(" C");

    hdc1080Hum = hdc1080.readHumidity();
    Serial.print("\nhumidity: ");
    Serial.print(hdc1080Hum);
    Serial.print(" %");

    Serial.print("\neCO2 concentration: ");
    Serial.print(eco2);
    Serial.print(" ppm");

    Serial.print("\nTVOC concentration: ");
    Serial.print(etvoc);
    Serial.print(" ppb");

    Serial.println();

    //Correction factor -3.6 deg
    bme280Temp = bme.readTemperature()-3.6;
    Serial.print("BME Temperature = ");
    Serial.print(bme280Temp);
    Serial.println(" *C");

    
    bme280Pres = bme.readPressure() / 100.0F;
    Serial.print("BME Pressure = ");
    Serial.print(bme280Pres);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");
    
    //Correction factor 20% higher
    bme280Hum = 1.2*bme.readHumidity();
    Serial.print("BME Humidity = ");
    Serial.print(bme280Hum);
    Serial.println(" %");

  } else if ( errstat == CCS811_ERRSTAT_OK_NODATA ) {
    Serial.println("CCS811: waiting for (new) data");
  } else if ( errstat & CCS811_ERRSTAT_I2CFAIL ) {
    Serial.println("CCS811: I2C error");
  } else {
    Serial.print("CCS811: errstat="); Serial.print(errstat, HEX);
    Serial.print("="); Serial.println( ccs811.errstat_str(errstat) );
  }
  Serial.println();
  postData(hdc1080Temp,hdc1080Hum,eco2,etvoc,bme280Temp, bme280Hum, bme280Pres);

  display.clear();
  
  int x=0;
  int y=0;
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0 + x, 35 + y, "Hum");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(53 + x, y, "CO2");

  display.setFont(ArialMT_Plain_24);
  String co2 = String(eco2) + " ppm";
  display.drawString(25 + x, 8 + y, co2);
  int co2Width = display.getStringWidth(co2);
  
  display.setFont(ArialMT_Plain_16);
  String hum = String(hdc1080.readHumidity()) + "%";
  display.drawString(0 + x, 45 + y, hum);
  int humWidth = display.getStringWidth(hum);
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(95 + x, 35 + y, "Temp");
  
  display.setFont(ArialMT_Plain_16);
  //Correction factor -5.3 deg
  String temp = String(hdc1080.readTemperature()-5.3) + "°C";
  display.drawString(70 + x, 45 + y, temp);
  int tempWidth = display.getStringWidth(temp);

  display.display();

    if (eco2 > 6000){
      digitalWrite(RED, HIGH);
    }
    else {
      digitalWrite(RED, LOW);
    }
  
  delay (5*60000); //5min delay
}

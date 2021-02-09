# HomeWeather
IOT sensors around the house

## Why?
So I had a punch of IOT boards and sensors and decided to do a freeform weather station. Mainly was interested in indoor humidity and CO2 levels at night. I had previously done some weather stations on breadboards and had them send data to Blynk and Thingspeak but both had their pros and cons. I wanted to have my data saved and accessible from anywhere, more freedom to costumize how data is displayed and I wanted to do it for free. So I found out I can use google scripts to put data to sheets and I do not need any server or third party software for that. 

## Weather Station nr.1
<img src="/img/co2.jpg" width="75%">

### Hardware:
- Wemos NodeMCU V3 ESP8266 dev board
- CCS811+HDC1080 CO2+TVOC and Temperature+Humidity combo board.
- BME280 Temperature+Humidity board
- SSD1306 display board

### Connections
NodeMCU | CCS811 | BME280 | SSD1306 
|---|---|---|---|
3V3 | VCC | VCC | VCC |
GND | GND | GND | GND |
D1 | SCL | SCL | SCL |
D2 | SDA | SDA | SDA |

RGB LED with resistor between NodeMCU ground and cathode, other LED legs at D6, D7, D8.

### Notes:
It seems that HDC1080 does not give correct values, probably because it is too close to CCS811, which is basically always hot. Also my BME280 does not work, not sure if it arrived dead or I killed it. Could not test it before because of the freeform plan. Until a new board arrives I am displaying HDC1080 values on the screen. The CCS811 keeps getting high peaks seemingly at random, so I added also a RGB LED to show me when it had gone "crazy" but one can also utilize the LED as some level indicator etc. I still have the 1.x.x firmware on that chip, maybe I will flash it later to see if it will be more stable then. I will first have it burn-in for a while.


## Weather Station nr.2
<img src="/img/kit8.jpg" width="75%">

### Hardware:
- Wifi KIT 8 Heltec ESP8266 dev board
- DHT11 Temperature+Humidity sensor
- DS18B20 Temperature sensor

### Connections
Wifi KIT 8 | DHT11 | DS18B20  
|---|---|---|
3V3 | VCC | VCC |
GND | GND | GND |
D1 | SCL | SCL |
D2 | SDA | SDA | 

Resistor between DS18B20 Data and VCC.

### Notes:
Even though even Heltec gives a link to [robotzero](https://robotzero.one/heltec-wifi-kit-8/) project on their homepage that uses U8g2 library, I had problems with it (For example flipping the screen) so I think [HelTecAutomation](https://github.com/HelTecAutomation/Heltec_ESP8266) works better. You only need to install it correctly first, which is not as straight forward as most libraries. If you have problems with DHT sensors, always check that you are not getting data too often (etc more than every 3s).

## Google Sheets

<img src="/img/Freeform.png" width="75%">
<img src="/img/data.png" width="75%">

<img src="/img/hum.png" width="75%">

I have data coming from these 2 devices to tabs ("spreadsheets" or "sub-sheets" or...) Data1 and Data2, I can easily add more tabs and devices without breaking the system. Humidity, Temperature and CO2 tabs show corresponing graphs and the latest data is also updated in the "latest" tab.

### Recommended reading:
- [Mohit Bhoite](https://www.bhoite.com/sculptures/) makes some of the coolest freeform out there.
- [Maarten Pennings](https://github.com/maarten-pennings/CCS811) has made a good overview of the CCS811 sensor. Also check discussions under Issues.
- Setting up Google Sheets and your [script](https://www.electronics-lab.com/project/iot-log-sensor-data-google-sheet-using-nodemcu/). If I remember correctly then their arduino code does not work with the new libraries but the script part is nicely explained. 



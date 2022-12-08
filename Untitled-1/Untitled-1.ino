#include<BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include "BH1750FVI.h"
#include<WiFi.h>
#include<WiFiClient.h>


#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL26cODGLX"
#define BLYNK_DEVICE_NAME "Greenhouse Monitoring System"
#define BLYNK_AUTH_TOKEN "7S11cvWxwYQ2t7tWYSc4vMOsEvQMWZ-d"

#define AOUT_PIN 34 // ESP32 pin GIOP34 
#define THRESHOLD 2047
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define I2C_SDA1 33
#define I2C_SCL1 32
#define DHTTYPE DHT11   // DHT 11  (AM2302),
#define SEALEVELPRESSURE_HPA (1013.25)
#define rainAnalog 26

int humidity = 0;
float temperature = 0;
float altitude = 0;
float pressure = 0;
float sealevelpressure = 0;
int inpsoilhumidity = 0;
int soilhumidity = 0;
int inprain = 0;
int rain = 0;
float lightlux = 0;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Ahad (Shadhinota net)";
char pass[] = "abiba0206";

BlynkTimer timer;

TwoWire I2CPIN = TwoWire(0);
unsigned long delayTime;
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;
BH1750FVI lightmeter(0x23, &I2CPIN);

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(500L, myTimer);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  Serial.println(F("sensor test"));
  I2CPIN.begin(I2C_SDA1, I2C_SCL1, 100000);
  bool status;
  status = bmp.begin(0x76, &I2CPIN);

  if (!status) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }
  status = lightmeter.begin();
  if (!status) {
    Serial.println("Could not find a valid light meter sensor, check wiring!");
    while (1);
  }
  
  lightmeter.powerOn();
  lightmeter.setContHighRes();

  delayTime = 1000;
  
  Serial.println();
}

void loop() {
  take_value();
  Blynk.run();
  timer.run();
}
void myTimer() 
{
  Blynk.virtualWrite(V1, temperature);  
}
void take_value(){
     humidity = dht.readHumidity();
     temperature = bmp.readTemperature();
     altitude = bmp.readAltitude();
     pressure = bmp.readPressure();
     sealevelpressure = bmp.readSealevelPressure();
     inpsoilhumidity = analogRead(AOUT_PIN);
     soilhumidity = map(inpsoilhumidity, 1400, 4029, 100, 0);
     inprain = analogRead(rainAnalog);
     rain = map(inprain, 1200, 4029, 100, 0);
     lightlux = lightmeter.getLux();;
     print(humidity, temperature, altitude, pressure, sealevelpressure, soilhumidity, rain, lightlux);
    
}
 
void print(int humidity, int temperature, int altitude, int pressure, int sealevelpressure, int soilhumidity, int rain, float lightlux){
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.print("altitude");
    Serial.print(altitude);
    Serial.println(" meters");
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" Pa");
    Serial.print("Sea Level Pressure: ");
    Serial.print(sealevelpressure);
    Serial.println(" Pa");
    Serial.print("Soil humidity: ");
    Serial.print(soilhumidity);
    Serial.println("%");
    Serial.print("Rain level: ");
    Serial.print(rain);
    Serial.println("%");
    Serial.print("Light level (LUX): ");
    Serial.print(lightlux, 1);
    Serial.println(" lux");
    Serial.println("\n");  
    Blynk.virtualWrite(V1, temperature);
}

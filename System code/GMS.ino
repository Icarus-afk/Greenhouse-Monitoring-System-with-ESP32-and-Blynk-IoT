#include <ESP32Servo.h>
#include<BlynkSimpleEsp32.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include "BH1750FVI.h"
#include<WiFi.h>
#include<WiFiClient.h>


#define SERVO_PIN 13 // ESP32 pin GIOP13 connected to servo motor
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL26cODGLX"
#define BLYNK_DEVICE_NAME "Greenhouse Monitoring System"
#define BLYNK_AUTH_TOKEN "7S11cvWxwYQ2t7tWYSc4vMOsEvQMWZ-d"
#define AOUT_PIN 34 // ESP32 pin GIOP34 
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define I2C_SDA1 33
#define I2C_SCL1 32
#define DHTTYPE DHT11   // DHT 11  (AM2302),
#define SEALEVELPRESSURE_HPA (1013.25)
#define rainAnalog 35


const float realalt = 3.28084;
int pos1=0;
int humidity = 0;
float temperature = 0;
float altitude = 0;
float pressure = 0;
float sealevelpressure = 0;
int soilhumidity = 0;
int rain = 0;
float lightlux = 0;
int flag_1 = 0;
int flag_2 = 0;
int flag_3 = 0;
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Ahad (Shadhinota net)";
char pass[] = "abiba0206";
char server[] = "192.168.1.11";

Servo servoMotor;
BlynkTimer timer;
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
TwoWire I2CPIN = TwoWire(0);
unsigned long delayTime;

BH1750FVI lightmeter(0x23, &I2CPIN);



Adafruit_BMP085 bmp;


void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(500L, myTimer);
  servoMotor.attach(SERVO_PIN);
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
  dbconnect();
}
void myTimer()
{
  Blynk.virtualWrite(V0, humidity);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, altitude);
  Blynk.virtualWrite(V3, pressure);
  Blynk.virtualWrite(V4, sealevelpressure);
  Blynk.virtualWrite(V5, soilhumidity);
  Blynk.virtualWrite(V6, rain);
  Blynk.virtualWrite(V7, lightlux);
}
void take_value() {
  humidity = dht.readHumidity();
  temperature = bmp.readTemperature();
  altitude = bmp.readAltitude()*realalt;
  pressure = bmp.readPressure();
  sealevelpressure = bmp.readSealevelPressure();
  soilhumidity = analogRead(AOUT_PIN);
  soilhumidity = constrain(soilhumidity, 1500,4029);
  soilhumidity = map(soilhumidity, 1400, 4029, 100, 0);
  rain = analogRead(rainAnalog);
  rain = constrain(rain, 0,4029);
  rain = map(rain, 0 , 4029, 100, 0);
  lightlux = lightmeter.getLux();

  check_val(soilhumidity, rain);
  print(humidity, temperature, altitude, pressure, sealevelpressure, soilhumidity, rain, lightlux);

}
void check_val(int soilhumidity, int rain) {
  if (soilhumidity >= 50) {
    flag_1 = 1;
  }
  else {
    flag_1 = 0;
  }

  if (rain >= 40){
    flag_2 = 1;
  }
  else{
    flag_2 = 0;
  }
  if(flag_1==0 && flag_2 ==1 && pos1==0){
    servoMotor.write(90);
    pos1 = 90;
    delay(10);
  }
  else if(flag_1==1 && flag_2 ==1 && pos1==90){
    servoMotor.write(0);
    pos1 = 0;
    delay(10);
  }
  else if(flag_1==0 && flag_2 ==0 && pos1 == 90){
    servoMotor.write(0);
    pos1 = 0;
    delay(10);
  }
}
void print(int humidity, int temperature, int altitude, int pressure, int sealevelpressure, int soilhumidity, int rain, float lightlux) {
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
}

void dbconnect(){
  
 if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /iot/connect.php?humidity=");
    client.print("GET /iot/connect.php?humidity=");     //YOUR URL
    Serial.print(humidity);
    client.print(humidity);
    client.print("&temperature=");
    Serial.print("&temperature=");
    client.print(temperature);
    Serial.print(temperature);
    client.print("&altitude=");
    client.print(altitude);
    Serial.print(altitude);
    client.print("&pressure=");
    client.print(pressure);
    Serial.print(pressure);
    client.print("&sealevelpressure=");
    client.print(sealevelpressure);
    Serial.print(sealevelpressure);
    client.print("&soilhumidity=");
    client.print(soilhumidity);
    Serial.print(soilhumidity);
    client.print("&rain=");
    client.print(rain);
    Serial.print(rain);
    client.print("&lightlux=");
    client.print(lightlux);
    Serial.print(lightlux);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 192.168.1.11");
    client.println("Connection: close");
    client.println();
    delay(500);
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
    }

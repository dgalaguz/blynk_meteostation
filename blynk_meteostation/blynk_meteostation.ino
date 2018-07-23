
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Auth params.
char auth[] = "a01139016b6d4749b16a402f9f043656";
char ssid[] = "ASUS";
char pass[] = "asuswifinet";

// Temp sensor set up.
DHT dht(14, DHT22, 11);
float humidity, temp;
unsigned long tempReadPreviousMillis = 0;
const long tempReadInterval = 2000;

void readTempSensor(){
  unsigned long currentMillis = millis();
  if(currentMillis - tempReadPreviousMillis >= tempReadInterval) {
    tempReadPreviousMillis = currentMillis;   
    humidity = dht.readHumidity();
    temp = dht.readTemperature();
    if (isnan(humidity) || isnan(temp)) {
      humidity = 0;
      temp = 0;
    }
  }
}

BLYNK_READ(V0)
{
  readTempSensor();
  Blynk.virtualWrite(V0, temp);
}

BLYNK_READ(V1)
{
  readTempSensor();
  Blynk.virtualWrite(V1, humidity);
}

// Illuminance sensor set up.
BH1750 lightMeter;

uint16_t lux = 0;
unsigned long illuminanceReadPreviousMillis = 0;
const long illuminanceReadInterval = 1000;

void readIlluminanceSensor(){
  unsigned long currentMillis = millis();
  if(currentMillis - illuminanceReadPreviousMillis >= illuminanceReadInterval) {
    illuminanceReadPreviousMillis = currentMillis; 
    lux = lightMeter.readLightLevel();
  }
}

BLYNK_READ(V3)
{
  readIlluminanceSensor();
  Blynk.virtualWrite(V3, lux/1000.0);
}

// Relays setup.
int vent_pin = 4;
int light_pin = 5;

int vent_status = 0;
int light_status = 0;

BLYNK_WRITE(V4){
  vent_status = param.asInt();
  digitalWrite(vent_pin, vent_status);
}

BLYNK_WRITE(V5){
  light_status = param.asInt();
  digitalWrite(light_pin, light_status);
}

// Sync on app connect.
BLYNK_APP_CONNECTED() {
  Blynk.virtualWrite(V4, vent_status);
  Blynk.virtualWrite(V5, light_status);
}

void setup()
{
  pinMode(vent_pin, OUTPUT);
  pinMode(light_pin, OUTPUT);
  Serial.begin(115200);  
  dht.begin();
  Wire.begin(D7, D6);
  lightMeter.begin();
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();  
}


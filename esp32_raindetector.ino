// import servo
#include <Servo.h>

// import dht22
#include "DHT.h"
#define DHT_PIN 12

// import ldr
#include <Wire.h>
#include <BH1750.h>

#define SERVO_PIN 26
#define RAIN_SENSOR_PIN 36

// import blynk
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_TEMPLATE_ID "TMPL6FwK5cAkN"
#define BLYNK_TEMPLATE_NAME "RainDetector"
#define BLYNK_AUTH_TOKEN "RP3Q17Mi460_J-jo6KNVdluxcsNqDy_L"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "testing";
char pass[] = "satset123";
   
DHT dht(DHT_PIN, DHT22);
BH1750 lightMeter;

Servo servoMotor;

int angle = 0;
int prev_rain_state;
int rain_state;

int autoMode;
int autoGate;

bool rainState() {
  rain_state = analogRead(RAIN_SENSOR_PIN);
  Blynk.virtualWrite(V2, rain_state);
  if (rain_state <= 1500) {
    Serial.print(rain_state);
    Serial.println(" :Rain detected!");
    return true;
  } else {
    Serial.print(rain_state);
    Serial.println("Rain stopped!");
    return false;
  } 
}

bool lightState() {
  int lux = lightMeter.readLightLevel();
  Blynk.virtualWrite(V1, lux);
  switch(lux) {
        case 25000 ... 60000:
            Serial.print("Panas - ");
            Serial.print("Light: ");
            Serial.print(lux);
            Serial.println(" lx");
            return true;
            break;
        case 14000 ... 24999:
            Serial.print("Berawan - ");
            Serial.print("Light: ");
            Serial.print(lux);
            Serial.println(" lx");
            return true;
            break;
        case 1800 ... 13999:
            Serial.print("Mendung - ");
            Serial.print("Light: ");
            Serial.print(lux);
            Serial.println(" lx");
            return false;
            break;
        case 0 ... 1799:
            Serial.print("Malam - ");
            Serial.print("Light: ");
            Serial.print(lux);
            Serial.println(" lx");
            return false;
            break;
        default:
            Serial.print("Kondisi cuaca tidak valid: ");
            break;
  }
}

bool humadityState() {
  float h = dht.readHumidity();
  Blynk.virtualWrite(V0, h);
  Serial.println(h);
  if (h >= 90) {
    return true;
  } else if (h < 90) {
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Blynk.begin(auth, ssid, pass);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  servoMotor.attach(SERVO_PIN);

  servoMotor.write(angle);
  rain_state = analogRead(RAIN_SENSOR_PIN);
  lightMeter.begin();
  dht.begin();
  autoMode = 0;
  autoGate = 0;
}

void loop() {
  Mode();
  delay(2000);
  Blynk.run();
}

void status(int s) {
  if(s == 1){
    return servoMotor.write(90);  // dibuka
  } else if (s == 0) {
    return servoMotor.write(0);  // ditutup
  }
}

void Mode() {
  rainState();  
  lightState();  
  humadityState();
  if (autoMode == 1){
      bool nilaiRainState = rainState();  // true : mendeteksi air || false : tidak mendeteksi air
      bool nilaiLightState = lightState();  // true : mendeteksi cahaya || false : tidak mendeteksi cahaya
      bool nilaiHumadityState = humadityState();  // true : mendeteksi kelembapan tinggi || false : mendeteksi kelembapan rendah
    
      // rotates from 0 degrees to 180 degrees
      if (nilaiRainState == false && nilaiLightState == false && nilaiHumadityState == false) {
          Serial.println("1. Kanopi Ditutup!");
          status(1);
        } else if (nilaiRainState == false && nilaiLightState == false && nilaiHumadityState == true) {
          Serial.println("2. Kanopi Ditutup!");
          status(1);
        } else if (nilaiRainState == false && nilaiLightState == true && nilaiHumadityState == false) {
          Serial.println("3. Kanopi Dibuka!");
          status(0);
        } else if (nilaiRainState == false && nilaiLightState == true && nilaiHumadityState == true) {
          Serial.println("4. Kanopi Dibuka!");
          status(0);
        } else if (nilaiRainState == true && nilaiLightState == false && nilaiHumadityState == true) {
          Serial.println("5. Kanopi Ditutup!");
          status(1);
        } else if (nilaiRainState == true && nilaiLightState == false && nilaiHumadityState == false) {
          Serial.println("6. Kanopi Ditutup!");
          status(1);
        } else if (nilaiRainState == true && nilaiLightState == true && nilaiHumadityState == true) {
          Serial.println("7. Kanopi Ditutup!");
          status(1);
        } else if (nilaiRainState == true && nilaiLightState == true && nilaiHumadityState == false) {
          Serial.println("8. Kanopi Dibuka!");
          status(0);
        } 
    } else if (autoMode == 0) {
        if (autoGate == 1) {
          status(1);
        } else if (autoGate == 0) {
          status(0);
        }
    }
}

BLYNK_WRITE(V3) {
    int n = param.asInt();
    autoMode = n;
}

BLYNK_WRITE(V4) {
    int s = param.asInt(); // assigning incoming value from pin V1 to a variable
    autoGate = s;
}

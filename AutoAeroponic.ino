#include <RGBLed.h>
#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <CytronMotorDriver.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define ssid "Anonymous"
#define pass "12345678"
#define URL "https://smart-green-system-default-rtdb.firebaseio.com/"
#define API "AIzaSyDRzzWU4I5rRwN4R1Vh_Pzd88xq8WByU28"
#define dhtType DHT11
#define freq 5000
#define freqz 3200000
#define typeC RGBLed::COMMON_CATHODE
#define typeA RGBLed::COMMON_ANODE
#define dhtPin 13
#define fanHotPos 12
#define fanHotNeg 14
#define fanHotPwm 27
#define fanColdPos 26
#define fanColdNeg 25
#define fanColdPwm 33
#define pump 32
#define tecHotDir 15
#define tecHotPwm 2
#define tecColdDir 4
#define tecColdPwm 16
#define r1 17
#define g1 5
#define b1 19
#define r2 21
#define g2 3
#define b2 1
#define uvAPin 22
#define uvBPin 23
typedef unsigned long long int ulli;
typedef const long long int clli;
FirebaseData fdata, fchange;
FirebaseAuth auth;
FirebaseConfig config;
ulli freq2, prevDataIn = 0, prevLedOut = 0, prevTempOut = 0, prevWater = 0, prevGetLedStatus = 0;
int ctrTemp = 0, flagWater = 1, flagLed = 1;
bool changeData = true, signup = false, tempo = true;
struct Plant {
  int wl1 = 0, wl2 = 0, pperiod = 0, minTemp = 0, maxTemp = 0, waterv = 0, uvA = 0, uvB = 0;
} plant;
struct light {
  int r, g, b;
} rgb1, rgb2;
RGBLed led1(r1, g1, b1, typeC);
RGBLed led2(r2, g2, b2, typeC);
DHT dht(dhtPin, dhtType);
CytronMD tecHot(PWM_DIR, tecHotPwm, tecHotDir);
CytronMD tecCold(PWM_DIR, tecColdPwm, tecColdDir);

void setup() {
  pinMode(dhtPin, 0); 
  pinMode(fanHotPos, 1);
  pinMode(fanHotNeg, 1);
  pinMode(fanHotPwm, 1);
  pinMode(fanColdPos, 1); 
  pinMode(fanColdNeg, 1);
  pinMode(fanColdPwm, 1); 
  pinMode(pump, 1);  
  pinMode(r1, 1);
  pinMode(g1, 1);
  pinMode(b1, 1);
  pinMode(r2, 1);
  pinMode(g2, 1);
  pinMode(b2, 1);
  pinMode(uvAPin, 1);
  pinMode(uvBPin, 1);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  config.api_key = API;
  config.database_url = URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signup OK");
    signup = true;
  } else
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (Firebase.RTDB.beginStream(&fdata, "Plant/wavelength_1"))
    Serial.printf("wavelength_1 begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/wavelength_2"))
    Serial.printf("wavelength_2 begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/photoperiod"))
    Serial.printf("photoperiod begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/minTemp"))
    Serial.printf("minTemp begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/maxTemp"))
    Serial.printf("maxTemp begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Data/temp"))
    Serial.printf("temp begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Data/ledStatus"))
    Serial.printf("ledStatus begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/waterVolume"))
    Serial.printf("waterVolume begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/uvA"))
    Serial.printf("uvA begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fdata, "Plant/uvB"))
    Serial.printf("uvB begin error, %s\n", fdata.errorReason().c_str());
  if (Firebase.RTDB.beginStream(&fchange, "Data/changeData"))
    Serial.printf("changeData begin error, %s\n", fchange.errorReason().c_str());
  dht.begin();
}

void loop() {
  while (isnan(dht.readTemperature()))
    Serial.println("DHT Error"); // doesn't exit the loop untill the DHT reads
  if ((Firebase.ready() && signup && ((millis() - prevDataIn > freq) || prevDataIn == 0)) && changeData) {
    if (Firebase.RTDB.getInt(&fdata, "Plant/wavelength_1")) {
      if (fdata.dataType() == "int") {
        plant.wl1 = fdata.intData();
        Serial.println(plant.wl1 + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read wavelength_1 because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/wavelength_2") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.wl2 = fdata.intData();
        Serial.println(plant.wl2 + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read wavelength_2 because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/photoperiod") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.pperiod = fdata.intData();
        Serial.println(plant.pperiod + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read photoperiod because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/minTemp") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.minTemp = fdata.intData();
        Serial.println(plant.minTemp + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read minTemp because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/maxTemp") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.maxTemp = fdata.intData();
        Serial.println(plant.maxTemp + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read maxTemp because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/uvA") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.uvA = fdata.intData();
        Serial.println(plant.uvA + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read uvA because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/waterVolume") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.waterv = fdata.intData();
        Serial.println(plant.waterv + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read uvA because " + fdata.errorReason());
    if (Firebase.RTDB.getInt(&fdata, "Plant/uvB") && fdata.dataType() == "int") {
      if (fdata.dataType() == "int") {
        plant.uvB = fdata.intData();
        Serial.println(plant.uvB + " - sucessfully read from " + fdata.dataPath());
      }
    } else
      Serial.println("Failed to read uvB because " + fdata.errorReason());
    prevDataIn = millis();
    prevLedOut = prevDataIn;
    freq2 = freqz + prevDataIn;
  } /*all of the copies above are used to recieve data on the ESP from the mobile application through the firebase, 
  and it checks for data changes every "freq1" seconds
  Note: "prevLedOut = prevDataIn" is written to be the base case for sending LedStatus to the application through the firebase*/
  if (Firebase.ready() && signup) {
    if (!Firebase.RTDB.readStream(&fchange))
      Serial.printf("Stream read error because %s\n", fchange.errorReason().c_str());
    if (fchange.streamAvailable()) {
      if (fchange.dataType() == "bool") {
        changeData = fchange.boolData();
        Serial.println("Successfully read change data: " + changeData);
      }
    }
  } /* a realtime stream data input from the android app through the firebase in order to
  know wether the user wants to change the data or not (if changeData is 1, the user will change the 
  data, but if it's 0, the data is already changed).*/
  if (changeData)
    ctrTemp = 0;
  //initializes the counter if the data will change
  if (!changeData) {
    if (Firebase.ready() && signup && (millis() - prevLedOut > freq || prevLedOut == prevDataIn)) {
      prevLedOut = millis();
      if (ctrTemp < 2)
        ctrTemp++;
      if (Firebase.RTDB.setInt(&fdata, "Data/ledStatus", getLedStatus()))
        Serial.println(getLedStatus() + " - sucessfully saved to: " + fdata.dataPath());
      else
        Serial.println("Failed to save ledStatus because " + fdata.errorReason());
    }
    sendTemp();
  }
  adjustTemp(); 
  lightControl();
  water(); 
}

int getTemp() {
  int t = round(dht.readTemperature());
  return t;
} //returns the air temperature

void sendTemp() {
  if (Firebase.ready() && signup && (millis() - prevTempOut > freq2 || ctrTemp == 1)) {
    prevTempOut = millis();
    if (ctrLed < 1) {
    ctrLed++;
    prevGetLedStatus = millis();
    }
    if (Firebase.RTDB.setInt(&fdata, "Data/temp", getTemp()))
      Serial.println(getTemp() + " - sucessfully saved to: " + fdata.dataPath());
    else
      Serial.println("Failed to save temp because " + fdata.errorReason());
  }
} //Sends temperature to the firebase every "freq2" seconds.

int getLedStatus() {
  if(changeData)
    return 0;
  ulli pmilli = plant.pperiod * 3.6e6;
  clli milliDay = 24 * 3.6e6;
  ulli npmilli = milliDay - pmilli;
  if (ctrWater < 1) {
    ctrWater++;
    prevWater = millis();
  }
  ledCheck:
  if (millis() - prevGetLedStatus < milliDay ){
    if (millis() - prevGetLedStatus < pmilli){
      flagLed = 1;
    } else {
      flagLed = 0;
    }
    
  } else {
      prevGetLedStatus = millis();
      goto ledCheck;
  }
  return flagLed;
} //returns the LEDs status (wheter turned off or on). 

void water() {
  int onTime = 30000, offTime = 72000, cycle = offTime + onTime;
  if (!changeData){
    waterCheck:
    if (millis() - prevWater < cycle){
      if (millis() - prevWater < onTime) {
        digitalWrite(pump, 1);
      } else {
        digitalWrite(pump, 0);
      }
    } else {
    prevWater = millis();
    goto waterCheck;
    } 
  }
} //controls water flow for irrigation 

void torgb(int wavelength, struct light* rgb) {
  float red, green, blue;
  if (wavelength >= 380 && wavelength < 440) {
    red = (440 - wavelength) / (440 - 380);
    green = 0.0;
    blue = 1.0;
  } else if (wavelength >= 440 && wavelength < 490) {
    red = 0.0;
    green = (wavelength - 440) / (490 - 440);
    blue = 1.0;
  } else if (wavelength >= 490 && wavelength < 510) {
    red = 0.0;
    green = 1.0;
    blue = (510 - wavelength) / (510 - 490);
  } else if (wavelength >= 510 && wavelength < 580) {
    red = (wavelength - 510) / (580 - 510);
    green = 1.0;
    blue = 0.0;
  } else if (wavelength >= 580 && wavelength < 645) {
    red = 1.0;
    green = (645 - wavelength) / (645 - 580);
    blue = 0.0;
  } else if (wavelength >= 645 && wavelength <= 780) {
    red = 1.0;
    green = 0.0;
    blue = 0.0;
  } else {
    red = 0.0;
    green = 0.0;
    blue = 0.0;
  }
  red *= 255;
  green *= 255;
  blue *= 255;
  rgb->r = round(red);
  rgb->g = round(green);
  rgb->b = round(blue);
} //turns wavelength into rgb value

void lightControl() {
  torgb(plant.wl1, &rgb1);
  torgb(plant.wl2, &rgb2);
  if (getLedStatus()) {
    led1.setColor(rgb1.r, rgb1.g, rgb1.b);
    led2.setColor(rgb2.r, rgb2.g, rgb2.b);
  } else {
    led1.off();
    led2.off();
  }
  if (getLedStatus() && plant.uvA)
    digitalWrite(uvAPin, 1);
  else
    digitalWrite(uvAPin, 0);
  if (getLedStatus() && plant.uvB)
    digitalWrite(uvBPin, 1);
  else
    digitalWrite(uvBPin, 0);
} //controls the lighting provided to the plant

void off() {
  digitalWrite(fanHotPos, 0);
  digitalWrite(fanHotNeg, 0);
  analogWrite(fanHotPwm, 0);
  digitalWrite(fanColdPos, 0);
  digitalWrite(fanColdNeg, 0);
  analogWrite(fanColdPwm, 0);
  tecHot.setSpeed(0);
  tecCold.setSpeed(0);
} //turns off all the temperature adjustment system's components

bool cold() {
  if (getTemp() < plant.minTemp)
    return 1;
  else
    return 0;
} //checks if the temp is cold

void heat(int targetTemp) {
  while (getTemp() <= targetTemp) {
    if (changeData){
      off();
      break;
    }
    digitalWrite(fanHotPos, 1);
    digitalWrite(fanHotNeg, 0);
    analogWrite(fanHotPwm, 255);
    digitalWrite(fanColdPos, 0);
    digitalWrite(fanColdNeg, 1);
    analogWrite(fanColdPwm, 255);
    tecHot.setSpeed(255);
    tecCold.setSpeed(0);
    sendTemp();
    lightControl();
    water();
  }
} //heats the air until targetTemp, and all main functions are included in order not to intrrupt any of them because of the for loop

bool hot() {
  if (getTemp() > plant.maxTemp)
    return 1;
  else
    return 0;
} //checks if the temp is hot

void cool(int targetTemp) {
  while (getTemp() >= targetTemp) {
    if (changeData){
      off();
      break;
    }
    digitalWrite(fanHotPos, 0);
    digitalWrite(fanHotNeg, 1);
    analogWrite(fanHotPwm, 255);
    digitalWrite(fanColdPos, 1);
    digitalWrite(fanColdNeg, 0);
    analogWrite(fanColdPwm, 255);
    tecHot.setSpeed(0);
    tecCold.setSpeed(255);
    sendTemp();
    lightControl();
    water();
  }
} //cools the air until targetTemp, and all main functions are included in order not to intrrupt any of them because of the for loop

void adjustTemp() {
  int morl = round((plant.maxTemp - plant.minTemp) / 3);
  if (cold())
    heat(plant.minTemp + morl);
  else if (hot())
    cool(plant.maxTemp - morl);
  else
    off();
} /*adjusts the temperature by using cool, heat and off functions. 
  Note: morl is made to make the adjustment run until being somehow close to the average temperature, 
  and then stops until the system until reaching the minTemp or maxTemp again. This is done in order not
  to make the tec turn on and off a lot in a short period of time.*/

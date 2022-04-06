#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "realme 3 pro"
#define WIFI_PASSWORD "asdfghjkl"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBblca24-sJ-ISJ7s0xlpRPjHkoIO_XgdI"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://lasem-ac1ae-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
int jarak;
int triggerBuzzer;
int triggerLed;
int ledStatus;
int buzzerStatus;
int information;
bool signupOK = false;
const int triggerPin = 12;
const int echoPin = 13;
const int led = 4;
const int buzzer = 2;
void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop(){
  long duration;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration/2) / 29.1;
  Serial.println("jarak :");
  Serial.println(jarak);
  String jarakString = String(jarak);
  Serial.print(jarakString);
  Serial.println(" cm");
  if (jarak<=30){
    delay(5000);
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    jarak = (duration/2) / 29.1;
    Serial.print("jarak :");
    Serial.print(jarak);
    Serial.println(" cm");
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, HIGH);
    if(jarak<=30){
      digitalWrite(led, HIGH);
      digitalWrite(buzzer, HIGH);
      //Serial.print(jarak);
      Serial.println("information : bahaya");
      ledStatus = 1;
      buzzerStatus = 1;
      information = 1;
    }
    else{
      digitalWrite(led, LOW);
      digitalWrite(buzzer, LOW);
      ledStatus = 0;
      buzzerStatus = 0;
      information = 0;
      Serial.println("information : aman");
    }
  }
  else{
    digitalWrite(led, LOW);
    digitalWrite(buzzer, LOW);
    ledStatus = 0;
    buzzerStatus = 0;
    information = 0;
    Serial.println("information : aman");
  }
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/monitoring/trigBuzzer")) {
      if (fbdo.dataType() == "int") {
        triggerBuzzer = fbdo.intData();
        Serial.println(triggerBuzzer);
        if(triggerBuzzer==1){
          //digitalWrite(led, HIGH);
          digitalWrite(buzzer, HIGH);
          //ledStatus=1;
          buzzerStatus=1;
          information=1;
          //exit;
        }
        else{
          //digitalWrite(led, LOW);
          //digitalWrite(buzzer, LOW);
        }
      }
    }
    if (Firebase.RTDB.getInt(&fbdo, "/monitoring/ledBuzzer")) {
      if (fbdo.dataType() == "int") {
        triggerLed = fbdo.intData();
        Serial.println(triggerLed);
        if(triggerLed==1){
          digitalWrite(led, HIGH);
          //digitalWrite(buzzer, HIGH);
          ledStatus=1;
          //buzzerStatus=1;
          information=1;
          //exit;
        }
        else{
          //digitalWrite(led, LOW);
          //digitalWrite(buzzer, LOW);
        }
      }
    }
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setString(&fbdo, "monitoring/jarak", jarakString)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //count++;
    if (Firebase.RTDB.setInt(&fbdo, "monitoring/ledStatus", ledStatus)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setInt(&fbdo, "monitoring/buzzerStatus", buzzerStatus)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setInt(&fbdo, "monitoring/information", information)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }  
  }
  delay(1000);
}

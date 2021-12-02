// SOURCE: https://www.zweitaktforum.de/wbb/forum/thread/16035654-rave-control-v2-universal-auslassteuerung-mit-modellbauservo/?postID=43437945#post43437945
// Created by: el bodo es loco
// ARDUINO CODE FOR TEST
// Connect Arduino PIN 13 with ESP PIN 2 & ARDUINO GND to ESP32 GND
#include <Arduino.h>

int sensorPin = A0;
int ledPin = 13;
int rpmPin = 2;

void setup () {
  pinMode(ledPin, OUTPUT);
  pinMode(rpmPin, OUTPUT);
  Serial.begin (9600);
}

void loop () {
  
float  t = map (analogRead (sensorPin), 0, 1023, 32, 220);          

  tone(13, t);
  
  Serial.print ("RPM:");
  Serial.println(t*60); 
}

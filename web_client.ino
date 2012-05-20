#include <Ethernet.h>
#include <SPI.h>
#include "init.h"

//init file helps build some of these variables
IPAddress server(  SERVER_BYTE_1,
                   SERVER_BYTE_2,
                   SERVER_BYTE_3,
                   SERVER_BYTE_4
);

byte mac[] = {  MAC_BYTE_1, 
                MAC_BYTE_2, 
                MAC_BYTE_3, 
                MAC_BYTE_4, 
                MAC_BYTE_5, 
                MAC_BYTE_6
};

EthernetClient client;

//indicators
int leds[] = { 22, 26, 30, 34, 38, 42, 46, 48 };
int ledCount = 8;
int ledPos = 0; //position of led in loading pattern
boolean ledForward = true; //direction of the loading pattern
int status = 0; //used to trigger a reset

//read delay
long previousMillis = 0;
long readInterval = 5000; 

//ethernet
int pageValue = 0;
boolean connectFail = false;  //flag for a connection failure
boolean startRead = false;    // if the program is reading data

void setup(){
  Ethernet.begin(mac);
  Serial.begin(9600);

  //setup indicators
  for(int i = 0; i < ledCount; i++) {
    //outputs
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
    //grounds
    pinMode(leds[i] + 1, OUTPUT);
    digitalWrite(leds[i] + 1, LOW);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  //string pageString[32];
  if(currentMillis - previousMillis > readInterval || previousMillis == 0) {
    previousMillis = currentMillis;
    pageValue = connectAndRead(); //connect to the server and read the output
  }

  if(connectFail) {
    //handle error
    if(status != 1) {
      indicatorsReset();
    }
    status = 1;
    indicatorBlinkOne();
  } else {
    if(pageValue == 0) {
      if(status != 2) {
        indicatorsReset();
      }
      status = 2;
      indicatorLoading();
    } else if(pageValue <= ledCount) {
      if(status != 3) {
        indicatorsReset();
      }
      status = 3;
      indicatorLightQuantity(pageValue);
    } else {
      if(status != 4) {
        indicatorsReset();
      }
      status = 4;
      indicatorBlinkAll();
    }
  }
  //Serial.println(pageValue); //print out the findings.
  //delay(15000); //wait 5 seconds before connecting again
}

void indicatorsReset() {
  for(int i = 0; i < ledCount; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void indicatorBlinkOne() {
  digitalWrite(leds[0], HIGH);
  delay(300);
  digitalWrite(leds[0], LOW);
  delay(300);
}

void indicatorLoading() {
  if(ledPos == ledCount - 1) {
    ledForward = false;
  }
  if(ledPos == 0) {
    ledForward = true;
  }

  digitalWrite(leds[ledPos], HIGH);
  delay(100);
  digitalWrite(leds[ledPos], LOW);
  delay(100);

  if(ledForward) {
    ledPos++;
  } else {
    ledPos--;
  }
}

void indicatorLightQuantity(int ledTurnOnCount) {
  for(int i = 0; i < ledTurnOnCount; i++) {
    digitalWrite(leds[i], HIGH);
  }
}

void indicatorBlinkAll() {
  for(int i = 0; i < ledCount; i++) {
    digitalWrite(leds[i], HIGH);
  }
  delay(300);
  for(int i = 0; i < ledCount; i++) {
    digitalWrite(leds[i], LOW);
  }
  delay(300);
}

int connectAndRead() {
  //connect to the server
  Serial.println("connecting...");
  connectFail = false;
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.println(REQUEST_STRING);
    client.println(REQUEST_HOST);
    client.println();

    //Connected - Read the page
    return readPage();
  } else {
    Serial.println("connection failure!");
    connectFail = 1;
    return 0;
  }

}

//capture & return integer between brackets
int readPage() {
  while(true){
    if (client.available()) {
      char c = client.read();

      if (c == '{' ) {
        startRead = true;
      } else if(startRead) {
        startRead = false;
        client.stop();
        client.flush();
        Serial.println("disconnecting.");
        Serial.println(c);

        if(c == '0') return 0;
        if(c == '1') return 1;
        if(c == '2') return 2;
        if(c == '3') return 3;
        if(c == '4') return 4;
        if(c == '5') return 5;
        if(c == '6') return 6;
        if(c == '7') return 7;
        if(c == '8') return 8;
        if(c == 'x') return ledCount + 1;
        return 0;
      }
    }
  }
}
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

boolean connectFail = false;  //flag for a connection failure
boolean startRead = false;    // if the program is reading data
char inString[32];            // string for incoming serial data
int stringPos = 0;            // string index counter

void setup(){
  Ethernet.begin(mac);
  Serial.begin(9600);
}

void loop(){
  String pageValue = connectAndRead(); //connect to the server and read the output
  if(connectFail) {
    //handle error
  } else {

  }
  Serial.println(pageValue); //print out the findings.
  delay(15000); //wait 5 seconds before connecting again
  connectFail = false;
}

String connectAndRead(){
  //connect to the server
  Serial.println("connecting...");

  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.println(REQUEST_STRING);
    client.println(REQUEST_HOST);
    client.println();

    //Connected - Read the page
    return readPage();
  }else{
    connectFail = 1;
    return "connection failed";
  }

}

//capture & return integer between brackets
String readPage(){
  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '{' ) {
        startRead = true;
      } else if(startRead) {
        if(c != '}') {
          inString[stringPos] = c;
          stringPos ++;
        } else {
          //got everything
          startRead = false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");
          return int(inString);
        }
      }
    }
  }
}
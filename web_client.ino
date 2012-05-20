/*
  Web client
 
 based off code by David A. Mellis
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include "init.h"

//mac and server built from init file
byte mac[] = {  MAC_BYTE_1, 
                MAC_BYTE_2, 
                MAC_BYTE_3, 
                MAC_BYTE_4, 
                MAC_BYTE_5, 
                MAC_BYTE_6
};

IPAddress server(  SERVER_BYTE_1,
                   SERVER_BYTE_2,
                   SERVER_BYTE_3,
                   SERVER_BYTE_4
);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // start the serial library:
  Serial.begin(9600);
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println(REQUEST_STRING);
    client.println(REQUEST_HOST);
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}

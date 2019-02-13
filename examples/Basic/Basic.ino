#include <Ethernet.h>
#include "IoTWay.h"

//Put your MAC here
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE }; 

//Put your product symetric key here (AES)
byte key[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  };

IPAddress ip(192, 168, 10, 100);

void setup() 
{
  Serial.begin (9600);
  if (Ethernet.begin(mac) == 0) 
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  
  //Arguments in order : Proxy url, Port, ProductId, ProductToken (Short Token, 64 chars), Product Symetric Key declared above (AES)
  IoTWayInit ("proxy.iotway.net", 80, "arduinoBoardUniqueIdFromStudioIotWayNet", "ThisIsTheTokenForTheProductThatYouTakeFromStudioIotWayNetxxxxxxx", key);
}

void loop() 
{
  //example status sending
  //IoTWayStatus();

  //put delay because the server rejects requests that are made too often
  //delay (10000);

  //example signal sending
  //IoTWaySignal ("signalName", "Value");
  
  //put delay because the server rejects requests that are made too often
  //delay (10000);

  //example message sending
  //IoTWayMessage ("This message will be sent to server");
  
  //put delay because the server rejects requests that are made too often
  //delay (10000);

  //example message sending
  //IoTWayError ("This message will be sent to server as error");

  //put delay because the server rejects requests that are made too often
  //delay (10000);
}

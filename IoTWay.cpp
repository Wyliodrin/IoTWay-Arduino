#include "IoTWay.h"
#include <avr/pgmspace.h>
#include "AES.h"
#include "base64.h"
#include "MemoryFree.h"

AES aes;

static const char *server;
static unsigned short int port;
static const char *id;
static const char *token;
static const unsigned char *key;

const PROGMEM char formatSignalMessage[]  = "w:a\nt:%s\na:%s\nn:%ld\ns:\n%s";
const PROGMEM char formatStatusMessage[]  = "w:b\nt:%s\ns:%s\nn:%ld\ne:%d\nf:%d\nc:%d";
const PROGMEM char formatMessageMessage[] = "w:%c\nt:%s\nn:%ld\nm:%s"; //uses w:c and w:d
const PROGMEM char formatSignal[] = "%s %s:%s\n";
const PROGMEM char HTTP_HEADERS[] = "POST /exchange HTTP/1.1\nConnection: close\nContent-Type: text/plain\nUser-Agent: iotway-product\n";
const PROGMEM char HTTP_HOST[] = "Host: ";
const PROGMEM char HTTP_AUTHORIZATION_BEARER[] = "Authorization: Bearer ";
const PROGMEM char HTTP_CONTENT_LENGTH[] = "Content-Length: ";

char init_msg[] = "i";

#include <Ethernet.h>

EthernetClient client;

void print_P (const char *str)
{
  for (int i = 0; i < strlen_P (str); i++)
  {
    client.print ((char)pgm_read_byte_near(str + i));
  }
}

void IoTWayInit (const char *serverAddress, const char *productId, const char *accessToken, const unsigned char *encryptionKey)
{
  IoTWayInit (serverAddress, (unsigned short int)80, productId, accessToken, encryptionKey);
}

void IoTWayInit (const char *serverAddress, unsigned short int serverPort, const char *productId, const char *accessToken, const unsigned char *encryptionKey)
{
  server = serverAddress;
  port = serverPort;
  id = productId;
  token = accessToken;
  key = encryptionKey;
}

void IoTWayStatus ()
{
  
  byte iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  for(int i=0;i<N_BLOCK;i++){
    iv[i] = token[i];
  }
  
  char plainData[128];
  client.stop();

  // if there's a successful connection:
  #ifdef IOTWAY_DEBUG
  Serial.print ("Connecting to http://");
  Serial.print (server);
  Serial.print (":");
  Serial.println (port);
  #endif
  if (client.connect(server, port)) {
    // send the HTTP GET request:
    #ifdef IOTWAY_DEBUG
    Serial.println ("SUCCESS: connection");
    #endif
    print_P (HTTP_HEADERS);
    print_P (HTTP_HOST);
    client.println (server);
    print_P (HTTP_AUTHORIZATION_BEARER);
    client.println (id);

    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
      sprintf_P (plainData, formatStatusMessage, token, "on", (long)100, freeMemory(), (int)2048, (int)100);
    #elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
      sprintf_P (plainData, formatStatusMessage, token, "on", (long)100, freeMemory(), (int)8192, (int)100);
    #else
      //unknown
      sprintf_P (plainData, formatStatusMessage, token, "on", (long)100, freeMemory(), (int)2048, (int)100);
    #endif

    int encryptedSize = strlen (plainData) + (16 - (strlen (plainData) % 16));

    char encrypted[encryptedSize];
    char base64Data[2*encryptedSize];
    // aesLib.encrypt64(plainData, encrypted, key, iv);
    aes.do_aes_encrypt ((unsigned char *)plainData, strlen (plainData), encrypted, key, 128, iv);
    base64_encode (base64Data, encrypted, encryptedSize);
    Serial.println (base64Data);
    
    print_P (HTTP_CONTENT_LENGTH);
    client.println (strlen (base64Data));
    client.println();
    
    client.print (base64Data);
  } else {
    // if you couldn't make a connection:
    #ifdef IOTWAY_DEBUG
    Serial.println ("ERROR: connection");
    #endif
  }
}

void IoTWaySignal (const char *signalName, const char *value)
{
  IoTWaySignals (1, signalName, value);
}

void IoTWaySignals (const short int count, ...)
{
  char signalData[count*20];
  strcpy (signalData, "");
  va_list args;
  va_start(args, count);

  for (int j=0;j<count;j++)
  {
    Serial.print ("signal ");
    Serial.println (j+1);
    const char * signalName = va_arg (args, const char *);
    const char * signalValue = va_arg (args, const char *);
    sprintf_P (signalData, formatSignal, signalData, signalName, signalValue);
//    Serial.println (va_arg (args, const char *));
//    Serial.println (va_arg (args, double));
    Serial.println (signalData);
  }
 
  va_end(args);

  byte iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  for(int i=0;i<N_BLOCK;i++){
    iv[i] = token[i];
  }
  
  char plainData[strlen (signalData)+128];
  client.stop();

  // if there's a successful connection:
  #ifdef IOTWAY_DEBUG
  Serial.print ("Connecting to http://");
  Serial.print (server);
  Serial.print (":");
  Serial.println (port);
  #endif
  if (client.connect(server, port)) {
    // send the HTTP GET request:
    #ifdef IOTWAY_DEBUG
    Serial.println ("SUCCESS: connection");
    #endif
    print_P (HTTP_HEADERS);
    print_P (HTTP_HOST);
    client.println (server);
    print_P (HTTP_AUTHORIZATION_BEARER);
    client.println (id);
  
    sprintf_P (plainData, formatSignalMessage, token, "", (long)100, signalData);

    int encryptedSize = strlen (plainData) + (16 - (strlen (plainData) % 16));

    char encrypted[encryptedSize];
    char base64Data[2*encryptedSize];
    // aesLib.encrypt64(plainData, encrypted, key, iv);
    aes.do_aes_encrypt ((unsigned char *)plainData, strlen (plainData), encrypted, key, 128, iv);
    base64_encode (base64Data, encrypted, encryptedSize);
    Serial.println (base64Data);
    
    print_P (HTTP_CONTENT_LENGTH);
    client.println (strlen (base64Data));
    client.println();
    
    client.print (base64Data);
  } else {
    // if you couldn't make a connection:
    #ifdef IOTWAY_DEBUG
    Serial.println ("ERROR: connection");
    #endif
  }
}

void IoTWayMessage (const char *message){
  IoTWayMessage(message, IOTWAY_MESSAGE);
}

void IoTWayError (const char *message){
  IoTWayMessage(message, IOTWAY_ERROR);
}

void IoTWayMessage (const char *message, const int type){
  byte iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  for(int i=0;i<N_BLOCK;i++){
    iv[i] = token[i];
  }
  
  char plainData[128];
  client.stop();

  // if there's a successful connection:
  #ifdef IOTWAY_DEBUG
  Serial.print ("Connecting to http://");
  Serial.print (server);
  Serial.print (":");
  Serial.println (port);
  #endif

  if (client.connect(server, port)) {
    // send the HTTP GET request:
    #ifdef IOTWAY_DEBUG
    Serial.println ("SUCCESS: connection");
    #endif
    print_P (HTTP_HEADERS);
    print_P (HTTP_HOST);
    client.println (server);
    print_P (HTTP_AUTHORIZATION_BEARER);
    client.println (id);

    char t;
    if (type == IOTWAY_ERROR){
      t = 'd';
    }
    else {
      t = 'c';
    }
    sprintf_P (plainData, formatMessageMessage, t, token, (long)100, message);
    

    int encryptedSize = strlen (plainData) + (16 - (strlen (plainData) % 16));

    char encrypted[encryptedSize];
    char base64Data[2*encryptedSize];
    // aesLib.encrypt64(plainData, encrypted, key, iv);
    aes.do_aes_encrypt ((unsigned char *)plainData, strlen (plainData), encrypted, key, 128, iv);
    base64_encode (base64Data, encrypted, encryptedSize);
    Serial.println (base64Data);
    
    print_P (HTTP_CONTENT_LENGTH);
    client.println (strlen (base64Data));
    client.println();
    
    client.print (base64Data);
  } else {
    // if you couldn't make a connection:
    #ifdef IOTWAY_DEBUG
    Serial.println ("ERROR: connection");
    #endif
  }
}


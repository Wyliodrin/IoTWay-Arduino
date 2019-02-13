#ifndef _IOTWAY__H
#define _IOTWAY__H

#define IOTWAY_DEBUG
#define IOTWAY_ERROR 2
#define IOTWAY_MESSAGE 1

void IoTWayMessage (const char *message);
void IoTWayError (const char *message);
void IoTWayMessage (const char *message, const int type);

void IoTWayInit (const char *serverAddress, const char *productId, const char *accessToken, const unsigned char *encryptionKey);
void IoTWayInit (const char *serverAddress, unsigned short int serverPort, const char *productId, const char *accessToken, const unsigned char *encryptionKey);
void IoTWayStatus ();
void IoTWaySignal (const char *signalName, const char *value);
void IoTWaySignals (const short int count, ...);

#endif


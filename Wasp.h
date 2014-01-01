#ifndef Wasp_h
#define Wasp_h

#include "HardwareSerial.h"
#include "crc16_ccitt.h"

class Wasp {

public:
  Wasp(HardwareSerial *serial);
  Wasp(HardwareSerial *serial, int timeout);
  void begin(long baudrate);
  void sendMessage(unsigned char *content, int length);
  int readMessage(unsigned char *buffer, int bufSize);

private:
  void writeContent(unsigned char *content, int length);
  void writeCrc(unsigned char *content, int length);
  void writeByte(unsigned char b);
  int checkCrc(unsigned char *content, int length);
  crc_t crc16(unsigned char *content, int length);

  HardwareSerial *serial;
  int timeout;
};

#endif


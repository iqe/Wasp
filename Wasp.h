#ifndef Wasp_h
#define Wasp_h

#include "HardwareSerial.h"
#include "crc16_ccitt.h"

class Wasp {

public:
  Wasp(HardwareSerial *serial);
  Wasp(HardwareSerial *serial, int timeout);
  void begin(int baudrate);
  void sendMessage(char *content, int length);
  int readMessage(char *buffer, int bufSize);

private:
  void writeContent(char *content, int length);
  void writeCrc(char *content, int length);
  void writeByte(uint8_t b);
  int checkCrc(char *content, int length);
  crc_t crc16(char *content, int length);

  HardwareSerial *serial;
  int timeout;
};

#endif


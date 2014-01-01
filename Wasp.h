#ifndef Wasp_h
#define Wasp_h

#include "HardwareSerial.h"
#include "crc16_ccitt.h"

class Wasp {

public:
  Wasp(HardwareSerial *serial);
  Wasp(HardwareSerial *serial, int timeout);
  void begin(long baudrate);
  void sendMessage(uint8_t *content, int length);
  int readMessage(uint8_t *buffer, int bufSize);

private:
  void writeContent(uint8_t *content, int length);
  void writeCrc(uint8_t *content, int length);
  void writeByte(uint8_t b);
  int checkCrc(uint8_t *content, int length);
  crc_t crc16(uint8_t *content, int length);

  HardwareSerial *serial;
  int timeout;
};

#endif


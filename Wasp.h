#ifndef Wasp_h
#define Wasp_h

#include "HardwareSerial.h"

class Wasp {

public:
  Wasp(HardwareSerial *serial);
  void sendMessage(char *content, int length);

private:
  void writeContent(char *content, int length);
  void writeCrc(char *content, int length);

  HardwareSerial *serial;
};

#endif


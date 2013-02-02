#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wasp.h"
#include "crc16_ccitt.h"

#define WASP_DEFAULT_TIMEOUT 100

#define WASP_SFLAG   0x5B // '['
#define WASP_EFLAG   0x5D // ']'
#define WASP_ESC     0x5C // '\'
#define WASP_ESC_XOR 0x20 // ' '

Wasp::Wasp(HardwareSerial *serial) {
  this->serial = serial;
  this->timeout = WASP_DEFAULT_TIMEOUT;
}

Wasp::Wasp(HardwareSerial *serial, int timeout) {
  this->serial = serial;
  this->timeout = timeout;
}

void Wasp::sendMessage(char *content, int length) {
  serial->write(WASP_SFLAG);
  writeContent(content, length);
  writeCrc(content, length);
  serial->write(WASP_EFLAG);
}

void Wasp::begin(int baudrate) {
  serial->begin(baudrate);
}

int Wasp::readMessage(char *buffer, int bufsize) {
  char c;
  int contentLength = 0;
  bool inMessage, afterEsc = false;
  long lastByteTimestamp = millis();

  while (contentLength <= bufsize) {
    if (millis() - lastByteTimestamp > timeout) {
      return -1;
    }

    if (serial->available() > 0) {
      c = serial->read();
      lastByteTimestamp = millis();

      switch (c) {
        case -1:
          return -1;
        case WASP_SFLAG:
          inMessage = true;
          contentLength = 0;
          break;
        case WASP_EFLAG:
          if (inMessage) {
            return contentLength;
          }
          inMessage = false;
          break;
        case WASP_ESC:
          afterEsc = true;
          break;
        default:
          if (inMessage) {
            if (afterEsc) {
              c ^= WASP_ESC_XOR;
              afterEsc = false;
            }
            buffer[contentLength++] = c;
          }
      }
    }
  }

  return -2;
}

void Wasp::writeContent(char *content, int length) {
  char c;

  for (int i = 0; i < length; i++) {
    c = content[i];
    if (c == WASP_SFLAG || c == WASP_EFLAG || c == WASP_ESC) {
      serial->write(WASP_ESC);
      serial->write(c ^ WASP_ESC_XOR);
    } else {
      serial->write(c);
    }
  }
}

void Wasp::writeCrc(char *content, int length) {
  crc_t crc = crc16(content, length);
  uint8_t high = (crc >> 8);
  uint8_t low = (uint8_t)crc;

  serial->write(high);
  serial->write(low);
}

crc_t Wasp::crc16(char *content, int length) {
  crc_t crc = crc_init();
  crc = crc_update(crc, (unsigned char *)content, length);
  crc = crc_finalize(crc);

  return crc;
}


#include "Arduino.h"
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

void Wasp::sendMessage(uint8_t *content, int length) {
  serial->write(WASP_SFLAG);
  writeContent(content, length);
  writeCrc(content, length);
  serial->write(WASP_EFLAG);
}

void Wasp::begin(long baudrate) {
  serial->begin(baudrate);
}

int Wasp::readMessage(uint8_t *buffer, int bufsize) {
  int c;
  int contentLength = 0;
  bool inMessage = false, afterEsc = false;
  unsigned long lastByteTimestamp = millis();

  while (contentLength <= bufsize) {
    if (millis() - lastByteTimestamp > timeout) {
      return -3;
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
            return checkCrc(buffer, contentLength);
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

void Wasp::writeContent(uint8_t *content, int length) {
  for (int i = 0; i < length; i++) {
    writeByte(content[i]);
  }
}

void Wasp::writeCrc(uint8_t *content, int length) {
  crc_t crc = crc16(content, length);
  uint8_t high = highByte(crc);
  uint8_t low = lowByte(crc);

  writeByte(low);
  writeByte(high);
}

void Wasp::writeByte(uint8_t b) {
  if (b == WASP_SFLAG || b == WASP_EFLAG || b == WASP_ESC) {
    serial->write(WASP_ESC);
    serial->write(b ^ WASP_ESC_XOR);
  } else {
    serial->write(b);
  }
}

int Wasp::checkCrc(uint8_t *content, int length) {
  if (length < 2) { // too small for crc
    return -1;
  }

  uint16_t low = content[length - 2];
  uint16_t high = content[length - 1];

  crc_t expectedCrc = (high << 8) | low;
  crc_t actualCrc = crc16(content, length - 2);

  return expectedCrc == actualCrc ? length - 2 : -1;
}

crc_t Wasp::crc16(uint8_t *content, int length) {
  crc_t crc = crc_init();
  crc = crc_update(crc, content, length);
  crc = crc_finalize(crc);

  return crc;
}


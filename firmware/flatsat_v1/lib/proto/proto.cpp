#include <proto.h>

void serialPrintUint8Hex(uint8_t *packet, uint16_t packet_length) {
  for (int i = 0; i < packet_length + 1; ++i) {
    Serial.printf("%02X ", packet[i]);
    if ((i + 1) % 16 == 0)
      Serial.println();
  }
  if ((packet_length + 1) % 16 != 0)
    Serial.println();
}

void printHexDump(const uint8_t *data, size_t len) {
  const size_t bytesPerLine = 32;
  char ascii[bytesPerLine + 1];
  ascii[bytesPerLine] = '\0';

  for (size_t i = 0; i < len; i++) {
    if (i % bytesPerLine == 0) {
      if (i != 0) {
        Serial.print("  |");
        Serial.println(ascii);
      }
      Serial.printf("%08X  ", (unsigned int)i);
    }

    Serial.printf("%02X ", data[i]);
    ascii[i % bytesPerLine] = (data[i] >= 32 && data[i] <= 126) ? data[i] : '.';
  }

  size_t remaining = len % bytesPerLine;
  if (remaining > 0) {
    for (size_t i = remaining; i < bytesPerLine; i++) {
      Serial.print("   ");
    }
  }

  Serial.print("  |");
  ascii[remaining == 0 ? bytesPerLine : remaining] = '\0';
  Serial.println(ascii);
}

void printStringHexDump(const String &input) {
  const size_t bytesPerLine = 32;
  char ascii[bytesPerLine + 1];
  ascii[bytesPerLine] = '\0';

  size_t len = input.length();
  const char *data = input.c_str();

  for (size_t i = 0; i < len; i++) {
    if (i % bytesPerLine == 0) {
      if (i != 0) {
        Serial.print("  |");
        Serial.println(ascii);
      }
      Serial.printf("%08X  ", (unsigned int)i);
    }

    Serial.printf("%02X ", (uint8_t)data[i]);
    ascii[i % bytesPerLine] = (data[i] >= 32 && data[i] <= 126) ? data[i] : '.';
  }

  size_t remaining = len % bytesPerLine;
  if (remaining > 0) {
    for (size_t i = remaining; i < bytesPerLine; i++) {
      Serial.print("   ");
    }
  }

  Serial.print("  |");
  ascii[remaining == 0 ? bytesPerLine : remaining] = '\0';
  Serial.println(ascii);
}

void floatToBigEndian(float value, uint8_t *bytes) {
  proto_float_t temp;
  temp.f = value;
  // reverse bytes
  bytes[0] = temp.bytes[3]; // MSB
  bytes[1] = temp.bytes[2];
  bytes[2] = temp.bytes[1];
  bytes[3] = temp.bytes[0]; // LSB
}
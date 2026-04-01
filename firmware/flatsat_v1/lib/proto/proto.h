#ifndef __PROTO_H
#define __PROTO_H
#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#define LORA_PACKET_LEN 255
#define MAX_PAYLOAD_CHUNK 128

typedef union {
  float f;
  unsigned char bytes[4];
} proto_float_t;

typedef enum { SENSOR_FAIL = -1, SENSOR_OK = 0 } sensor_status_t;

enum {
  TM_SENSOR_STATUS_OK = 0x00,
  TM_SENSOR_STATUS_ERR = 0x01,
};

void serialPrintUint8Hex(uint8_t *packet, uint16_t packet_length);
void printHexDump(const uint8_t *data, size_t len);
void printStringHexDump(const String &input);
void floatToBigEndian(float value, uint8_t *bytes);
#endif

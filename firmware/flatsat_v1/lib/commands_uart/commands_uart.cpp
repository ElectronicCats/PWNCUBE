#include <SerialCommand.h>
#include <apid.h>
#include <commands_list.h>
#include <commands_uart.h>
#include <radio_wrapper.h>
#include <spp.h>

#define BINARY_START_BYTE 0x7E
#define MAX_PACKET_SIZE 256

SerialCommand SCmd;

static void processBinary(uint8_t *data, uint8_t len) {
  uint8_t offset = 0;
  uint8_t segment = data[offset++];
  space_packet_t packet;
  spp_tc_build_packet(&packet, segment, SPP_SECHEAD_FLAG_NOPRESENT, 0,
                      APID_TM_IMAGE, data, len);
  // radioTransmit((uint8_t*)&packet,
  //               (SPP_PRIMARY_HEADER_LEN + packet.header.length));

  radioTransmitToModem((uint8_t *)&packet,
                       (SPP_PRIMARY_HEADER_LEN + packet.header.length));
}

static void handleBinary() {
  uint8_t start = Serial.read(); // 0x7E
  uint8_t len = Serial.read();   // len
  uint8_t payload[MAX_PACKET_SIZE];
  int i = 0;
  while (i < len && Serial.available()) {
    payload[i++] = Serial.read();
  }
  if (i == len) {
    processBinary(payload, len);
    Serial.write(0x06);
    Serial.println("");
  }
}

void commandUARTHandler(void) { handleBinary(); }

static void serialDefaultHandler(const char *arg) { Serial.println("Unknown"); }

void commandUARTInit(void) { SCmd.setDefaultHandler(serialDefaultHandler); }
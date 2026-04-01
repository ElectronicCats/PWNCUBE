#include <apid.h>
#include <bme_wrapper.h>
#include <mission.h>
#include <mpu_wrapper.h>
#include <radio_wrapper.h>
#include <spp.h>
#include <workers.h>

typedef struct {
  const unsigned long inverval;
  unsigned long previous;
} timeout_worker_t;

static timeout_worker_t t_tm_data = {.inverval = 10000, .previous = 0};

static timeout_worker_t t_tc_idle = {.inverval = 15000, .previous = 0};

static timeout_worker_t t_tc_ping = {.inverval = 30000, .previous = 0};

static void telemetryGetData(void) {
  uint8_t buffer[MAX_PAYLOAD_CHUNK];
  int offset = 0;
  buffer[offset++] = SPACECRAFT_ID;
  bmeReadData(buffer, offset);
  mpuReadData(buffer, offset);

  buffer[offset++] = '\0';

  space_packet_t space_packet;
  int ret = spp_tm_build_packet(&space_packet, SPP_GROUP_FLAG_UNSEGMENTED,
                                SPP_SECHEAD_FLAG_NOPRESENT, 0, APID_TM_SEND_TM,
                                buffer, offset);

  if (ret != SPP_ERROR_NONE) {
    Serial.print("[SYS - TM] ERROR SENSOR Packet: ");
    Serial.println(ret);
    return;
  }
  radioTransmit((uint8_t *)&space_packet,
                (SPP_PRIMARY_HEADER_LEN + space_packet.header.length));

  radioTransmitToModem((uint8_t *)&space_packet,
                       (SPP_PRIMARY_HEADER_LEN + space_packet.header.length));
}

void telemetryConfigureSensors(void) {
  bmeInit();
  mpuInit();
}

void telemetryWorker(void) {
  if (millis() - t_tm_data.previous > t_tm_data.inverval) {
    t_tm_data.previous = millis();
    telemetryGetData();
  }
}

void telemetryPingToGsWorker(void) {
  if (millis() - t_tc_ping.previous > t_tc_ping.inverval) {
    t_tc_ping.previous = millis();
    const uint8_t buffer[8] = {SPACECRAFT_ID, 0x70, 0x77, 0x6e,
                               0x73,          0x61, 0x74, 0x00};
    space_packet_t space_packet;
    int ret = spp_tm_build_packet(&space_packet, SPP_GROUP_FLAG_UNSEGMENTED,
                                  SPP_SECHEAD_FLAG_NOPRESENT, 0,
                                  APID_TC_PING_SYNC, buffer, 8);

    if (ret != SPP_ERROR_NONE) {
      Serial.print("[SYS - TM] ERROR PING Packet: ");
      Serial.println(ret);
      return;
    }
    radioTransmit((uint8_t *)&space_packet,
                  (SPP_PRIMARY_HEADER_LEN + space_packet.header.length));
  }
}

void telemetryIdleWorker(void) {
  if (millis() - t_tc_idle.previous > t_tc_idle.inverval) {
    t_tc_idle.previous = millis();
    space_packet_t space_packet;
    int ret = spp_idle_build_packet(&space_packet);
    if (ret != SPP_ERROR_NONE) {
      Serial.print("[SYS - TM] ERROR IDLE Packet: ");
      Serial.println(ret);
      return;
    }
    radioTransmit((uint8_t *)&space_packet,
                  (SPP_PRIMARY_HEADER_LEN + space_packet.header.length));
  }
}
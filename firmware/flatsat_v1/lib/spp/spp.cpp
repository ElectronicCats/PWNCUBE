/**
 * @file spp.c
 * @brief Space Packet Protocol
 *
 * @author Pwnsat
 * @date 2025-08-7
 */
#include <proto.h>
#include <spp.h>

typedef struct {
  uint16_t tc;
  uint16_t tm;
} packet_counter_t;

static packet_counter_t packet_counter = {.tc = 0, .tm = 0};

void spp_print_packet_details(space_packet_t *packet) {
  uint16_t version = (packet->header.identification >> 13) & 0x7;
  uint16_t type = (packet->header.identification >> 11) & 0x1;
  uint16_t sec_header = (packet->header.identification >> 10) & 0x1;
  uint16_t apid = packet->header.identification & 0x7FF;

  uint16_t seq_flags = (packet->header.sequence >> 14) & 0x3;
  uint16_t seq_count = packet->header.sequence & 0x3FFF;

  Serial.println("=== Space Packet Header ===");
  Serial.printf(" Version:             %u\n", version);
  Serial.printf(" Type:                %02X\n", type);
  Serial.printf(" Secondary Header:    %u\n", sec_header);
  Serial.printf(" APID:                0x%04X\n", apid);
  Serial.printf(" Sequence Flags:      0x%X (%s)\n", seq_flags,
                seq_flags == SPP_GROUP_FLAG_UNSEGMENTED ? "Unsegmented"
                : seq_flags == SPP_GROUP_FLAG_START     ? "Start"
                : seq_flags == SPP_GROUP_FLAG_CONT      ? "Continuation"
                                                        : "End");

  Serial.printf(" Sequence Count:      %u\n", seq_count);
  Serial.printf(" Data Length:         %u\n", packet->header.length);

  Serial.println("=== Payload Dump (Hex) ===");
  serialPrintUint8Hex(packet->data, packet->header.length);
}

static void spp_validate_counters() {
  if (packet_counter.tc == 16383) {
    packet_counter.tc = 0;
  }
  if (packet_counter.tm == 16383) {
    packet_counter.tm = 0;
  }
}

static int spp_build_packet(space_packet_t *space_packet, uint8_t type,
                            uint8_t flag, uint8_t sec_header,
                            uint16_t sec_header_len, uint16_t apid,
                            uint16_t sequence_count, const uint8_t *data,
                            uint16_t data_len) {

  if (data_len > SPP_MAX_PAYLOAD_CHUNK) {
    return -1;
  }

  memset(space_packet, 0, sizeof(space_packet_t));

  /* htons -> for better compatibilites to MSB */
  space_packet->header.identification = (CCSDS_SPP_VERSION << 13) |
                                        (type << 11) | (sec_header << 10) |
                                        (apid & 0x7FF);
  space_packet->header.sequence = (flag << 14) | (sequence_count & 0x3FFF);

  if (sec_header == SPP_SECHEAD_FLAG_PRESENT && sec_header_len == 0) {
    return SPP_ERROR_MALFORMED_SEC_HEADER;
  }

  space_packet->header.length = data_len + sec_header_len - 1;

  if (data_len > 0 && data != NULL) {
    memcpy(space_packet->data, data, data_len);
  }

  return SPP_ERROR_NONE;
}

int spp_tc_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len) {
  spp_validate_counters();
  packet_counter.tc++;
  return spp_build_packet(space_packet, SPP_PTYPE_TC, flag, sec_header,
                          sec_header_len, apid, packet_counter.tc, data,
                          data_len);
}

int spp_tm_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len) {
  spp_validate_counters();
  packet_counter.tm++;
  return spp_build_packet(space_packet, SPP_PTYPE_TM, flag, sec_header,
                          sec_header_len, apid, packet_counter.tm, data,
                          data_len);
}

int spp_idle_build_packet(space_packet_t *space_packet) {
  const uint16_t buffer_len = 9;
  const uint8_t buffer_idle[buffer_len] = {0x65, 0x6b, 0x6f, 0x70, 0x61,
                                           0x72, 0x74, 0x79, 0x00};
  return spp_build_packet(
      space_packet, SPP_PTYPE_TM, SPP_GROUP_FLAG_UNSEGMENTED,
      SPP_SECHEAD_FLAG_NOPRESENT, 0, SPP_APID_IDLE, 0, buffer_idle, buffer_len);
}

int spp_unpack_packet(space_packet_t *space_packet, const uint8_t *buffer,
                      uint16_t buffer_len) {
  if (buffer_len < SPP_PRIMARY_HEADER_LEN) {
    return SPP_ERROR_PACKET_LEN;
  }
  if (buffer == NULL) {
    return SPP_ERROR_INVALID_BUFFER;
  }

  memset(space_packet, 0, sizeof(space_packet_t));
  space_packet->header.identification = (buffer[1] << 8) | buffer[0];

  uint8_t version = (space_packet->header.identification << 13) & 0x07;
  if (version != CCSDS_SPP_VERSION) {
    return SPP_ERROR_VERSION;
  }

  space_packet->header.sequence = (buffer[3] << 8) | buffer[2];
  space_packet->header.length = (buffer[5] << 8) | buffer[4];

  if (space_packet->header.length > SPP_MAX_PAYLOAD_CHUNK) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  // if (space_packet->header.length > buffer_len - SPP_PRIMARY_HEADER_LEN) {
  // return SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS;}

  if (space_packet->header.length > 0 && buffer != NULL) {
    memcpy(space_packet->data, buffer + SPP_PRIMARY_HEADER_LEN,
           space_packet->header.length + 1);
  }
  return SPP_ERROR_NONE;
}
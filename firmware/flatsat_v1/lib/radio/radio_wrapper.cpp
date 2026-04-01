#include <proto.h>
#include <radio_wrapper.h>

#define SX_NSS_1 2
#define SX_BUSY_1 38
#define SX_NSS_2 3
#define SX_BUSY_2 39
#define SX_DIO1 11

/* Sender - This radio don't support the FSK feature, the have
a lot of problems and dropped packets - Use this as TM
*/
SX1262 uplink = new Module(SX_NSS_1, SX_BUSY_1, -1, -1);
/* Receiver - This work so well, support FSK and LoRa - Use this as TC*/
SX1262 downlink = new Module(SX_NSS_2, SX_DIO1, -1, SX_BUSY_2);

typedef struct {
  float frequency;
  float bandwidth;
  uint8_t spreadingFactor;
  uint8_t preambleLength;
  uint8_t codingRate;
  bool active = false;
} radio_config_t;

static radio_config_t uplink_cfg;
static radio_config_t downlink_cfg;

static radioPacketReceivedCb radi_recv_cb = NULL;

volatile bool receivedFlag = false;
volatile bool enableInterruptRadio = true;

static void radio_received_flag(void) {
  if (!enableInterruptRadio) {
    return;
  }
  receivedFlag = true;
}

static void radioSetInitialConfig(void) {
  uplink_cfg.frequency = 916;
  uplink_cfg.bandwidth = 250;
  uplink_cfg.spreadingFactor = 12;
  uplink_cfg.preambleLength = 8;
  uplink_cfg.codingRate = 5;
  uplink_cfg.active = false;

  downlink_cfg.frequency = 918;
  downlink_cfg.bandwidth = 250;
  downlink_cfg.spreadingFactor = 12;
  downlink_cfg.preambleLength = 8;
  downlink_cfg.codingRate = 5;
  downlink_cfg.active = false;
}

static void radioConfigureFSK() {
  Serial.println("[SYS] Radio Uplink init");
  int state = uplink.beginFSK(916, 4.8, 5, 156.2, 10, 16, 1.6, false);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("[SYS] Radio Uplink OK");
    uplink_cfg.active = true;
    uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uplink.setSyncWord(syncWord, 8);
    uplink.setCRC(2, 0xFFFF, 0x8005, false); // CRC igual al receptor
  } else {
    Serial.print("[SYS] Radio Uplink Error: ");
    Serial.println(state);
    uplink_cfg.active = false;
  }
}

void radioConfigure() {
  Serial.println("[SYS] Radio Uplink init");
  int state = uplink.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("[SYS] Radio Uplink OK");
    uplink_cfg.active = true;
  } else {
    Serial.print("[SYS] Radio Uplink Error: ");
    Serial.println(state);
    uplink_cfg.active = false;
  }
  if (uplink_cfg.active) {
    uplink.setFrequency(916);
    uplink.setBandwidth(250);
    uplink.setSpreadingFactor(12);
    uplink.setPreambleLength(8);
    uplink.setCodingRate(5);
    // uplink.setOutputPower(-11);
    uplink.setOutputPower(12);
  }

  delay(500);

  Serial.println("[SYS] Radio Downlink init");
  state = downlink.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("[SYS] Radio Downlink OK");
    downlink_cfg.active = true;
  } else {
    Serial.print("[SYS] Radio Downlink Error: ");
    Serial.println(state);
    downlink_cfg.active = false;
  }
  if (downlink_cfg.active) {
    downlink.setFrequency(918);
    downlink.setBandwidth(250);
    downlink.setSpreadingFactor(12);
    downlink.setPreambleLength(8);
    downlink.setCodingRate(5);
    downlink.setPacketReceivedAction(radio_received_flag);
    downlink.startReceive();
  }
}

void radioCheckPacketReceived(void) {
  if (!downlink_cfg.active) {
    return;
  }
  if (receivedFlag) {
    receivedFlag = false;
    enableInterruptRadio = false;

    int recvLen = downlink.getPacketLength();
    byte byteArr[recvLen];
    int state = downlink.readData(byteArr, recvLen);
    if (state == RADIOLIB_ERR_NONE) {
      if (radi_recv_cb != NULL) {
        radi_recv_cb(byteArr, recvLen);
      } else {
        Serial.print("[SYS - Radio] Recv: ");
        Serial.write(byteArr, recvLen);
        Serial.println();
        Serial.print("[SYS - Radio] RSSI: ");
        Serial.println(downlink.getRSSI());
        Serial.print("[SYS - Radio] SNR: ");
        Serial.println(downlink.getSNR());
      }
    } else {
      Serial.print("[SYS - Radio] Recv Error: ");
      Serial.println(state);
    }

    downlink.startReceive();
    enableInterruptRadio = true;
  }
}

void radioTransmitToModem(uint8_t *buffer, uint16_t buffer_len) {
  if (!uplink_cfg.active) {
    return;
  }
  uplink.setFrequency(925);
  delay(500);
  int state = uplink.transmit(buffer, buffer_len);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[SYS - Radio] Transmit Error: ");
    Serial.println(state);
  }
  delay(500);
  uplink.setFrequency(916);
}

void radioTransmit(uint8_t *buffer, uint16_t buffer_len) {
  if (!uplink_cfg.active) {
    return;
  }
  int state = uplink.transmit(buffer, buffer_len);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[SYS - Radio] Transmit Error: ");
    Serial.println(state);
  }
}

void radioRegisterCb(radioPacketReceivedCb recv_cb) { radi_recv_cb = recv_cb; }

void radioPackConfig(uint8_t *buffer, int &offset) {
  proto_float_t freq_b;
  proto_float_t band_b;

  freq_b.f = uplink_cfg.frequency;
  band_b.f = uplink_cfg.bandwidth;

  memcpy(buffer + offset, freq_b.bytes, 4);
  offset += 4;
  memcpy(buffer + offset, band_b.bytes, 4);
  offset += 4;
  buffer[offset++] = uplink_cfg.codingRate;
  buffer[offset++] = uplink_cfg.spreadingFactor;
  buffer[offset++] = uplink_cfg.preambleLength;
}
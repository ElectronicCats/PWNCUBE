#ifndef __RADIO_H
#define __RADIO_H
#include <RadioLib.h>

typedef void (*radioPacketReceivedCb)(uint8_t *buffer, uint16_t buffer_len);

void radioConfigure();
void radioCheckPacketReceived(void);
void radioTransmitToModem(uint8_t *buffer, uint16_t buffer_len);
void radioTransmit(uint8_t *buffer, uint16_t buffer_len);
void radioRegisterCb(radioPacketReceivedCb recv_cb);
void radioPackConfig(uint8_t *buffer, int &offset);
#endif

/**
 * @file apid.h
 * @brief Space Packet Protocol Mission APIDs
 *
 * @author Pwnsat
 * @date 2025-08-7
 */
#ifndef __APID_H
#define __APID_H

#define APID_TC_BROADCAST 10

/* APID sended from spacecraft */
#define APID_TM_SENSORS 8
#define APID_TM_BME 6
#define APID_TM_MPU 7
// This is the spacecraft sender ack ping

#define APID_TC_SEND_CONFIG 320

/* APID recv from spacecraft */
#define APID_TC_GET_CONFIG 300
#define APID_TC_SET_FREQ 301
// APIDS with response
#define APID_TC_PING_SYNC 200
#define APID_TC_GET_STATUS 100
#define APID_TC_GET_TEMP 200
#define APID_TC_GET_GYRO 300
#define APID_TC_GET_TM 400
// APID TC Recv file
#define APID_TC_FIRMWARE_UPDATE 66
#define APID_TM_IMAGE 10
// APID FOR ROOT stuff
#define APID_TC_SET_ROOT 73

// APID TM for command response
#define APID_TM_PING_ACK 13     // Response <- APID_TC_PING_SYNC
#define APID_TM_SEND_STATUS 101 // Response <- APID_TC_GET_STATUS
#define APID_TM_SEND_TEMP 6     // Response <- APID_TC_GET_TEMP
#define APID_TM_SEND_GYRO 7     // Response <- APID_TC_GET_GYRO
#define APID_TM_SEND_TM 8       // Response <- APID_TC_GET_TM
#define APID_TM_FILE_ACK 453    // Response <- APID_TC_FIRMWARE_UPDATE

#define APID_TM_ERROR01 400 // Format error
#define APID_TM_ERROR02 404 // APID not found

#endif
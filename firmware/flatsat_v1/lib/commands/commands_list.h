#ifndef __COMMANDS_LISTS_H
#define __COMMANDS_LISTS_H

#define CMD_PING_SYNC_LEN 6
#define CMD_PING_ACK_LEN 8
#define GROUND_STICK_ID 0x7C

#define CMD_PING "send_ping"
#define CMD_STATUS "get_status"
#define CMD_GETTEMP "get_temp"
#define CMD_GETGYRO "get_gyro"
#define CMD_GETTM "get_tm"

#define CMD_SEND_IMAGE "send_image"
#define CMD_SEND "send"

#define CMD_RES_PING 0x22
#define CMD_RES_STATUS 0x21
#define CMD_RES_GETTEMP 0x24
#define CMD_RES_GETGYRO 0x25
#define CMD_RES_GETTM 0x23

#endif
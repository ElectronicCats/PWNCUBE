#ifndef __MPU_WRAPPER_H
#define __MPU_WRAPPER_H

#include <Adafruit_MPU6050.h>
#include <proto.h>

sensor_status_t mpuInit();
sensor_status_t mpuReadData(uint8_t *buffer, int &offset);
sensor_status_t mpuReadEuler(uint8_t *buffer, int &offset);

#endif

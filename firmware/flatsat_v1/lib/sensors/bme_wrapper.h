#ifndef __BME_WRAPPER_H
#define __BME_WRAPPER_H

#include <Adafruit_BME280.h>
#include <proto.h>

sensor_status_t bmeInit();
sensor_status_t bmeReadData(uint8_t *buffer, int &offset);

#endif

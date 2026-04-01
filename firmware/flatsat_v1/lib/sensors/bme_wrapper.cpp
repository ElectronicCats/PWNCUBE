#include "bme_wrapper.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

static sensor_status_t bme_status = SENSOR_OK;

sensor_status_t bmeInit() {
  Serial.println("[SYS - BME] Init");
  if (!bme.begin(0x76)) {
    Serial.println("[SYS - BME] Failed to find BME280 chip");
    Serial.print("[SYS - BME] SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    bme_status = SENSOR_FAIL;
    return bme_status;
  }
  Serial.println("[SYS - BME] BME280 Found!");
  bme_status = SENSOR_OK;
  return bme_status;
}

sensor_status_t bmeReadData(uint8_t *buffer, int &offset) {
  if (bme_status == SENSOR_FAIL) {
    buffer[offset++] = TM_SENSOR_STATUS_ERR;
    return bme_status;
  }
  buffer[offset++] = TM_SENSOR_STATUS_OK;

  floatToBigEndian(bme.readTemperature(), buffer + offset);
  offset += 4;
  floatToBigEndian(bme.readPressure() / 100.0F, buffer + offset);
  offset += 4;
  floatToBigEndian(bme.readAltitude(SEALEVELPRESSURE_HPA), buffer + offset);
  offset += 4;
  floatToBigEndian(bme.readHumidity(), buffer + offset);
  offset += 4;
  return bme_status;
}
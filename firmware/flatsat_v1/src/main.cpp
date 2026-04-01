#include <Arduino.h>
#include <bme_wrapper.h>
#include <commands.h>
#include <commands_uart.h>
#include <mission.h>
#include <mpu_wrapper.h>
#include <radio_wrapper.h>
#include <workers.h>

void setup() {
  Serial.begin(115200);

  Serial.flush();
  commandUARTInit();
  radioConfigure();
  telemetryConfigureSensors();
  radioRegisterCb(commandHandler);
}

void loop() {
  commandUARTHandler();
  radioCheckPacketReceived();
  telemetryWorker();
  telemetryIdleWorker();
}
#include "mpu_wrapper.h"

Adafruit_MPU6050 mpu;

static sensor_status_t mpu_status = SENSOR_OK;

float roll = 0.0;
float pitch = 0.0;
float yaw = 0.0;

// Variables para el filtro complementario
float alpha =
    0.98; // Ponderación del giroscopio (0.98) frente al acelerómetro (0.02)
unsigned long lastTime = 0;

sensor_status_t mpuInit() {
  Serial.println("[SYS - MPU] MPU6050 Init");
  if (!mpu.begin(0x69) || !mpu.begin(0x68)) {
    Serial.println("[SYS - MPU] Failed to find MPU6050 chip");
    mpu_status = SENSOR_FAIL;
    return mpu_status;
  }
  Serial.println("[SYS - MPU] MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("[SYS - MPU] Accelerometer range set to: +-8G");
  Serial.println("[SYS - MPU] Gyro range set to: +- 500 deg/s");
  Serial.println("[SYS - MPU] Filter bandwidth set to: 21 Hz");
  mpu_status = SENSOR_OK;
  // Wait for sensor to stabilize
  delay(100);
  return mpu_status;
}

sensor_status_t mpuReadData(uint8_t *buffer, int &offset) {
  sensors_event_t a, g, temp;
  if (!mpu.getEvent(&a, &g, &temp)) {
    buffer[offset++] = TM_SENSOR_STATUS_ERR;
    return SENSOR_FAIL;
  }
  buffer[offset++] = TM_SENSOR_STATUS_OK;
  floatToBigEndian(a.acceleration.x, buffer + offset);
  offset += 4;
  floatToBigEndian(a.acceleration.y, buffer + offset);
  offset += 4;
  floatToBigEndian(a.acceleration.z, buffer + offset);
  offset += 4;
  floatToBigEndian(g.gyro.x, buffer + offset);
  offset += 4;
  floatToBigEndian(g.gyro.y, buffer + offset);
  offset += 4;
  floatToBigEndian(g.gyro.z, buffer + offset);
  offset += 4;
  floatToBigEndian(temp.temperature, buffer + offset);
  offset += 4;
  return SENSOR_OK;
}

sensor_status_t mpuReadEuler(uint8_t *buffer, int &offset) {
  sensors_event_t a, g, temp;
  if (!mpu.getEvent(&a, &g, &temp)) {
    buffer[offset++] = TM_SENSOR_STATUS_ERR;
    return SENSOR_FAIL;
  }
  buffer[offset++] = TM_SENSOR_STATUS_OK;

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0; // Convertir a segundos
  lastTime = currentTime;

  // Calcular roll y pitch desde el acelerómetro (en grados)
  float accel_roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float accel_pitch =
      atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y +
                                    a.acceleration.z * a.acceleration.z)) *
      180 / PI;

  // Calcular velocidades angulares del giroscopio (en rad/s)
  float gyro_roll = g.gyro.x;  // rad/s
  float gyro_pitch = g.gyro.y; // rad/s
  float gyro_yaw = g.gyro.z;   // rad/s

  // Filtro complementario para roll y pitch
  roll = alpha * (roll + gyro_roll * dt) + (1 - alpha) * accel_roll;
  pitch = alpha * (pitch + gyro_pitch * dt) + (1 - alpha) * accel_pitch;

  // Integrar el giroscopio para yaw
  yaw += gyro_yaw * dt * 180 / PI; // Convertir a grados

  Serial.print("Roll: ");
  Serial.println(roll);
  floatToBigEndian(roll, buffer + offset);
  offset += 4;
  floatToBigEndian(pitch, buffer + offset);
  offset += 4;
  floatToBigEndian(yaw, buffer + offset);
  offset += 4;

  return SENSOR_OK;
}
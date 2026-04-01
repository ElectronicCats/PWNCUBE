/* Test firmware for BME280 & MPU6050
  PWNSAT Project 2025
*/

#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define FIRMWARE_VERSION "0.0.1"
#define LED_PIN D0
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_MPU6050 mpu;
Adafruit_BME280 bme;

static int initMPU() {
  Serial.println("[MPU] MPU6050 Init");
  if (!mpu.begin(0x69)) {
    Serial.println("[MPU] Failed to find MPU6050 chip");
    return -1;
  }
  Serial.println("[MPU] MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("[MPU] Accelerometer range set to: +-8G");
  Serial.println("[MPU] Gyro range set to: +- 500 deg/s");
  Serial.println("[MPU] Filter bandwidth set to: 21 Hz");
  return 0;
}

static void readMPU() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
  Serial.println("");
}

static int initBME() {
  Serial.println("BME280 Init");
  if (!bme.begin(0x76)) {
    Serial.println("[MPU] Failed to find BME280 chip");
    Serial.print("[MPU] SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    return -1;
  }
  Serial.println("[MPU] MPU6050 Found!");
  return 0;
}

static void readBME() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println("");
}

static void initLED() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

static void bootLED() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
  }
}

static void blinkLED() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(500);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(500);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Sensor Test - " FIRMWARE_VERSION);

  initLED();
  bootLED();

  initMPU();
  initBME();

  delay(100);
}

void loop() {
  Serial.println("==========MPU==========");
  readMPU();
  blinkLED();

  Serial.println("==========BME==========");
  readBME();
  blinkLED();
}
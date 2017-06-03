#ifdef IS_ESP8266
#include <ESP8266WiFi.h>
#else
#include <Arduino.h>
#endif

#define VERSION "v0.1.0"
#define PRODUCT "PulsationsKit"

#define LED_PIN_ERROR 0
#define LED_PIN_ERROR_PULLUP true
#define LED_PIN_ACTIVITY 2
#define LED_PIN_ACTIVITY_PULLUP true

#define TARGET_FPS 30.0

#include <WiFiUdp.h>

#include "Logger.h"
#include "StatusManager.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include <OSCMessage.h>
#include <OSCTiming.h>

typedef struct {
    uint8_t system;
    uint8_t gyro;
    uint8_t accel;
    uint8_t mag;
} bno_calibration_t;

typedef struct {
    float x;
    float y;
    float z;
} sensor_data_float3d_t;

Adafruit_BNO055       _device;
bno_calibration_t _calibration;

WiFiUDP _out_udp;
IPAddress _ip;
IPAddress _ipBcast;
IPAddress _ipServer;
StatusManager _statusManager;

long lastFrameTime;
long lastCalibrationUpdate;
long frameDuration;
char oscaddr[255];
OSCMessage _msg;
osctime_t time;
sensors_event_t _event;
imu::Vector<3> accel;
imu::Vector<3> euler;

void setup() {
  delay(100);

  _statusManager.setup(true);
  frameDuration = (long)(1000.0 / TARGET_FPS);

  if (USE_SERIAL) {
      Serial.begin(SERIAL_RATE);
      Serial.println();
      Serial.println((char *) "-------------------------");
      Serial.println((char *) PRODUCT);
      Serial.println((char *) VERSION);
      Serial.println();
  }

  char log_msg[128];
  sprintf(log_msg, "Connecting to SSID: %s", "pulsations");
  Logger::info(log_msg);

  WiFi.begin("pulsations", "pulsations");
  uint8_t wifi_wait_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    _statusManager.error(true);
    wifi_wait_count += 1;
    if (wifi_wait_count == 8) {
        Logger::error((char *) "WiFi connection failed.");
    }
    delay(100);
    _statusManager.error(false);
  }

  _ip = WiFi.localIP();
  _ipBcast = WiFi.localIP();
  _ipBcast[3] = 255;
  _ipServer[3] = 2;

  Logger::info((char *) "Connected!");

  sprintf(log_msg, "IPADD: %u.%u.%u.%u", _ip[0], _ip[1], _ip[2], _ip[3]);
  Logger::debug(log_msg);
  sprintf(log_msg, "BCAST: %u.%u.%u.%u", _ipBcast[0], _ipBcast[1], _ipBcast[2], _ipBcast[3]);
  Logger::debug(log_msg);
  sprintf(log_msg, "RSSI: %ddBm       ", WiFi.RSSI());
  Logger::debug(log_msg);

  sprintf(oscaddr, "/bno055/%u", _ip[3]);

  _device = Adafruit_BNO055(0x28);
  bool _active = _device.begin();
  while (!_active) {
    Logger::debug((char *) "Waiting for BNO055 on 0x28");
    delay(10);
    _statusManager.error(true);
    _active = _device.begin();
  }
  Logger::debug((char *) "BNO055 active on 0x28");
  _device.setExtCrystalUse(true);
  _statusManager.error(false);

  lastCalibrationUpdate = millis();
  _device.getCalibration(&(_calibration.system), &(_calibration.gyro),
    &(_calibration.accel), &(_calibration.mag));
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    _statusManager.error(true);
    Logger::error((char *) "WiFi disconnected, reconnecting...");
    delay(100);
    _statusManager.error(false);
    return;
  }

  lastFrameTime = millis();

  accel = _device.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  euler = _device.getVector(Adafruit_BNO055::VECTOR_EULER);

  if (lastFrameTime - lastCalibrationUpdate >= 1000) {
    lastCalibrationUpdate = lastFrameTime;
    _device.getCalibration(&(_calibration.system), &(_calibration.gyro),
      &(_calibration.accel), &(_calibration.mag));
  }

  _statusManager.error(!_device.isFullyCalibrated());

  _msg.empty();
  _msg.setAddress(oscaddr);

  time.seconds = (long)(millis() * 0.001);
  time.fractionofseconds = (long)(millis() % 1000);
  _msg.add(time);

  _msg.add((float)euler.x());
  _msg.add((float)euler.y());
  _msg.add((float)euler.z());

  _msg.add((float)accel.x());
  _msg.add((float)accel.y());
  _msg.add((float)accel.z());

  uint8_t cal_blob[] = {
    _calibration.system,
    _calibration.gyro,
    _calibration.accel,
    _calibration.mag
  };
  _msg.add(cal_blob, 4);

  _statusManager.activity(true);
  _out_udp.beginPacket(_ipBcast, 8888);
  _msg.send(_out_udp);
  _out_udp.endPacket();
  _statusManager.activity(false);

  if (millis() - lastFrameTime <= frameDuration) {
    _statusManager.error(false);
    delay(frameDuration - (millis() - lastFrameTime));
  } else {
    _statusManager.error(true);
  }
}

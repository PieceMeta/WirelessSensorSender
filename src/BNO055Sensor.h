#ifndef __WSS_BNO055_SENSOR_H__
#define __WSS_BNO055_SENSOR_H__

#include <ESP8266WiFi.h>
#include "ConfigManager.h"
#include "Timing.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define USE_EXTERNAL_CRYSTAL true

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

class BNO055Sensor {
public:
    void setup(ConfigManager *config, Timing *timing, uint8_t i2c_port, uint8_t i2c_address) {
        _config = config;
        _timing = timing;

        _sensor_type = (char *) "BNO055";
        _i2c_address = i2c_address;
        _i2c_port = i2c_port;

        char buffer[16];
        sprintf(buffer, "/%s/%02u/%02x", _sensor_type,
          _i2c_port, _i2c_address);
        _osc_address = buffer;

        _last_update = 0;
        _active = false;
    }

    boolean initialize() {
        _device = Adafruit_BNO055(_i2c_address);
        _active = _device.begin();
        if (_active) {
            _device.setExtCrystalUse(USE_EXTERNAL_CRYSTAL);
        }
        return _active;
    }

    void update() {
        if (!_active) return;
        if (_config->getSend()->orientation) {
          sensors_event_t _event;
          _device.getEvent(&_event);
          _orientation.x = _event.orientation.x;
          _orientation.y = _event.orientation.y;
          _orientation.z = _event.orientation.z;
        }
        if (_config->getSend()->accel) {
            imu::Vector<3> accel = _device.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
            _acceleration.x = accel.x();
            _acceleration.y = accel.y();
            _acceleration.z = accel.z();
        }
        if (_config->getSend()->temp) {
            _temperature = _device.getTemp();
        }
        if (_config->getSend()->calibration &&
          _timing->calibrationNeedsUpdate(_last_update)) {

          _last_update = millis();
          _device.getCalibration(&(_calibration.system), &(_calibration.gyro),
            &(_calibration.accel), &(_calibration.mag));
        }
    }

    sensor_data_float3d_t *getOrientation() {
        return &_orientation;
    }

    sensor_data_float3d_t *getAcceleration() {
        return &_acceleration;
    }

    sensor_data_float3d_t *getGyroscope() {
        return &_gyroscope;
    }

    sensor_data_float3d_t *getMagnetometer() {
        return &_magnetometer;
    }

    int8_t getTemperature() {
        return _temperature;
    }

    bno_calibration_t *getCalibration() {
        return &_calibration;
    }

    uint8_t getPort() {
      return _i2c_port;
    }

    uint8_t getI2CAddress() {
      return _i2c_address;
    }

    char *getSensorType() {
      return _sensor_type;
    }

    String getOscAddress() {
      return _osc_address;
    }

    boolean isActive() {
        return _active;
    }

    void setActive(boolean active) {
      _active = active;
    }

private:
    ConfigManager         *_config;
    Timing                *_timing;
    Adafruit_BNO055       _device;

    boolean               _active;
    long                  _last_update;
    char*                 _sensor_type;
    String                _osc_address;
    uint8_t               _i2c_address;
    uint8_t               _i2c_port;
    sensor_data_float3d_t _orientation;
    sensor_data_float3d_t _acceleration;
    sensor_data_float3d_t _gyroscope;
    sensor_data_float3d_t _magnetometer;
    int8_t                _temperature;
    bno_calibration_t     _calibration;
};

#endif

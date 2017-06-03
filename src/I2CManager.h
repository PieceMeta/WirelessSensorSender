#ifndef __WSS_I2C_MANAGER_H__
#define __WSS_I2C_MANAGER_H__

#include <ESP8266WiFi.h>
#include <Wire.h>

extern "C" {
#include "twi.h"
}

#include "ConfigManager.h"
#include "Logger.h"
#include "Timing.h"
#include "BNO055Sensor.h"

#define NUM_I2C_PORTS 1
#define NUM_I2C_ADDRESSES 128
#define NUM_SENSORS_PER_PORT 2

typedef struct {
    boolean active;
    int num;
    BNO055Sensor sensors[NUM_SENSORS_PER_PORT];
} i2c_port_t;

class I2CManager {
public:
    void setup(ConfigManager *config, Timing *timing) {
        _config = config;
        _timing = timing;
        _current_port = 255;
        Wire.begin();
    }

    int getPortCount() {
        return NUM_I2C_PORTS;
    }

    int getSensorCount(int port_index) {
        return NUM_SENSORS_PER_PORT;
    }

    i2c_port_t *getPort(uint8_t port_index) {
        if (port_index != _current_port && _config->getSensor()->use_tca) tcaSelectPort(port_index);
        _current_port = port_index;
        return &_ports[port_index];
    }

    BNO055Sensor *getSensor(uint8_t port_index, uint8_t sensor_index) {
        return &(getPort(port_index)->sensors[sensor_index]);
    }

    void tcaSelectPort(uint8_t port_index) {
        if (port_index >= NUM_I2C_PORTS) return;
        Wire.beginTransmission(_config->getSensor()->tca_i2c_address);
        Wire.write(1 << port_index);
        Wire.endTransmission();
    }

    void scanI2C() {
        Logger::info((char *) "I2C port scan...");
        for (uint8_t p = 0; p < NUM_I2C_PORTS; p++) {
            getPort(p);
            for (uint8_t addr = 0; addr < NUM_I2C_ADDRESSES; addr++) {
                if (_config->getSensor()->use_tca &&
                    addr == _config->getSensor()->tca_i2c_address)
                    continue;

                uint8_t data;
                if (!twi_writeTo(addr, &data, 0, 1)) {
                    int d = 0;
                    while (d < NUM_SENSORS_PER_PORT && getSensor(p, d)->isActive()) d += 1;
                    if (d >= NUM_SENSORS_PER_PORT) continue;

                    getSensor(p, d)->setup(_config, _timing, p, addr);
                    if (getSensor(p, d)->initialize()) {
                        getPort(p)->active = true;
                        char log_msg[64];
                        sprintf(log_msg, "BNO055 at 0x%02x on port %d", addr, p);
                        Logger::info(log_msg);
                    }
                }
            }
        }
    }

    boolean update() {
        for (int p = 0; p < getPortCount(); p++) {
            if (!getPort(p)->active) continue;

            for (int d = 0; d < getSensorCount(p); d++) {
                if (!getSensor(p, d)->isActive()) continue;
                getSensor(p, d)->update();
            }
        }
        return true;
    }


private:
    ConfigManager *_config;
    Timing *_timing;
    i2c_port_t _ports[NUM_I2C_PORTS];
    uint8_t _current_port;
};

#endif

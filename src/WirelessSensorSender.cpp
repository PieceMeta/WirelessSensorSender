#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ConfigManager.h"
#include "Logger.h"
#include "Timing.h"
#include "I2CManager.h"
#include "WiFiManager.h"
#include "BNO055Sensor.h"
#include "OscManager.h"
#include "StatusManager.h"

#define VERSION "v0.1.0"
#define PRODUCT "WirelessSensorSender"

ConfigManager config;
Timing timing;
I2CManager i2c;
WiFiManager wifi;
OscManager osc;
StatusManager status;

void setup() {
    config.load();

    if (USE_SERIAL) {
        Serial.begin(config.getSys()->serial_rate);
        delay(100);
        Serial.println();
        Serial.println();
    }

    Logger::info((char *) PRODUCT);
    Logger::info((char *) VERSION);

    i2c.setup(&config, &timing);

    status.setup(config.getSys()->use_led);

    wifi.setup(&config, &status);
    while (!wifi.update()) {
        delay(WIFI_RECONNECT_DELAY_MS);
    }

    i2c.scanI2C();

    timing.setup(&config, &wifi);
    timing.update();

    osc.setup(&config, &wifi);
}

void loop() {
    if (!wifi.update()) {
        delay(WIFI_RECONNECT_DELAY_MS);
        return;
    }

    timing.update();
    timing.beginFrame();
    if (i2c.update()) {
        for (uint8_t p = 0; p < i2c.getPortCount(); p++) {
            if (!i2c.getPort(p)->active) continue;
            for (uint8_t d = 0; d < i2c.getSensorCount(p); d++) {
                BNO055Sensor *sensor = i2c.getSensor(p, d);
                if (!sensor->isActive()) continue;

                osc.initMessage(sensor->getOscAddress().c_str(), timing.getNtpTime());

                if (config.getSend()->orientation) {
                  osc.addFloat(sensor->getOrientation()->x);
                  osc.addFloat(sensor->getOrientation()->y);
                  osc.addFloat(sensor->getOrientation()->z);
                }

                if (config.getSend()->accel) {
                  osc.addFloat(sensor->getAcceleration()->x);
                  osc.addFloat(sensor->getAcceleration()->y);
                  osc.addFloat(sensor->getAcceleration()->z);
                }

                if (config.getSend()->temp) {
                  osc.addInt(sensor->getTemperature());
                }

                if (config.getSend()->calibration) {
                  uint8_t cal_blob[] = {
                    sensor->getCalibration()->system,
                    sensor->getCalibration()->gyro,
                    sensor->getCalibration()->accel,
                    sensor->getCalibration()->mag
                  };
                  osc.addBlob(cal_blob, 4);
                }

                osc.send(wifi.getBroadcastIp(), config.getOsc()->send_port);
            }
        }
    }

    timing.endFrame();
    if (timing.getFrameDuration() <= config.getSys()->data_update_ms) {
        status.error(false);
        delay(config.getSys()->data_update_ms - timing.getFrameDuration());
    } else {
        status.error(true);
        if (LOG_LEVEL == Logger::LOG_LEVEL_DEBUG) {
            char log_msg[64];
            sprintf(log_msg, "Long send: %dms", timing.getFrameDuration());
            Logger::debug(log_msg);
        }
    }
}

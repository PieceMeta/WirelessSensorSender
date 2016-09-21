#ifndef __WSS_WI_FI_MANAGER_H__
#define __WSS_WI_FI_MANAGER_H__

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "StatusManager.h"
#include "ConfigManager.h"
#include "Logger.h"

#define WIFI_RECONNECT_DELAY_MS 200
#define WIFI_RECONNECT_MAX_DELAYS 20

class WiFiManager {
public:
    void setup(ConfigManager *config, StatusManager *status) {
        _config = config;
        _status = status;
    }

    boolean update() {
        if (WiFi.status() != WL_CONNECTED) {
            _status->error(true);
            if (connectWiFi(_config->getWiFi()->ssid, _config->getWiFi()->key)) {
                _status->error(false);
            } else {
                return false;
            }
        }
        return true;
    }

    IPAddress getBroadcastIp() {
        return _ipBcast;
    }

    WiFiUDP *getOutUDP() {
      return &_out_udp;
    }

    void networkActivity(boolean active) {
      _status->activity(active);
    }

private:
    IPAddress _ip;
    IPAddress _ipBcast;
    WiFiUDP _out_udp;
    StatusManager *_status;
    ConfigManager *_config;

    boolean connectWiFi(char *ssid, char *key) {
        char log_msg[128];
        sprintf(log_msg, "Connecting to SSID: %s", ssid);
        Logger::info(log_msg);

        WiFi.begin(ssid, key);
        uint8_t wifi_wait_count = 0;
        while (WiFi.status() != WL_CONNECTED) {
            wifi_wait_count += 1;
            if (wifi_wait_count == WIFI_RECONNECT_MAX_DELAYS) {
                Logger::error((char *) " WiFi connection failed.");
                return false;
            }
            Logger::log((char *) ".", Logger::LOG_LEVEL_INFO, false);
            delay(WIFI_RECONNECT_DELAY_MS);
        }

        _ip = WiFi.localIP();
        _ipBcast = WiFi.localIP();
        _ipBcast[3] = 255;

        Logger::info((char *) " Connected!");

        sprintf(log_msg, "IPADD: %u.%u.%u.%u", _ip[0], _ip[1], _ip[2], _ip[3]);
        Logger::debug(log_msg);
        sprintf(log_msg, "BCAST: %u.%u.%u.%u", _ipBcast[0], _ipBcast[1], _ipBcast[2], _ipBcast[3]);
        Logger::debug(log_msg);
        sprintf(log_msg, "RSSI (dBm): %d    ", WiFi.RSSI());
        Logger::debug(log_msg);

        return true;
    }
};

#endif

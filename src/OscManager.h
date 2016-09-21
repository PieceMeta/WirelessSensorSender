#ifndef __WSS_OSC_MANAGER_H__
#define __WSS_OSC_MANAGER_H__

#include <OSCMessage.h>
#include <OSCTiming.h>

#include "ConfigManager.h"
#include "WiFiManager.h"

class OscManager {
public:
    void setup(ConfigManager *config, WiFiManager *wifi) {
        _config = config;
        _wifi = wifi;
    }

    void initMessage(const char *address, ntp_time_t ntp_time) {
      osctime_t timetag = ntpToOscTime(ntp_time);
      _msg.empty();
      _msg.setAddress(address);
      _msg.add(timetag);
    }

    osctime_t ntpToOscTime(ntp_time_t ntp_time) {
      osctime_t osctime;
      osctime.seconds = ntp_time.seconds;
      osctime.fractionofseconds = ntp_time.fraction;
      return osctime;
    }

    void addFloat(float value) {
        _msg.add(value);
    }

    void addInt(int32_t value) {
        _msg.add(value);
    }

    void addBlob(uint8_t *value, uint8_t length) {
        _msg.add(value, length);
    }

    void send(IPAddress target_ip, uint16_t target_port) {
        if (_msg.size() > 1) {
            _wifi->networkActivity(true);
            _wifi->getOutUDP()->beginPacket(target_ip, target_port);
            _msg.send(*_wifi->getOutUDP());
            _wifi->getOutUDP()->endPacket();
            _wifi->networkActivity(false);
        }
    }

private:
    ConfigManager   *_config;
    WiFiManager     *_wifi;

    OSCMessage      _msg;
};

#endif

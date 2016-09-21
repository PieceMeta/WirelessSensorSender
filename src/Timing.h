#ifndef __WSS_TIMING_H__
#define __WSS_TIMING_H__

#include <ESP8266WiFi.h>

#include "NTPClient.h"
#include "RTClib.h"
#include "WiFiManager.h"

class Timing {
public:
    void setup(ConfigManager *config, WiFiManager *wifi) {
        _config = config;
        _wifi = wifi;
        _rtc.begin(DateTime(0));
        if (_config->getSys()->use_ntp) {
            _timeClient.setup(_wifi, (char *) NTP_SERVER);
            _timeClient.begin();
        }
    }

    void update() {
      if (_config->getSys()->use_ntp) {
        if (_timeClient.update()) {
          ntp_time_t *epoch = _timeClient.getLastNtpEpoch();
          _rtc.adjust(epoch->seconds);
          if (LOG_LEVEL < Logger::LOG_LEVEL_INFO) return;
          char buff[64];
          sprintf(buff, "NTP update: %u.%u", epoch->seconds, epoch->fraction);
          Logger::info(buff);
        }
      }
    }

    void beginFrame() {
        _frame_start_ms = millis();
    }

    void endFrame() {
        _frame_duration = millis() - _frame_start_ms;
    }

    boolean calibrationNeedsUpdate(uint32_t last_update_ms) {
        return getFrameStart() - last_update_ms > _config->getSys()->calibration_update_ms;
    }

    uint32_t getFrameStart() {
        return _frame_start_ms;
    }

    uint32_t getFrameDuration() {
        return _frame_duration;
    }

    ntp_time_t getNtpTime() {
      ntp_time_t *epoch = _timeClient.getLastNtpEpoch();
      ntp_time_t ntp_time = {
          _rtc.now().unixtime(),
          epoch->fraction
      };
      return ntp_time;
    }

private:
    ConfigManager   *_config;
    WiFiManager     *_wifi;

    NTPClient       _timeClient;
    RTC_Millis      _rtc;
    uint32_t        _frame_start_ms;
    uint32_t        _frame_duration;
};

#endif

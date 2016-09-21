#ifndef __WSS_CONFIG_MANAGER_H__
#define __WSS_CONFIG_MANAGER_H__

#include "WSSDefaults.h"

typedef struct {
  int             data_update_ms        = UPDATE_DATA_INTERVAL_MS;
  int             calibration_update_ms = UPDATE_CALIBRATION_INTERVAL_MS;
  long            serial_rate           = SERIAL_RATE;
  boolean         use_ntp               = USE_NTP;
  boolean         use_led               = USE_LED;
  boolean         use_activity_led      = USE_ACTIVITY_LED;
} wss_config_system_t;

typedef struct {
  char*           ssid                  = (char *) WIFI_DEFAULT_SSID;
  char*           key                   = (char *) WIFI_DEFAULT_KEY;
} wss_config_wifi_t;

typedef struct {
  int             listen_port           = LISTEN_OSC_UDP_PORT;
  int             send_port             = SEND_OSC_UDP_PORT;
} wss_config_osc_t;

typedef struct {
  boolean         use_tca               = USE_TCA;
  uint8_t         tca_i2c_address       = TCA_ADDR;
} wss_config_sensor_t;

typedef struct {
  boolean         orientation           = SEND_ORIENTATION;
  boolean         accel                 = SEND_ACCELERATION;
  boolean         gyro                  = SEND_GYROSCOPE;
  boolean         magnet                = SEND_MAGNET;
  boolean         temp                  = SEND_TEMPERATURE;
  boolean         calibration           = SEND_CALIBRATION;
} wss_config_send_t;

class ConfigManager
{
public:
  void load() {
    // stub
  }

  void save() {
    // stub
  }

  wss_config_system_t* getSys() {
    return &_sys;
  }

  wss_config_wifi_t* getWiFi() {
    return &_wifi;
  }

  wss_config_osc_t* getOsc() {
    return &_osc;
  }

  wss_config_sensor_t* getSensor() {
    return &_sensor;
  }

  wss_config_send_t* getSend() {
    return &_send;
  }

private:
  wss_config_system_t _sys;
  wss_config_wifi_t _wifi;
  wss_config_osc_t _osc;
  wss_config_sensor_t _sensor;
  wss_config_send_t _send;
};

#endif

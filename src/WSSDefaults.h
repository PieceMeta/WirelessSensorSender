#ifndef __WSS_USER_DEFAULTS_H__
#define __WSS_USER_DEFAULTS_H__

#define LOG_LEVEL 0   // Logger::LOG_LEVEL_INFO

//
//
// User configuration defaults

#define USE_LED true
#define USE_ACTIVITY_LED true

#define USE_SERIAL true
#define SERIAL_RATE 115200

#define UPDATE_DATA_INTERVAL_MS 20
#define UPDATE_CALIBRATION_INTERVAL_MS 500

#define USE_NTP false
#define NTP_SERVER "pool.ntp.org"

#define USE_TCA false
#define TCA_ADDR 0x70

#define WIFI_DEFAULT_SSID "AndroidAP5"
#define WIFI_DEFAULT_KEY "butterscotch66"

#define SEND_OSC_UDP_PORT 8888
#define LISTEN_OSC_UDP_PORT 4444

#define SEND_ORIENTATION true
#define SEND_ACCELERATION true
#define SEND_GYROSCOPE false
#define SEND_TEMPERATURE false
#define SEND_MAGNET false
#define SEND_CALIBRATION true

#endif

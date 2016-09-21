#ifndef __WSS_LOGGER_H__
#define __WSS_LOGGER_H__

#include <ESP8266WiFi.h>
#include "WSSDefaults.h"

class Logger {
public:
    static const uint8_t LOG_LEVEL_DEBUG = 0;
    static const uint8_t LOG_LEVEL_INFO = 1;
    static const uint8_t LOG_LEVEL_WARN = 2;
    static const uint8_t LOG_LEVEL_ERROR = 3;

    static void log(char *msg, uint8_t level, boolean printLine) {
        if (level >= LOG_LEVEL) {
            if (USE_SERIAL) {
                if (printLine) {
                    Serial.println(msg);
                } else {
                    Serial.print(msg);
                }
            }
        }
    }

    static void debug(char *msg) {
        log(msg, LOG_LEVEL_DEBUG, true);
    }

    static void info(char *msg) {
        log(msg, LOG_LEVEL_DEBUG, true);
    }

    static void warn(char *msg) {
        log(msg, LOG_LEVEL_DEBUG, true);
    }

    static void error(char *msg) {
        log(msg, LOG_LEVEL_DEBUG, true);
    }
};

#endif

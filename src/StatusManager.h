#ifndef __WSS_STATUS_MANAGER_H__
#define __WSS_STATUS_MANAGER_H__

#include <ESP8266WiFi.h>

#define LED_PIN_ERROR 0
#define LED_PIN_ERROR_PULLUP true
#define LED_PIN_ACTIVITY 2
#define LED_PIN_ACTIVITY_PULLUP true

class StatusManager {
public:
    void setup(boolean use_led) {
        _use_led = use_led;
        if (_use_led) {
            pinMode(LED_PIN_ERROR, OUTPUT);
            digitalWrite(LED_PIN_ERROR, LED_PIN_ERROR_PULLUP ? HIGH : LOW);
            pinMode(LED_PIN_ACTIVITY, OUTPUT);
            digitalWrite(LED_PIN_ACTIVITY, LED_PIN_ACTIVITY_PULLUP ? HIGH : LOW);
        }
    }

    void error(boolean active) {
        if (_use_led && active != _error) {
          _error = active;
          setLED(LED_PIN_ERROR, active, LED_PIN_ERROR_PULLUP);
        }
    }

    void activity(boolean active) {
        if (_use_led && active != _activity) {
          _activity = active;
          setLED(LED_PIN_ACTIVITY, active, LED_PIN_ACTIVITY_PULLUP);
        }
    }

private:
    boolean _use_led;
    boolean _error;
    boolean _activity;

    void setLED(uint8_t pin, boolean active, boolean pullup) {
        if (active) {
            digitalWrite(pin, pullup ? LOW : HIGH);
        } else {
            digitalWrite(pin, pullup ? HIGH : LOW);
        }
    }
};

#endif

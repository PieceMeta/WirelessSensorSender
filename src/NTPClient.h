/**
 * The MIT License (MIT)
 * Copyright (c) 2015 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "WiFiManager.h"
#include "Logger.h"

#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337

#define NTP_PORT 123
#define UPDATE_NTP_INTERVAL_MS 10000
#define UPDATE_NTP_TIMEOUT_MS 250
#define UPDATE_NTP_TIMEOUT_INTERVAL_MS 5

typedef struct {
  uint32_t seconds;
  uint32_t fraction;
} ntp_time_t;

class NTPClient {
public:
  void setup(WiFiManager *wifi, char* ntp_server) {
    _wifi = wifi;
    _ntp_server = ntp_server;
  }

  void begin() {
    _last_update = 0;
    _udp.begin(NTP_DEFAULT_LOCAL_PORT);
  }

  bool update() {
    if (millis() - _last_update >= UPDATE_NTP_INTERVAL_MS) {
      _wifi->networkActivity(true);
      sendNTPPacket();
      uint8_t timeout = 0;
      int cb = 0;
      do {
        delay(UPDATE_NTP_TIMEOUT_INTERVAL_MS);
        cb = _udp.parsePacket();
        if (timeout > UPDATE_NTP_TIMEOUT_MS / UPDATE_NTP_TIMEOUT_INTERVAL_MS) {
          Logger::warn((char *) "NTP update timeout");
          return false;
        }
        timeout++;
      } while (cb == 0);
      _last_update = millis() - (UPDATE_NTP_TIMEOUT_INTERVAL_MS * (timeout + 1));
      _udp.read(_buffer, NTP_PACKET_SIZE);
      _wifi->networkActivity(false);

      uint16_t high = word(_buffer[40], _buffer[41]);
      uint16_t low = word(_buffer[42], _buffer[43]);
      uint16_t high_frac = word(_buffer[44], _buffer[45]);
      uint16_t low_frac = word(_buffer[46], _buffer[47]);

      _last_ntp_epoch.seconds = (high << 16 | low) - 2208988800UL;
      _last_ntp_epoch.fraction = high_frac << 16 | low_frac;

      return true;
    }
    return false;
  }

  ntp_time_t *getLastNtpEpoch() {
    return &_last_ntp_epoch;
  }

  void end() {
    _udp.stop();
  }

private:
  WiFiManager   *_wifi;
  WiFiUDP       _udp;
  uint8_t       _buffer[NTP_PACKET_SIZE];
  char*         _ntp_server;
  uint32_t      _last_update;
  ntp_time_t    _last_ntp_epoch;

  void sendNTPPacket() {
    memset(_buffer, 0, NTP_PACKET_SIZE);
    _buffer[0] = 0b11100011;
    _buffer[1] = 0;
    _buffer[2] = 6;
    _buffer[3] = 0xEC;
    _buffer[12]  = 49;
    _buffer[13]  = 0x4E;
    _buffer[14]  = 49;
    _buffer[15]  = 52;
    _wifi->networkActivity(true);
    _udp.beginPacket((const char*) NTP_SERVER, NTP_PORT);
    _udp.write(_buffer, NTP_PACKET_SIZE);
    _udp.endPacket();
    _wifi->networkActivity(false);
  }
};

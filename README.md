# Wireless Sensor Sender #

Transmits IMU Fusion data read from multiple [BNO055 Sensors](https://www.adafruit.com/product/2472) wired to an [ESP8266 WiFi module](https://www.adafruit.com/products/2471) via an [I2C multiplexer](https://www.adafruit.com/products/2717). Data is timestamped and sent as OSC messages over UDP broadcast.

## Building the project ##

Install [PlatformIO](http://platformio.org/) IDE or CLI to build the project and upload it to a microcontroller.

First clone the libraries:
```shell
git submodule init
git submodule update
```

Then build with PlatformIO CLI:
```shell
platformio run --target=upload
```

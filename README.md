# MotorOnARoller
 This is my implementation of Motor on a Roller from [nidayand](https://github.com/nidayand/motor-on-roller-blind-ws) and [thehookup](https://github.com/thehookup/motor-on-roller-blind-ws). I rewrote most of the code as a mean to teach myself arduino, so that this code is split among many files defining what's on each file. I also took the oportunity to add some functionalities.

[Main Screen](https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/main_screen.png)

This git has the following functionalities:
- Dynamic Single and Dual motor control
- BME280 and BMP280 chip support
- Automatic detection and configuration under home assistant (using [MQTT discovery](https://www.home-assistant.io/docs/mqtt/discovery/))
- WebSocket and MQTT support
- Full webpage support for the dual motors, temperature and humidity


I use a ESP8266 Wemos D1 mini 

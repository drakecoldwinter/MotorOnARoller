# MotorOnARoller
 This is my implementation of Motor on a Roller from [nidayand](https://github.com/nidayand/motor-on-roller-blind-ws) and [thehookup](https://github.com/thehookup/motor-on-roller-blind-ws). I rewrote most of the code as a mean to teach myself arduino, so that this code is split among many files defining what's on each file. I also took the oportunity to add some functionalities. As for the motor I implemented the AccelStepper library wich is very impressive.

![Main Screen](https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/main_screen.png)

This project has the following functionalities:
- Dynamic Single and Dual motor control
- BME280 and BMP280 chip support
- Automatic detection and configuration under home assistant (using [MQTT discovery](https://www.home-assistant.io/docs/mqtt/discovery/))
- WebSocket and MQTT support
- Configuration saved on power loss
- Full webpage support for the dual motors, temperature and humidity
- OTA support

# Hardware Wiring
I use a ESP8266 Wemos D1 mini for this project. It has less avalaible pins than other ESP8266 or ESP32 chips. To be able to use dual motors and BME280 chip I did reconfigure the TX and RX pins, this means the chip will not have serial capabilities, hence you will not be able to do serial monitor to it. While you test I suggest you set to false the useBME280Sensor variable so you can use the serial monitor.

I connected pins D1, D2, D3, D4 to motor 1 and pins D5, D6, D7 and D8 to motor 2. The bme280 is connected to pins TX and RX.

Also you need to use a 9V power supply for the motors, I suggest you use a 7805 chip to lower the voltage and power up the ESP.

# MQTT and Home Assistant
![MQTT](https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/MQTT.png)

Upon starting a complete hierachy will be created on MQTT. If you have activated MQTT discovery on home assistant, then you don't need to do anything else, under devices you should see the ESP appear and the covers as well as the sensors will be avalaible. Be aware that the BME280 sensor is probed once each 2 minutes, so upon booting the information may not be avalaible. I also implemented a status sensor that will render all sensors unavalaible if the connection to the ESP is lost. 

![HA](https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/ha_screen.png)

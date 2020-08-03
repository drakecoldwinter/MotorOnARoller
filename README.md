# MotorOnARoller
 This is my implementation of Motor on a Roller from [nidayand](https://github.com/nidayand/motor-on-roller-blind-ws) and [thehookup](https://github.com/thehookup/motor-on-roller-blind-ws). I rewrote most of the code as a mean to teach myself arduino, so that this code is split among many files defining what's on each file. I also took the oportunity to add some functionalities. As for the motor I implemented the AccelStepper library wich is very impressive and modified the code to use 1 or 2 motors deppending on your needs. The webpage will dynamically change depending if you activated dual motors and if you used a BME280 or a BMP280 sensor. The BME280 is sampled once each 2 minutes, so upon booting the information may not be avalaible.
 
<img src="https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/main_screen.png" width="500">

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
<img src="https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/MQTT.png" width="300">

Upon starting a complete hierachy will be created on MQTT. If you have activated MQTT discovery on home assistant, then you don't need to do anything else, under devices you should see the ESP appear and the covers as well as the sensors will be avalaible. I also implemented a network status sensor that will render all sensors unavalaible (using Last Will on MQTT) if the connection to the ESP is lost. The main purpose is that you will not need to change nor configure anything under home assistant, everything should simply work. Of course you can still use the code without home assistant with MQTT Payloads.

<img src="https://github.com/drakecoldwinter/MotorOnARoller/blob/master/images/ha_screen.png" width="300">

# Required libraries (3rd party)
- AccelStepper: https://www.airspayce.com/mikem/arduino/AccelStepper/
- PubSubClient: https://github.com/knolleary/pubsubclient/
- ArduinoJson v5: https://github.com/bblanchon/ArduinoJson
- WIFIManager: https://github.com/tzapu/WiFiManager
- WbSocketsServer: https://github.com/Links2004/arduinoWebSockets
- BME280I2C : https://github.com/finitespace/BME280


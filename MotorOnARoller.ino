#include <AccelStepper.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "index_html.h"
#include <BME280I2C.h>
#include <Wire.h>

WiFiClient espClient;
bool shouldSaveConfig = false;      //Used for WIFI Manager callback to save parameters
char config_name[40] ="Rho2";         //WIFI config: Bonjour name of device
String wifi_APid = "Roller Shade Configuration";    //Name of wifi access point
String config_name_string;

PubSubClient psclient(espClient);      //MQTT client
char mqtt_server[40]="192.168.2.100";  //WIFI config: MQTT server config (optional)
char mqtt_port[6] = "1883";            //WIFI config: MQTT port config (optional)
char mqtt_uid[20];                     //WIFI config: MQTT server username (optional)
char mqtt_pwd[20];                     //WIFI config: MQTT server password (optional)

String availability_topic;

boolean mqtt_active = true;            //Checks if communication with MQTT is alive
String mqtt_clientid = "ESPClient-" + String(ESP.getChipId());  //Identification for MQTT registering

String esp_model = "PLATFORMIO_D1_MINI";    //ESP model, used for HA config
String esp_manufacturer = "espressif";      //ESP Manufacturer, used for HA config

String _configfile = "/config.json";       //Configuration file
String version = "1.0.0";        // Version number for checking if there are new code releases or updates

ESP8266WebServer server(80);              // TCP server at port 80 will respond to HTTP requests
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSockets will respond on port 81

bool controlDualBlinds=false;        //Allows you to connect 2 motors to this ESP
String action;                      //Action manual/auto
int setPos1 = 0;                     //The set position 0-100% by the client
long currentPosition1 = 0;          //Current position of the motor1
long maxPosition1 = 10000;         //Max position of the blind 1. Initial value
int setPos2 = 0;                     //The set position 0-100% by the client
long currentPosition2 = 0;          //Current position of the motor1
long maxPosition2 = 10000;         //Max position of the blind 1. Initial value
boolean saveItNow = false;          //If true will store positions to SPIFFS
boolean motor1Running = false;       //Checks the motor status to stop it
boolean motor2Running = false;       //Checks the motor status to stop it

//declare variables for the motor pins of stepper1
int motorPin1 = D1;  
int motorPin2 = D2;  
int motorPin3 = D3; 
int motorPin4 = D4; 

//declare variables for the motor pins of stepper2
int motorPin5 = D5;  
int motorPin6 = D6;  
int motorPin7 = D7; 
int motorPin8 = D8; 
                 
// The sequence 1-3-2-4 required for proper sequencing of 28BYJ48
AccelStepper stepper1(8, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(8, motorPin5, motorPin7, motorPin6, motorPin8);

bool useBME280Sensor=false;   //Set to true if you connected a BME280
BME280I2C bme;   //BME280 sensor
int bme280SDAPin = 1;  //If you connect the BME to TX and RX pins you will not have serial monitor.
int bme280SCLPin = 3;
bool send_humidity = false;
String humidity;   //Stores the humidity
bool send_temperature = false;
String temperature;  //Stores the temperature
long target_time = 0L;   //variable to be able to update the temperature and humidity    
const unsigned long  PERIOD = 2*60*1000UL; //update temp and humidity each period

//Listens to WebSocket Events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      String res = (char*)payload;

      String motor = res.substring(0, 1);
      String msg = res.substring(2);

      Serial.println("Message for motor: " + motor);
      Serial.println("Message: " + msg);
      //Send to common MQTT and websocket function
      if(motor=="1"){
        processMsgStepper1(msg);
      }else{
        processMsgStepper2(msg);
      }
      
      break;
  }
}

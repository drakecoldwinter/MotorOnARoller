#include <Stepper_28BYJ_48.h>
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
char config_name[40] ="Roller";         //WIFI config: Bonjour name of device
String wifi_APid = "Roller Shade Configuration";    //Name of wifi access point
String config_name_string;

PubSubClient psclient(espClient);      //MQTT client
char mqtt_server[40]="";  //WIFI config: MQTT server config (optional)
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
int path1 = 0;                      //Used for constant movement
int setPos1 = 0;                     //The set position 0-100% by the client
long currentPosition1 = 0;          //Current position of the motor1
long maxPosition1 = 10000;         //Max position of the blind 1. Initial value
bool ccw1 = true;                 //Turns counter clockwise to lower the curtain
int path2 = 0;                      //Used for constant movement
int setPos2 = 0;                     //The set position 0-100% by the client
long currentPosition2 = 0;          //Current position of the motor1
long maxPosition2 = 10000;         //Max position of the blind 1. Initial value
bool ccw2 = true;                 //Turns counter clockwise to lower the curtain
bool saveItNow = false;          //If true will store positions to SPIFFS
bool motor1Running = false;       //Checks the motor status to stop it
bool motor2Running = false;       //Checks the motor status to stop it

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
                 
// Initialize the 28BYJ48 steppers
Stepper_28BYJ_48 stepper1(motorPin1, motorPin2, motorPin3, motorPin4);
Stepper_28BYJ_48 stepper2(motorPin5, motorPin6, motorPin7, motorPin8);

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

void(* resetFunc) (void) = 0; //declare reset function @ address 0

//Listens to WebSocket Events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_TEXT:
      String res = (char*)payload;

      String motor = res.substring(0, 2);
      String msg = res.substring(3);

      if(motor=="01"){
        Serial.println("Message for motor 1: " + msg);
        processMsgStepper1(msg);
          
      }else if(motor=="02"){
        Serial.println("Message for motor 2: " + msg);
        processMsgStepper2(msg);
          
      }else if(motor=="03"){
        Serial.println("Setting clockwise motor 1 to : " + msg);
        ccw1=(msg == "true");
        
      }else if(motor=="04"){
        Serial.println("Setting clockwise motor 2 to : " + msg);
        ccw2=(msg == "true");
        
      }else if(motor=="05"){
        Serial.println("Setting dual covers to : " + msg);
        controlDualBlinds=(msg == "true");
        
      }else if(motor=="06"){
        Serial.println("Setting BME280 to : " + msg);
        useBME280Sensor=(msg == "true");
        
      }else if(motor=="07"){
        Serial.println("Setting device Name to : " + msg);
        msg.toCharArray(config_name, 40);
        
      }else if(motor=="08"){
        Serial.println("Setting MQTT server to : " + msg);
        msg.toCharArray(mqtt_server, 40);
        
      }else if(motor=="09"){
        Serial.println("Setting MQTT port to : " + msg);
        msg.toCharArray(mqtt_port, 6);
        
      }else if(motor=="10"){
        Serial.println("Setting MQTT username to : " + msg);
        msg.toCharArray(mqtt_uid, 20);
        
      }else if(motor=="11"){
        Serial.println("Setting MQTT password to : " + msg);
        msg.toCharArray(mqtt_pwd, 20);
        
      }
      

      break;
  }
}

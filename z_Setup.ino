void setup()
{    
  Serial.begin(115200);
  while (!Serial) continue;
  Serial.print(F("Starting now\n"));

  StartWifi();

  LoadConfiguration();
  
  StartWebSocket();

  StartMDNS();
  
  StartMQTT();

  InitializeTemperatureSensor();
 
  StartWebPage();

  StartOTA();

  StopPowerToCoilsStepper1();

  StopPowerToCoilsStepper2();
}



//Start websocket
void StartWebSocket(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}


//Setup multi DNS (Bonjour)
void StartMDNS(){
  if (MDNS.begin(config_name)) {
    Serial.println(F("MDNS responder started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
  } else {
    Serial.println(F("Error setting up MDNS responder!"));
    while (1) {
      delay(1000);
    }
  }
}


// Setup connection for MQTT and for subscribed messages IF a server address has been entered
void StartMQTT(){
  if (String(mqtt_server) != "") {
    Serial.println(F("Registering MQTT server"));
    psclient.setBufferSize(512);
    psclient.setServer(mqtt_server, String(mqtt_port).toInt());
    psclient.setCallback(MqttCallback);
  } else {
    mqtt_active = false;
    Serial.println(F("NOTE: No MQTT server address has been registered. Only using websockets"));
  }
}


//Configures and setup the webPage
void StartWebPage(){
  //Update webpage
  INDEX_HTML.replace("{VERSION}", "V" + version);
  INDEX_HTML.replace("{NAME}", String(config_name));
  INDEX_HTML.replace("{controlDualBlinds}", String(controlDualBlinds));
  INDEX_HTML.replace("{useBME280Sensor}", String(useBME280Sensor));
  INDEX_HTML.replace("{useDHTsensor}", String(useDHTsensor));
  INDEX_HTML.replace("{clockwise1}", String(ccw1));
  INDEX_HTML.replace("{clockwise2}", String(ccw2));
  INDEX_HTML.replace("{config_name}", String(config_name));
  INDEX_HTML.replace("{mqtt_server}", String(mqtt_server));
  INDEX_HTML.replace("{mqtt_port}", String(mqtt_port));
  INDEX_HTML.replace("{mqtt_uid}", String(mqtt_uid));
  INDEX_HTML.replace("{mqtt_pwd}", String(mqtt_pwd));

  //Serve the webpage
  server.on("/", handleRoot);
  server.on("/reset", handleResetSettings);
  server.on("/saveReboot", handleSaveReboot);
  
  if((useBME280Sensor || useDHTsensor) && send_temperature){
    server.on("/readTemp", handleTemp);
  }
  if((useBME280Sensor || useDHTsensor) && send_humidity){
    server.on("/readHum", handleHum);
  }
  server.onNotFound(handleNotFound);
  server.begin();
}


//Loads configuration file
void LoadConfiguration(){
   
    //Load config upon start
    if (!SPIFFS.begin()) {
      Serial.println("Failed to mount file system");
      return;
    }
   
    if (!loadConfig()) {
      Serial.println(F("Unable to load saved data"));
      maxPosition1 = 100000;
      maxPosition2 = 100000;
    }  
}


//Setups the OTA service
void StartOTA(){
    ArduinoOTA.setHostname(config_name);

    ArduinoOTA.onStart([]() {
      Serial.println(F("Start OTA"));
    });
    ArduinoOTA.onEnd([]() {
      Serial.println(F("\nEnd OTA"));
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
      else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
      else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
      else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
      else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    });
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
}

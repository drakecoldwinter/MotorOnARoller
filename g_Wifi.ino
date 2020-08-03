
//Configures and starts Wifi
void StartWifi(){
  //Set the WIFI hostname
  WiFi.hostname(config_name);

  //Define customer parameters for WIFI Manager
  WiFiManagerParameter custom_config_name("Name", "Bonjour name", config_name, 40);
  WiFiManagerParameter custom_text("<p><b>Optional MQTT server parameters:</b></p>");
  WiFiManagerParameter custom_mqtt_server("server", "MQTT server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_uid("uid", "MQTT username", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_pwd("pwd", "MQTT password", mqtt_server, 40);

  char customhtml1[24] = "type=\"checkbox\"";
    if (useBME280Sensor) {
      strcat(customhtml1, " checked");
    }
  
  WiFiManagerParameter custom_bme("BME280", "Temperature Sensor", "T", 2, customhtml1);

  char customhtml2[24] = "type=\"checkbox\"";
    if (controlDualBlinds) {
      strcat(customhtml2, " checked");
    }
  
  WiFiManagerParameter custom_dualblinds("DualBlinds", "Dual blinds", "T", 2, customhtml2);

  //Setup WIFI Manager
  WiFiManager wifiManager;

  //resets wifi parameters
  //wifiManager.resetSettings();
  //resets the esp8266 config file
  //SPIFFS.format();

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //add all your parameters here
  wifiManager.addParameter(&custom_config_name);
  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_uid);
  wifiManager.addParameter(&custom_mqtt_pwd);
  wifiManager.addParameter(&custom_bme);
  wifiManager.addParameter(&custom_dualblinds);

  wifiManager.autoConnect(wifi_APid.c_str());

  delay(1000);
  //If connected to Wifi we disable AP
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    Serial.print("Connect to http://" + String(config_name) + ".local or http://");
    Serial.println(WiFi.localIP());
  }

  if (shouldSaveConfig) {
    //read updated parameters
    strcpy(config_name, custom_config_name.getValue());
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(mqtt_uid, custom_mqtt_uid.getValue());
    strcpy(mqtt_pwd, custom_mqtt_pwd.getValue());

    useBME280Sensor = (strncmp(custom_bme.getValue(), "T", 1) == 0);
    controlDualBlinds = (strncmp(custom_dualblinds.getValue(), "T", 1) == 0);
    
    //Save the data
    saveItNow = true;
  }
}




/*
   Callback from WIFI Manager for saving configuration
*/
void saveConfigCallback () {
  shouldSaveConfig = true;
}

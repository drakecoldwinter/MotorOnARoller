
/**
   Load configuration data from a JSON file
   on SPIFFS
*/
bool loadConfig() {

  File configFile = SPIFFS.open(_configfile, "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file"));
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println(F("Config file size is too large"));
    return false;
  }

  //No more memory leaks
  DynamicJsonBuffer jsonBuffer(300);

  //Reading directly from file DOES NOT cause currentPosition to break
  JsonVariant _config = jsonBuffer.parseObject(configFile);

  //Avoid leaving opened files
  configFile.close();

  if (!_config.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  
  //Useful if you need to see why confing is read incorrectly
  _config.printTo(Serial);
  Serial.println("");
  
  //Store variables locally
  currentPosition1 = _config["currentPosition1"].as<long>();
  setPos1 = currentPosition1;
  maxPosition1 = _config["maxPosition1"].as<long>();
  currentPosition2 = _config["currentPosition2"].as<long>();
  setPos2 = currentPosition2;
  maxPosition2 = _config["maxPosition2"].as<long>();
  useBME280Sensor =_config["useBME280Sensor"].as<bool>();
  controlDualBlinds =_config["controlDualBlinds"].as<bool>();
  ccw1 =_config["ccw1"].as<bool>();
  ccw2 =_config["ccw2"].as<bool>();

  strcpy(config_name, _config["config_name"]);
  strcpy(mqtt_server, _config["mqtt_server"]);
  strcpy(mqtt_port, _config["mqtt_port"]);
  strcpy(mqtt_uid, _config["mqtt_uid"]);
  strcpy(mqtt_pwd, _config["mqtt_pwd"]);

  return true;
}



/**
   Save configuration data to a JSON file
   on SPIFFS
*/
bool saveConfig() {
  DynamicJsonBuffer jsonBuffer(500);
  JsonObject& json = jsonBuffer.createObject();
  json["currentPosition1"] = currentPosition1;
  json["maxPosition1"] = maxPosition1;
  json["currentPosition2"] = currentPosition2;
  json["maxPosition2"] = maxPosition2;
  json["config_name"] = config_name;
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_uid"] = mqtt_uid;
  json["mqtt_pwd"] = mqtt_pwd;
  json["useBME280Sensor"] = useBME280Sensor;
  json["controlDualBlinds"] = controlDualBlinds;
  json["ccw1"] = controlDualBlinds;
  json["ccw2"] = controlDualBlinds;

  File configFile = SPIFFS.open(_configfile, "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }

  json.printTo(configFile);
  configFile.flush(); //Making sure it's saved
  
  Serial.println(F("Saved JSON to SPIFFS"));
  json.printTo(Serial);
  Serial.println();
  return true;
}


/**
   Load configuration data from a JSON file
   on SPIFFS
*/
bool loadConfig() {

  File file = SPIFFS.open(_configfile, "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = file.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a temporary JsonDocument
  StaticJsonDocument<512> _config;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(_config, file);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
    return false;
  }

  //Store variables locally
  currentPosition1 = _config["currentPosition1"].as<long>();
  setPos1 = currentPosition1;
  maxPosition1 = _config["maxPosition1"].as<long>();
  currentPosition2 = _config["currentPosition2"].as<long>();  setPos2 = currentPosition2;
  maxPosition2 = _config["maxPosition2"].as<long>();
  useBME280Sensor =_config["useBME280Sensor"].as<bool>();
  useDHTsensor =_config["useDHTsensor"].as<bool>();
  controlDualBlinds =_config["controlDualBlinds"].as<bool>();
  ccw1 =_config["ccw1"].as<bool>();
  ccw2 =_config["ccw2"].as<bool>();

  strcpy(config_name, _config["config_name"]);
  strcpy(mqtt_server, _config["mqtt_server"]);
  strcpy(mqtt_port, _config["mqtt_port"]);
  strcpy(mqtt_uid, _config["mqtt_uid"]);
  strcpy(mqtt_pwd, _config["mqtt_pwd"]);

  //output the loaded config file
  serializeJson(_config, Serial);
  Serial.println();
  
  //Avoid leaving opened files
  file.close();

  return true;
}



/**
   Save configuration data to a JSON file
   on SPIFFS
*/
bool saveConfig() {

  StaticJsonDocument<512> json;
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
  json["useDHTsensor"] = useDHTsensor;
  json["controlDualBlinds"] = controlDualBlinds;
  json["ccw1"] = controlDualBlinds;
  json["ccw2"] = controlDualBlinds;

  File file = SPIFFS.open(_configfile, "w");
  if (!file) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  // Serialize JSON to file
  if (serializeJson(json, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
  
  Serial.println(F("Saved JSON to SPIFFS"));
  Serial.println();
  return true;
}

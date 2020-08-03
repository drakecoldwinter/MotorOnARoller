
//Connects or Reconnects to MQTT server
void MqttReconnect() {

  config_name_string = String(config_name);
  config_name_string.toLowerCase();
  
  boolean mqtt_logon = false;
  if (mqtt_uid != NULL and mqtt_pwd != NULL) {
    mqtt_logon = true;
  }

  //loop until we connect
  while (!psclient.connected()) {
    Serial.print("Attempting MQTT connection...");

    availability_topic = "homeassistant/binary_sensor/connectivity/" + config_name_string + "/out";
    String last_will_message = "offline";

    // Attempt to connect
    if ((mqtt_logon ? psclient.connect(mqtt_clientid.c_str(), mqtt_uid, mqtt_pwd, availability_topic.c_str(), 1, true, last_will_message.c_str()) : psclient.connect(mqtt_clientid.c_str(), availability_topic.c_str(), 1, true, last_will_message.c_str()))) {
      Serial.println(F("connected"));

      //Sets avalaibility topic
      SendMsg(availability_topic, "online", true);

      if(controlDualBlinds){
          //Sends both covers configs to MQTT
          CoverConfig(config_name_string, "_a");
          CoverConfig(config_name_string, "_b");
      }else{
          //Sends a single cover config to MQTT
          CoverConfig(config_name_string, "");
      }


      //Sends the temperature sensor config to MQTT
      TemperatureConfig(config_name_string);

      //Sends the humdity sensor config to MQTT
      HumidityConfig(config_name_string);

      //Sends the connectivity sensor config to MQTT
      ConnectivityConfig(config_name_string);
      
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(psclient.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      ESP.wdtFeed();
      delay(5000);
    }
  }
  if (psclient.connected()) {
    psclient.loop();
  }
}


//Listens to MQTT events
void MqttCallback(char* topic, byte* payload, unsigned int length) {
  String res = "";
  for (int i = 0; i < length; i++) {
    res += String((char) payload[i]);
  }
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(":" + res);
  Serial.println(F("] "));

  if(controlDualBlinds && (String(topic).indexOf("_b/in") > 1)){
    processMsgStepper2(res);
  }else{
    processMsgStepper1(res);
  }  
}



//Sends a MQTT message
void SendMsg(String topic, String payload, bool retained) {
  if (!mqtt_active)
    return;
  
  topic.toLowerCase();
  Serial.println("Trying to send msg..." + topic + ":" + payload);
  if (psclient.connected()) {
    psclient.publish(topic.c_str(), payload.c_str(), retained);
  } else {
    Serial.println(F("PubSub client is not connected..."));
  }
}


//Prepares the config for the cover
void CoverConfig(String config_name_string, String cover_letter){
   
      String configString = "{\"dev_cla\":\"shade\", \"name\":\"" + config_name_string + cover_letter + "_cover\", ";
      configString += "\"uniq_id\":\"" + config_name_string + cover_letter + "_cover\", ";
      configString += DeviceConfig(config_name_string);
      configString += "\"avty_t\":\"" + availability_topic + "\", ";
      configString += "\"~\":\"homeassistant/cover/" + config_name_string + cover_letter + "\", ";
      configString += "\"cmd_t\":\"~/in\", ";
      configString += "\"set_pos_t\":\"~/in\", ";
      configString += "\"pos_t\":\"~/out\", ";
      configString += "\"json_attr_t\":\"~/out\", ";
      configString += "\"json_attr_tpl\":\"{{value_json.position|tojson}}\", ";
      configString += "\"pl_open\":\"0\", ";
      configString += "\"pl_cls\":\"100\", ";
      //configString += "\"pos_open\":100, ";
      //configString += "\"pos_clsd\":0, ";
      configString += "\"val_tpl\":\"{{value_json.position}}\"";
      configString += "}";

      //home assistant cover config page
      SendMsg("homeassistant/cover/" + config_name_string + cover_letter + "/config", configString, true);

      //MQTT topic for listening 
      String inputTopic = "homeassistant/cover/" + config_name_string + cover_letter + "/in";    

      //Setup subscription to cover input topic
      inputTopic.toLowerCase();
      psclient.subscribe((char*) inputTopic.c_str());
      Serial.println("Subscribed to " + inputTopic);
}


//Prepares the config for the temperature sensor
void TemperatureConfig(String config_name_string){
  if(useBME280Sensor && send_temperature){
      String configString = "{\"dev_cla\":\"temperature\", \"name\":\"" + config_name_string + "_temperature\", ";
      configString += "\"uniq_id\":\"" + config_name_string + "_temperature\", ";
      configString += DeviceConfig(config_name_string);
      configString += "\"avty_t\":\"" + availability_topic + "\", ";
      configString += "\"~\":\"homeassistant/sensor/temperature/" + config_name_string + "\", ";
      configString += "\"stat_t\":\"~/out\", ";
      configString += "\"unit_of_meas\":\"°C\", ";
      configString += "\"val_tpl\":\"{{value_json.temperature}}\"";
      configString += "}";

      //home assistant temperature config page
      SendMsg("homeassistant/sensor/temperature/" + config_name_string + "/config", configString, true);
  }
}

//Prepares the config for the humidity sensor
void HumidityConfig(String config_name_string){
  if (useBME280Sensor && send_humidity){
      String configString = "{\"dev_cla\":\"humidity\", \"name\":\"" + config_name_string + "_humidity\", ";
      configString += "\"uniq_id\":\"" + config_name_string + "_humidity\", ";
      configString += DeviceConfig(config_name_string);
      configString += "\"avty_t\":\"" + availability_topic + "\", ";
      configString += "\"~\":\"homeassistant/sensor/humidity/" + config_name_string + "\", ";
      configString += "\"stat_t\":\"~/out\", ";
      configString += "\"unit_of_meas\":\"%\", ";
      configString += "\"val_tpl\":\"{{value_json.humidity}}\"";
      configString += "}";

      //home assistant temperature config page
      SendMsg("homeassistant/sensor/humidity/" + config_name_string + "/config", configString, true);
  }
}

//Prepares the config for the connectivity sensor
void ConnectivityConfig(String config_name_string){
  
      String configString = "{\"dev_cla\":\"connectivity\", \"name\":\"" + config_name_string + "_status\", ";
      configString += "\"uniq_id\":\"" + config_name_string + "_status\", ";
      configString += DeviceConfig(config_name_string);
      configString += "\"~\":\"homeassistant/binary_sensor/connectivity/" + config_name_string + "\", ";
      configString += "\"stat_t\":\"~/out\", ";
      //configString += "\"val_tpl\":\"{{value_json.value}}\"";
      configString += "\"pl_on\":\"online\", ";
      configString += "\"pl_off\":\"offline\"";
      configString += "}";

      //home assistant temperature config page
      SendMsg("homeassistant/binary_sensor/connectivity/" + config_name_string + "/config", configString, true);
}

//Sends the device string
String DeviceConfig(String config_name_string){
  return "\"dev\":{\"ids\":\"" + config_name_string + "\",\"name\":\"" + config_name_string + "\",\"mdl\":\"" + esp_model + "\",\"mf\":\"" + esp_manufacturer + "\"}, ";    
}

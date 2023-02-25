

//Configures and starts Wifi
void StartWifi(){
  //Set the WIFI hostname
  WiFi.hostname(config_name);

  //Setup WIFI Manager
  WiFiManager wifiManager;

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.autoConnect(wifi_APid.c_str());

  delay(1000);
  //If connected to Wifi we disable AP
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    Serial.print("Connect to http://" + String(config_name) + ".local or http://");
    Serial.println(WiFi.localIP());
  }
}


//Tryes to reconnect the wifi after a long delay
void ReconnectWifi(){  
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=(PERIOD*3))) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}



//Takes care of resetting the ESP and Wifi Settings
void handleResetSettings() {
  
  WiFiManager wifiManager;
  //resets wifi parameters
  wifiManager.resetSettings();
  //resets the esp8266 config file
  SPIFFS.format();
}


/*
   Callback from WIFI Manager for saving configuration
*/
void saveConfigCallback () {
  saveItNow = true;
}

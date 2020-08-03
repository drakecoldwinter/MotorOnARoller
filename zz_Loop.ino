
void loop()
{

  //MQTT client
  if (mqtt_active)
        MqttReconnect();
        
 //Websocket listner
  webSocket.loop();
    
  //Webpage server
  server.handleClient();

  //Keep Bonjour alive
  MDNS.update();
  
  //OTA client code
  ArduinoOTA.handle();
          
  CheckBME280();
  
  RunMotors();
  
  Save();
}


//if the period is over, then update the humidity and temperature
void CheckBME280(){
  if (useBME280Sensor && (millis () - target_time >= PERIOD))
  {
        target_time += PERIOD ;   // change scheduled time exactly, no slippage will happen
        
        //Sends the BME280 data to MQTT
        SendTemperatureAndHumidity();
  }
}

  //make the stepper move
void RunMotors(){
  if (currentPosition1 != setPos1 || action == "settings"){
    motor1Running = true;
    stepper1.run();
    currentPosition1 = stepper1.currentPosition();
  }else{
    StopPowerToCoilsStepper1();
  }  

  if (currentPosition2 != setPos2 || action == "settings"){
    motor2Running = true;
    stepper2.run();
    currentPosition2 = stepper2.currentPosition();
  }else{
    StopPowerToCoilsStepper2();
  }
}

//Storing positioning data and turns off the power to the coils
void Save(){
  if (saveItNow) {
        saveConfig();
        saveItNow = false;
   }
}

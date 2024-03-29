
void loop()
{  
  //while running the motor we disable some features
  if (motor1Running==false){
    
     //Webpage server
     server.handleClient();
    
    //Keep Bonjour alive
    MDNS.update();
  
    //OTA client code
    ArduinoOTA.handle();

    //temperature sensors      
    CheckTemperature();
  }
    
  //MQTT client
  if (mqtt_active)
     MqttReconnect();

  //Websocket listner
  webSocket.loop();
    
  RunMotors();

  Save();
}


//if the period is over, then update the humidity and temperature
void CheckTemperature(){
  if (useBME280Sensor && (millis() - target_time >= PERIOD))
  {
        target_time += PERIOD ;   // change scheduled time exactly, no slippage will happen
        
        //Sends the BME280 data to MQTT
        SendBME280TemperatureAndHumidity();
  }
  if(useDHTsensor && (millis() - target_time >= PERIOD))
  {
    target_time += PERIOD ;   // change scheduled time exactly, no slippage will happen

    //Sends the DHT data to MQTT
    SendDHTTemperatureAndHumidity();
  }
}

  //make the stepper move
void RunMotors(){
  if ((currentPosition1 > setPos1 && path1==0) || path1 == -1){
    stepper1.step(ccw1 ? -1 : 1);
    motor1Running = true;
    currentPosition1 = currentPosition1-1;
    Serial.println("Motor 1 going up");
    
  } else if ((currentPosition1 < setPos1 && path1==0) || path1 == 1) {
    stepper1.step(ccw1 ? 1 : -1);
    motor1Running = true;
    currentPosition1 = currentPosition1+1;
    Serial.println("Motor 1 going down");
    
  }else{
    StopPowerToCoilsStepper1();
  }  

  if(controlDualBlinds){
     if ((currentPosition2 > setPos2 && path2==0) || path2 == -1){
      stepper2.step(ccw2 ? -1 : 1);
      motor2Running = true;
      currentPosition2 = currentPosition2-1;
      Serial.println("Motor 2 going up");
      
    } else if ((currentPosition2 < setPos2 && path2==0) || path2 == 1) {
      stepper2.step(ccw2 ? 1 : -1);
      motor2Running = true;
      currentPosition2 = currentPosition2+1;
      Serial.println("Motor 2 going down");
      
    }else{
      StopPowerToCoilsStepper2();
    }  
  }
 
}

//Storing positioning data and turns off the power to the coils
void Save(){
  if (saveItNow) {
        saveConfig();
        saveItNow = false;
   }
}

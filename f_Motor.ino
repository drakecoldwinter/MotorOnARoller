
//process the start, max and update messages for stepper 1
void processMsgStepper1(String res){

  if (res == "(start)") {
    //Store the current position as the start position
    InitializeMotor1();
    saveItNow = true;
    
  } else if (res == "(max)") {
    //Store the max position of a closed blind
    maxPosition1 = stepper1.currentPosition();
    saveItNow = true;
    
  }else if (res == "(update)") {
    //Send position details to client
    UpdateClientStepper1();
    
  } else if (res == "(ping)") {
    //Do nothing
    
  }else if (res == "(0)" || res =="STOP") {
    //Stop button pressed
    stepper1.stop();
    setPos1 = maxPosition1 * stepper1.currentPosition() / 100;
    UpdateClientStepper1();
    action="";
    
  } else if (res == "(1)") {
    //Move down without limit to max position
    stepper1.setAcceleration(500);
    stepper1.moveTo(100000000);
    action="settings";
    
  } else if (res == "(-1)") {
    //Move up without limit to top position
    stepper1.setAcceleration(500);
    stepper1.moveTo(-100000000);
    action="settings";
    
  } else {  
    if (action=="settings"){
      action="";
      InitializeMotor1();
    }
    setPos1 = maxPosition1 * res.toInt() / 100;
    stepper1.moveTo(setPos1);
    UpdateClientStepper1();
  }
}

//process the start, max and update messages for stepper 1
void processMsgStepper2(String res){
  if (res == "(start)") {
    //Store the current position as the start position
    InitializeMotor2();
    saveItNow = true;
    
  } else if (res == "(max)") {
    //Store the max position of a closed blind
    maxPosition2 = stepper2.currentPosition();
    saveItNow = true;
    
  }else if (res == "(update)") {
    //Send position details to client
    UpdateClientStepper2();
    
  } else if (res == "(ping)") {
    //Do nothing
    
  }else if (res == "(0)" || res =="STOP") {
    //Stop button pressed
    stepper2.stop();
    setPos2 = maxPosition2 * stepper2.currentPosition() / 100;
    UpdateClientStepper2();
    action="";
    
  } else if (res == "(1)") {
    //Move down without limit to max position
    stepper2.setAcceleration(500);
    stepper2.moveTo(100000000);
    action="settings";
    
  } else if (res == "(-1)") {
    //Move up without limit to top position
    stepper2.setAcceleration(500);
    stepper2.moveTo(-100000000);
    action="settings";
    
  } else {  
    if (action=="settings"){
      action="";
      InitializeMotor2();
    }
    setPos2 = maxPosition2 * res.toInt() / 100;
    stepper2.moveTo(setPos2);
    UpdateClientStepper2();
  }
}



//Set's the speed and acceleration of the motor
void InitializeMotor1(){
    stepper1.setCurrentPosition(currentPosition1);
    setPos1 = currentPosition1;
    stepper1.setMaxSpeed(800);
    stepper1.setAcceleration(100);
    UpdateClientStepper1();
}

//Set's the speed and acceleration of the motor
void InitializeMotor2(){
    stepper2.setCurrentPosition(currentPosition2);
    setPos2 = currentPosition2;
    stepper2.setMaxSpeed(800);
    stepper2.setAcceleration(100);
    UpdateClientStepper2();
}

//Send's the Set and the Pos to MQTT
void UpdateClientStepper1(){
    String outputTopic ="homeassistant/cover/" + String(config_name) + "/out";
    if(controlDualBlinds){
        outputTopic ="homeassistant/cover/" + String(config_name) + "_a/out";
    }
    int set = (setPos1 * 100) / maxPosition1;
    int pos = (currentPosition1 * 100) / maxPosition1;
    SendMsg(outputTopic, "{ \"set\":" + String(set) + ", \"position\":" + String(pos) + " }", true);
    webSocket.broadcastTXT("{ \"set\":" + String(set) + ", \"position\":" + String(pos) + ", \"motor\": 1 }");
}


//Send's the Set and the Pos to MQTT
void UpdateClientStepper2(){
        String outputTopic ="homeassistant/cover/" + String(config_name) + "_b/out";
        int set = (setPos2 * 100) / maxPosition2;
        int pos = (currentPosition2 * 100) / maxPosition2;
        SendMsg(outputTopic, "{ \"set\":" + String(set) + ", \"position\":" + String(pos) + " }", true);
        webSocket.broadcastTXT("{ \"set\":" + String(set) + ", \"position\":" + String(pos) + ", \"motor\": 2 }");
}

//Stopping the coils to avoid overheating 
void StopPowerToCoilsStepper1() {
  if (motor1Running){
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
    digitalWrite(D4, LOW);
    Serial.println(F("Motor 1 stopped"));
    motor1Running = false;
    saveItNow = true;
    UpdateClientStepper1();
  }
}

//Stopping the coils to avoid overheating 
void StopPowerToCoilsStepper2() {
  if (motor2Running){
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
    Serial.println(F("Motor 2 stopped"));
    motor2Running = false;
    saveItNow = true;
    UpdateClientStepper2();
  }
}

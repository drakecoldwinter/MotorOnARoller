
//process the start, max and update messages for stepper 1
void processMsgStepper1(String res){

  if (res == "(start)") {
    //Store the current position as the start position
    currentPosition1 = 0;
    setPos1=0;
    path1 = 0;
    saveItNow = true;
    
  } else if (res == "(max)") {
    //Store the max position of a closed blind
    path1 = 0;
    maxPosition1 = currentPosition1;
    setPos1 = maxPosition1;
    saveItNow = true;
    
  }else if (res == "(update)") {
    //Send position details to client
    path1 = 0;
    UpdateClientStepper1();
    
  } else if (res == "(ping)") {
    //Do nothing
    
  }else if (res == "(0)" || res =="STOP") {
    //Stop button pressed
    path1 = 0;
    setPos1 = currentPosition1;
    UpdateClientStepper1();
  
  } else if (res == "(1)") {
    //Move down without limit to max position
    path1 = 1;
    
  } else if (res == "(-1)") {
    //Move up without limit to top position
    path1= -1;
    
  } else {  
    path1 = 0;
    setPos1 = maxPosition1 * res.toInt() / 100;
    UpdateClientStepper1();
  }
}

//process the start, max and update messages for stepper 1
void processMsgStepper2(String res){
  if (res == "(start)") {
    //Store the current position as the start position
    currentPosition2 = 0;
    path2 = 0;
    setPos2=0;
    saveItNow = true;
    
  } else if (res == "(max)") {
    //Store the max position of a closed blind
    maxPosition2 = currentPosition2;
    setPos2=maxPosition2;
    path2 = 0;
    saveItNow = true;
    
  }else if (res == "(update)") {
    //Send position details to client
    path2 = 0;
    UpdateClientStepper2();
    
  } else if (res == "(ping)") {
    //Do nothing
    
  }else if (res == "(0)" || res =="STOP") {
    //Stop button pressed
    setPos2 = currentPosition2;
    UpdateClientStepper2();
    path2 = 0;
    
  } else if (res == "(1)") {
    //Move down without limit to max position
    path2 = 1;
    
  } else if (res == "(-1)") {
    //Move up without limit to top position
    path2 = -1;
    
  } else {  
    path2 = 0;
    setPos2 = maxPosition2 * res.toInt() / 100;
    UpdateClientStepper2();
  }
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

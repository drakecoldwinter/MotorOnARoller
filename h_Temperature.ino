
//Sends the temperature and humidity to MQTT
void SendBME280TemperatureAndHumidity() {

  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  sendToMQTT(temp, hum);
}

void SendDHTTemperatureAndHumidity(){

    float temp(NAN), hum(NAN);
    
    temp = dht.readTemperature();

    hum = dht.readHumidity();
  
    sendToMQTT(temp, hum);   
}

void sendToMQTT(float temp, float hum){
  if (send_temperature) {
    SendMsg("homeassistant/sensor/temperature/" + config_name_string + "/out", "{\"temperature\":" + String(temp) + "}", true);
    temperature = String(temp);
  }

  if (send_humidity) {
    SendMsg("homeassistant/sensor/humidity/" + config_name_string + "/out", "{\"humidity\":" + String(hum) + "}", true);
    humidity = String(hum);
  }  
}



//Initialize the Temperature sensor
void InitializeTemperatureSensor() {
  if(useBME280Sensor || useDHTsensor){
    //GPIO 1 (TX) swap the pin to a GPIO.
    pinMode(1, FUNCTION_3);
    //GPIO 3 (RX) swap the pin to a GPIO.
    pinMode(3, FUNCTION_3);
  }

  if (useBME280Sensor) {
    Wire.begin(bme280SDAPin, bme280SCLPin);
    if (!bme.begin())
    {
      Serial.println("Could not find BME280 sensor!");
    } else {
      switch (bme.chipModel())
      {
        case BME280::ChipModel_BME280:
          send_temperature = true;
          send_humidity = true;
          Serial.println("Found BME280 sensor!");
          break;
        case BME280::ChipModel_BMP280:
          send_temperature = true;
          send_humidity = false;
          Serial.println("Found BMP280 sensor! No Humidity available.");
          break;
        default:
          send_temperature = false;
          send_humidity = false;
          Serial.println("Found UNKNOWN sensor! Error!");
      }
    }
  }
  if(useDHTsensor){
      dht.begin();
      send_temperature = true;
      send_humidity = true;
      Serial.println("Configured for DHT11 sensor!");
  }
}

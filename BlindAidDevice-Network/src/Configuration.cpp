#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

class Configuration{
public:
  String config;
  bool debug;


  Configuration(bool d=false){
    debug = d;
  }
  const char* ssid = new char[60];
  const char* password = new char[60];
  const char* devID = new char [60];

  bool exists(){
    EEPROM.begin(2048);
    if(EEPROM.read(2)==0xFF)
    return true;
    else
    return false;
  }


  bool SaveConfig(String conf){
    int size = conf.length();
    if(debug){
      Serial.println("---------Config--------------------------");
      Serial.print("Given Record length:");
      Serial.println(size);
    }
    byte high = highByte(size);
    byte low = lowByte(size);
    if(debug){
      Serial.println(high);
      Serial.println(low);
    }
    EEPROM.begin(2048);
    EEPROM.write(0,high);
    EEPROM.write(1,low);
    EEPROM.write(2,0xFF);
    if(debug) Serial.println("Writing:");
    for(int i = 0; i<size; i++){
      EEPROM.write(i+10,conf[i]);
      if(debug) Serial.print((char)EEPROM.read(i+10));
    }
    if(debug) Serial.println();
    bool ret = EEPROM.commit();
    EEPROM.end();
    return ret;
  }

  void load(){
    DynamicJsonBuffer buffer;

    EEPROM.begin(2048);
    if(EEPROM.read(2)==0xFF){
    int len = (EEPROM.read(0) << 8) + EEPROM.read(1);
    char confArray[len+1];
    if(debug){
      Serial.println("---------Config--------------------------");
      Serial.print("Found record length:");
      Serial.println(len);
    }
    if(debug) Serial.println("Reading:");
    for(int i = 0; i<len; i++){
      confArray[i] = (char)EEPROM.read(i+10);
      if(debug) Serial.print(confArray[i]);
    }
    confArray[len] = '\0';
    if(debug) Serial.println();
    config = String(confArray);
    if(debug){
      Serial.println("---------Config--------------------------");
      Serial.print("Payload String:");
      Serial.println(config);
    }
    JsonObject& confjson = buffer.parseObject(config);
    if(confjson.success()){
      ssid = confjson["ssid"];
      password = confjson["password"];
      devID = confjson["devID"];
      if(debug){
        Serial.println("---------Config--------------------------");
        Serial.print("ssid: ");
        Serial.println(ssid);
        Serial.print("password: ");
        Serial.println(password);
        Serial.print("devID: ");
        Serial.println(devID);
      }
    }
    else{
      if(debug) Serial.println("Parsing error");
    }
  }
  else
    if(debug) Serial.println("No config record");
  }
};

#include <Arduino.h>
#include "Server.cpp"
#include "GeoClient.cpp"
#define mode 14

void postLocation(char location[300]);
bool debug = true;
bool serverMode = false;
double lat=0.0;
double lng=0.0;
double acc=0.0;

String API  = "AIzaSyADb7jnMpGn507x-m32M2gIFbPFJu0KvPs";
String dev;

serverHandler server(80,debug);
GeoClientHandler geo(debug);
Configuration configure(debug);

DynamicJsonBuffer jsonBuffer;

void setup() {
  pinMode(mode, INPUT_PULLUP);

  Serial.begin(115200);
    if(debug){
      Serial.println("<--Main module-->");
      if(digitalRead(mode)==LOW){
        Serial.println("CONFIG MODE");
        serverMode = true;
      }
    }
    if(serverMode) {
      server.start();
    }
    else{
      if(configure.exists()){
        delay(5000);
        configure.load();
        geo.init(configure.ssid,configure.password,API);
        dev = configure.devID;
        delay(2000);
        geo.start();

    }
    else{
      serverMode = true;
      server.start();
    }
  }
}

void loop() {
    if(serverMode){
      server.Handle();
      }
    else{
      String response = geo.Locate();
      JsonObject& res = jsonBuffer.parseObject(response); //Parse Response
      if(res.success()){
         Serial.println("Location Retrival: Success");
         res["id"] = dev;
         char loc[300];
         res.prettyPrintTo(loc,sizeof(loc));
         postLocation(loc);
       }
      else Serial.println("Location Retrival: Fail");
    }
}

void postLocation(char location[300]){

  HTTPClient http;
  http.begin("http://ec2-18-188-137-2.us-east-2.compute.amazonaws.com/location/update");
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(location);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);

  http.end();
}

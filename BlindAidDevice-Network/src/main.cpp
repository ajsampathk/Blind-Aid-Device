#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "Server.cpp"
#include "GeoClient.cpp"

void postLocation(char location[300]);
int debug = 0;
int serverMode =0;
double lat=0.0;
double lng=0.0;
double acc=0.0;
char* ssid = "Avadh";
char* pass = "9642063636";
String API  = "AIzaSyBBtgzfQUt9UhJg65fp8RQenNBVEw6HBoM";
String dev  = "PROTOTYPE_1";

serverHandler server(debug);
GeoClientHandler geo(debug);

void setup() {
  Serial.begin(9600);
    if(!debug){
      Serial.println("<--Main module-->");
    }
    if(serverMode) server.start();
    else{
      geo.init(ssid,pass,API);
      geo.start();
    }
}

void loop() {
    if(serverMode){
      server.Handle();
    }
    else{
      String response = geo.Locate();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& res = jsonBuffer.parseObject(response); //Parse Response
      if(res.success()){
         if(!debug){
           Serial.println("Location Retrival: Success");
           // Serial.println(response);
         }
         res["id"] = dev;
         char loc[300];
         res.prettyPrintTo(loc,sizeof(loc));
         postLocation(loc);
       }
      else if(!debug)Serial.println("Location Retrival: Fail");
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

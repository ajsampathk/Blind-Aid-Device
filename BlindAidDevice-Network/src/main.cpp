#include <Arduino.h>
#include "Server.cpp"
#include "GeoClient.cpp"

int debug = 0;
int serverMode =0;
double lat=0.0;
double lng=0.0;
double acc=0.0;

serverHandler server(debug);
GeoClientHandler geo(debug);

void setup() {
    if(!debug){
      Serial.begin(9600);
      Serial.println("<--Main module-->");
    }
    if(serverMode) server.start();
    else{
      geo.init((char*)"Avadh",(char*)"9642063636","AIzaSyBBtgzfQUt9UhJg65fp8RQenNBVEw6HBoM");
      geo.start();
    }
}

void loop() {
    if(serverMode)server.Handle();
    else{
      String response = geo.Locate();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& res = jsonBuffer.parseObject(response); //Parse Response
      if(res.success()){
         if(!debug)Serial.println("Conversion Succesful");
         Serial.println(response);
       }
      else if(!debug)Serial.println("Conversion Failed");
    }

}

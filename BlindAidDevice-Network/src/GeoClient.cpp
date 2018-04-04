#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class GeoClientHandler{

private:
  char* _SSID = new char[60];
  char* _PASS = new char[60];
  String key;

public:
    struct location{
      double lat;
      double lng;
      double acc;
    };
    typedef struct location location;

    bool debug;

    GeoClientHandler(bool d = false){
      debug =d;
    }


    bool init(const char* p_ssid,const char* p_pass,String API){
      strcpy(_SSID,p_ssid);
      strcpy(_PASS,p_pass);
      key = API;
      if(debug){
        Serial.println("---------Client--------------------------");
        Serial.print("Client ssid: ");
        Serial.println(_SSID);
        Serial.print("Client password: ");
        Serial.println(_PASS);
        Serial.println("-----------------------------------");
      }
      return true;
    }
    //URI for Google GeoLocation API...
    const char* Host = "www.googleapis.com";
    String thisPage = "/geolocation/v1/geolocate?key=";

    int status = WL_IDLE_STATUS;
    String jsonString = "{\n";

    double latitude    = 0.0;
    double longitude   = 0.0;
    double accuracy    = 0.0;

    bool start()   {
      if(debug){
        Serial.println("Start");
      }
      if(_SSID && _PASS){
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      if(debug){
      Serial.println("Setup done");
      Serial.println("Connecting to ");
      Serial.print("SSID: ");
      Serial.print(_SSID);
      Serial.print(" with password: ");
      Serial.println(_PASS);
    }
      WiFi.begin(_SSID, _PASS);
      delete [] _SSID;
      delete [] _PASS;

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if(debug)Serial.print(".");
      }
      if(debug)Serial.println(".");
      return true;
    }
    else{
      if(debug)Serial.println("Value Error");
      return false;
    }

    }


    String Locate() {
      String res;
      DynamicJsonBuffer jsonBuffer;
      int n = WiFi.scanNetworks();
      if (n == 0){
        if(debug)Serial.println("no networks found");
      }
      else
      {
        if(debug){
        Serial.print(n);
        Serial.println(" networks found...");
        }
      //Creating JSON Stirng
        jsonString = "{\n";
        jsonString += "\"wifiAccessPoints\": [\n";
        for (int i = 0; i < n; ++i)
        {
          jsonString += "{\n";
          jsonString += "\"macAddress\" : \"";
          jsonString += (WiFi.BSSIDstr(i));
          jsonString += "\",\n";
          jsonString += "\"signalStrength\": ";
          jsonString += WiFi.RSSI(i);
          jsonString += "\n";
          if (i < n - 1)
          {
            jsonString += "},\n";
          }
          else
          {
            jsonString += "}\n";
          }
        }
        jsonString += ("]\n");
        jsonString += ("}\n");


        if(debug)Serial.println("");

        WiFiClientSecure client;

        //Connect to the client
        if(debug)Serial.println("Requesting Location");
        if (client.connect(Host, 443)) {
          if(debug)Serial.println("Connected");
          client.println("POST " + thisPage + key + " HTTP/1.1");
          client.println("Host: " + (String)Host);
          client.println("Connection: close");
          client.println("Content-Type: application/json");
          client.println("User-Agent: Arduino/1.0");
          client.print("Content-Length: ");
          client.println(jsonString.length());
          client.println();
          client.print(jsonString);
          delay(500);
        }


        while (client.available()) {
          res = client.readStringUntil('\r'); //Read respose

          JsonObject& response = jsonBuffer.parseObject(res); //Parse Response
          if (response.success()) {
            latitude    = response["location"]["lat"];
            longitude   = response["location"]["lng"];
            accuracy   = response["accuracy"];
          }
        }


        client.stop();
        if(debug){
        Serial.print("Latitude = ");
        Serial.println(latitude, 6);
        Serial.print("Longitude = ");
        Serial.println(longitude, 6);
        Serial.print("Accuracy = ");
        Serial.println(accuracy);
      }
        return res;

      }
    }

    location LocateValues(){
      location l;
      l.lat = latitude;
      l.lng = longitude;
      l.acc = accuracy;
      return l;
    }
};

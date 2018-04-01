#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class GeoClientHandler{

  public:
    struct location{
      double lat;
      double lng;
      double acc;
    };
    typedef struct location location;

    char* ssid;
    char* pass;
    String key;
    int debug;

    GeoClientHandler(int d=0){
      debug =d;
    }

    void init(char* p_ssid,char* p_pass,String API){
      ssid = p_ssid;
      pass = p_pass;
      key = API;
    }
    //URI for Google GeoLocation API...
    const char* Host = "www.googleapis.com";
    String thisPage = "/geolocation/v1/geolocate?key=";

    int status = WL_IDLE_STATUS;
    String jsonString = "{\n";

    double latitude    = 0.0;
    double longitude   = 0.0;
    double accuracy    = 0.0;

    void start()   {
      if(debug){
        Serial.println("Start");
      }
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      if(debug){
      Serial.println("Setup done");
      Serial.print("Connecting to ");
    }
      WiFi.begin(ssid, pass);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if(debug)Serial.print(".");
      }
      if(debug)Serial.println(".");


    }


    String Locate() {
      char bssid[6];
      String res;
      DynamicJsonBuffer jsonBuffer;
      // if(debug)Serial.println("scan start");
      int n = WiFi.scanNetworks();
      // if(debug)Serial.println("scan done");
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

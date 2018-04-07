#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Configuration.cpp"


class serverHandler{

public:

const char *AP_ssid;
const char *AP_password;
String conf;
bool debug;

ESP8266WebServer *server;
Configuration *config;

serverHandler(int port=80, bool d=false){
  server = new ESP8266WebServer(port);
  config = new Configuration(d);
  debug = d;
  AP_ssid = "Aid-001";
  AP_password = "prototype1";
}



void putConfig(){
  conf = server->arg("plain");
  bool res = config->SaveConfig(conf);
  if(res)
  server->send(200,"text/json","{success: true}");
  else
  server->send(200,"text/json","{success: false}");
}

void getConfig(){
  config->load();
  server->send(200,"text/json",config->config);

}

void handleRoot() {
  config->load();
  String page = "<h1>Welcome to AID-deice</h1><br><h2>" + String(config->devID) + "</h2>";
  server->send(200, "text/html", page);
}

void start() {

  delay(1000);
  if(debug){
  Serial.println();
  Serial.println("Configuring access point...");
}
  WiFi.softAP(AP_ssid, AP_password);

  IPAddress myIP = WiFi.softAPIP();
  if(debug){
  Serial.print("IP address: ");
  Serial.println(myIP);
  Serial.print("SSID: ");
  Serial.println(AP_ssid);
  Serial.print("Password: ");
  Serial.println(AP_password);
}
  server->on("/", std::bind(&serverHandler::handleRoot, this));
  server->on("/config/set", HTTP_POST, std::bind(&serverHandler::putConfig, this));
  server->on("/config/get", HTTP_POST, std::bind(&serverHandler::getConfig, this));
  server->begin();
  if(debug) Serial.println("HTTP server started");
}

void Handle() {
  server->handleClient();
}
};

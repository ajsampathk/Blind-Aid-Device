#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class serverHandler{

public:

const char *ssid = "Blind-Aid-Device";
const char *password;

ESP8266WebServer *server;
int debug;

serverHandler(int port=80, int d=0){
  server = new ESP8266WebServer(port);
  debug = d;

}

void handleRoot() {
  server->send(200, "text/html", "<h1>You are connected</h1>");
}

void start() {
  delay(1000);
  if(debug){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
}
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  if(debug){
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
  server->on("/", std::bind(&serverHandler::handleRoot, this));
  server->begin();
  if(debug)Serial.println("HTTP server started");
}

void Handle() {
  server->handleClient();
}
};

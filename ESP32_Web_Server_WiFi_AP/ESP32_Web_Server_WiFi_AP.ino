/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include <DNSServer.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "ESPmDNS.h"

// Replace with your network credentials
const char* ssid = "scoreboard";
const char* password = "12345678";

const char* hostname = "myHostname.com";

DNSServer dnsServer;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/html", "Resource not found<br><a href=\"/\">back</a>");
}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p><a href='http://%s'>Click this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.softAP(ssid, password, 1, false, 1);
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Route for root / web page
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound(notFound);

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
 
    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        const char* value_param = p->value().c_str();
        Serial.write(value_param);
        Serial.write("\n");
    }
 
    request->send(200, "text/html", "message received<br><a href=\"/\">back</a>");
  });

//  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/json", "{name1:Team1,score1:0,name2:Team2,score2:0}");
//  });
  
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  // Start server
  server.begin();  

  MDNS.begin(hostname);
  MDNS.addService("http", "tcp", 80);
}
 
void loop() {
  dnsServer.processNextRequest();
}

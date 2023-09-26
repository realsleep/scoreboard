#include "DNSServer.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

const char* ssid = "scoreboard";
const char* password = "12345678";

DNSServer dnsServer;
AsyncWebServer server(80);

const size_t CAPACITY = 1024;

int team_num = 0;
bool timer = false;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/html", "Resource not found<br><a href=\"/\">back</a>");
}

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
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

void init_current_match_json(){
  current_match_json["type"] = "current_match_json";
  current_match_json["name1"] = "GUEST";
  current_match_json["name2"] = "HOME";
  current_match_json["score1"] = 0;
  current_match_json["score2"] = 0;
  current_match_json["isTime"] = false;  
  current_match_json["round_dur"] = 45;  
  current_match_json["rounds_num"] = 2;  
  current_match_json["status"] = "init";  
}

void send_success_response(AsyncWebServerRequest * request) {
  AsyncResponseStream *stream = request->beginResponseStream("application/json");
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["result"] = 0;
  serializeJson(jsonDoc, *stream);
  request -> send(stream);
}

void setup() {
  Serial.begin(115200);
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP(ssid, password, 2, 0, 1);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "RESET";
    Serial.println(jsonStr);
    send_success_response(request);
  });
  
  server.on("/member/count/increment/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "I1";
    Serial.println(jsonStr);
    send_success_response(request);
  });
  
  server.on("/member/count/increment/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "I2";
    Serial.println(jsonStr);
    send_success_response(request);
  });

  server.on("/member/count/decrement/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "D1";
    Serial.println(jsonStr);
    send_success_response(request);
  });
  
  server.on("/member/count/decrement/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "D2";
    Serial.println(jsonStr);
    send_success_response(request);
  });
  
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest * request) {
    String jsonStr = "START";
    Serial.println(jsonStr);
    send_success_response(request);
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();  
}

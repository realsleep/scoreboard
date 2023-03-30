#include "DNSServer.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

int btn_pins[] = {34,35,32,33,26,27};
int btn_state[6];
int btn_last_state[] = {LOW,LOW,LOW,LOW,LOW,LOW};  
unsigned long last_debounce_time[] = {0,0,0,0,0,0};  
unsigned long debounceDelay = 100;

const char* ssid = "scoreboard";
const char* password = "12345678";

DNSServer dnsServer;
AsyncWebServer server(80);

DynamicJsonDocument MATCH_INFO(1024);

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

void init_MATCH_INFO(){
  MATCH_INFO["type"] = "MATCH_INFO";
  MATCH_INFO["name1"] = "GUEST";
  MATCH_INFO["name2"] = "HOME";
  MATCH_INFO["score1"] = 0;
  MATCH_INFO["score2"] = 0;
  MATCH_INFO["isTime"] = false;  
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

  for (int i = 0; i < 6; i++)
    pinMode(btn_pins[i], INPUT);
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP(ssid, password, 2, 0, 1);

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  init_MATCH_INFO();

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    String json;
    serializeJson(MATCH_INFO, json);
    MATCH_INFO["isTime"] = timer; 
    Serial.println(json);

    request->send(200, "text/json", json);
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    int params = request->params();

    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      String name_param = p->name();
      String value_param = p->value();
      if (name_param.indexOf("score") > 0) {
      } else {
        
      }
      MATCH_INFO[name_param] = value_param;
    }
    
    request->redirect("/index.html");
  });

  server.on("/start", HTTP_POST, [](AsyncWebServerRequest * request) {
    timer = true;
    String json;
    serializeJson(MATCH_INFO, json);
    MATCH_INFO["isTime"] = timer; 
    Serial.println(json);
    request->redirect("/index.html");
  });

  server.on("/stop", HTTP_POST, [](AsyncWebServerRequest * request) {
    MATCH_INFO["isTime"] = false;
    request->redirect("/match.html");
  });

  server.on("/match/current/timer/start", HTTP_GET, [](AsyncWebServerRequest * request) {
    timer = true;
    String jsonStr;
    serializeJson(MATCH_INFO, jsonStr);
    MATCH_INFO["isTime"] = timer; 
    MATCH_INFO["status"] = "timer_start"; 
    Serial.println(jsonStr);
    
    send_success_response(request);
  });

  server.on("/match/current/timer/stop", HTTP_GET, [](AsyncWebServerRequest * request) {
    timer = false;
    String jsonStr;
    serializeJson(MATCH_INFO, jsonStr);
    MATCH_INFO["isTime"] = timer; 
    MATCH_INFO["status"] = "timer_stop";
    Serial.println(jsonStr);
    
    send_success_response(request);
  });
  
  server.on("/match/current/timer/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/name/1", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/name/2", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/1", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/2", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/1/increment", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/1/decrement", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/2/increment", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/score/2/decrement", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/timeout", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/swap", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/current/alert", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/all", HTTP_GET, [](AsyncWebServerRequest * request) {
  });
  
  server.on("/match/add", HTTP_GET, [](AsyncWebServerRequest * request) {
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();

  for (int i = 0; i < 6; i++) {
    int btn_input = digitalRead(btn_pins[i]);

    if ((millis() - last_debounce_time[i]) > debounceDelay) {
      if (btn_input != btn_state[i]) {
          btn_state[i] = btn_input;
          
          if (btn_state[i] == HIGH) {
            switch(i){
              case 0 : MATCH_INFO["score2"] = int(MATCH_INFO["score2"]) + 1;break;
              case 1 : MATCH_INFO["score1"] = int(MATCH_INFO["score1"]) + 1;break;
              case 2 : MATCH_INFO["score2"] = int(MATCH_INFO["score2"]) - 1;break;
              case 3 : MATCH_INFO["score1"] = int(MATCH_INFO["score1"]) - 1;break;
              case 4 : init_MATCH_INFO();break;
              case 5 : MATCH_INFO["isTime"] = !MATCH_INFO["isTime"];break;
            }

            String json;
            serializeJson(MATCH_INFO, json);
            Serial.println(json);
          }
        }
    }
    
    btn_last_state[i] = btn_input;
  }
  
  
}

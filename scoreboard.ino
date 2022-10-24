 /*
My comment
 */
 /*P10 led pannel pins to ESP 8266
 A D0
 B D6
 CLK D5
 SCK D3
 R D7
 NOE D8
 GND GND
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <FS.h>


#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>

#ifndef STASSID
#define STASSID "scoreboard"
#define STAPSK  "12345678"
#endif


const int HORIZONTAL_PANEL_NUMBER = 4;
const int VERTICAL_PANEL_NUMBER = 4;
const int HORIZONTAL_PANEL_PIXEL_NUMBER = 32;
const int VERTICAL_PANEL_PIXEL_NUMBER = 16;
const int CHARACTER_WIDTH = 5;
const int CHARACTER_HEIGHT = 7;

const int ONE = 1;
const int ZERO = 0;

const int TEAM_NUBMER = 2;
/* Set these to your desired credentials. */
const char* ssid     = STASSID;
const char* password = STAPSK;
const String QUERY_VAR_NAMES[2] = {"team_number", "name"};

AsyncWebServer server(80);

SPIDMD dmd(HORIZONTAL_PANEL_NUMBER,VERTICAL_PANEL_NUMBER);  // DMD controls the entire display

int SCORE[2] = {0,0};
String NAME[2] = {"Team 1", "Team 2"};
int TIME[2] = {0,0};

//AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/update", [](AsyncWebServerRequest *request,JsonVariant &json) {
//  const JsonObject& jsonObj = json.as<JsonObject>();
//  
//  const char* score1 = jsonObj["score1"];
//  const char* score2 = jsonObj["score2"];
//  const char* team1 = jsonObj["team1"];
//  const char* team2 = jsonObj["team2"];
//  
//  request->send(200,"text/plain", "Done");
//});

void refresh() {
  dmd.clearScreen();
  dmd.drawBox(HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - ONE, VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER - ONE,ZERO,ZERO);
  printName(ZERO,ONE);
  printName(ONE,3);
  printScore(ZERO,ONE);
  printScore(ONE,3);
//  printTime();
}

int getCenterX(int end, int size) {
  return end/2 - CHARACTER_WIDTH/2 * size;
}

//void printTime() {
//  dmd.drawString(getCenterX(HORIZONTAL_PANEL_PIXEL_NUMBER * 2, 4), 3, "TIME");
//  String min = String(TIME[0]);
//  if (min.length() != 2) min = "0" + min;
//  String sec = String(TIME[1]);
//  if (sec.length() != 2) sec = "0" + sec;
//
//  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - CHARACTER_WIDTH * 2 - 1, CHARACTER_HEIGHT + 3, min);
//  dmd.setPixel(66,12);
//  dmd.setPixel(66,13);
//  dmd.setPixel(65,12);
//  dmd.setPixel(65,13);
//  dmd.setPixel(66,15);
//  dmd.setPixel(66,16);
//  dmd.setPixel(65,15);
//  dmd.setPixel(65,16);
//  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 + 4, CHARACTER_HEIGHT + 3, sec);
//  
//}


void printScore(int team_number, int k) {
  String score = String(SCORE[team_number]);
  if (score.length() != 2) score = "0" + score;
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH, VERTICAL_PANEL_PIXEL_NUMBER * 2 + CHARACTER_HEIGHT, score);
}

void printName(int team_number, int k) {
  String name = NAME[team_number];
  int nameSize = name.length() > 7 ? 7 : name.length();
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH * (nameSize - 1), VERTICAL_PANEL_PIXEL_NUMBER * 2, name);
}


// the setup routine runs once when you press reset:
void setup() {
  delay(1000);
  Serial.begin(115200);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("/ path");
    request->send(SPIFFS, "/index.html", "text/html");
  });


    
  server.begin();
//  Serial.println("HTTP server started");

  dmd.setBrightness(20);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
//  refresh();
}

// the loop routine runs over and over again forever:
void loop() {
}

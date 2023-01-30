 /*P10 led pannel pins to ESP 8266
  A D0
  B D6
  CLK D5
  SCK D3
  R D7
  NOE D8  
  GND GND
*/

#include <SPI.h>
#include <DMD2.h>
#include "ArduinoJson.h"
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/Droid_Sans_12.h>
#include <fonts/Droid_Sans_16.h>
#include <fonts/Droid_Sans_24.h>

const int HORIZONTAL_PANEL_NUMBER = 4;
const int VERTICAL_PANEL_NUMBER = 4;
const int HORIZONTAL_PANEL_PIXEL_NUMBER = 32;
const int VERTICAL_PANEL_PIXEL_NUMBER = 16;
const int CHARACTER_WIDTH = 6;
const int CHARACTER_HEIGHT = 8;
const int MARGIN_IN_PX = 3;
const int ONE = 1;
const int ZERO = 0;

const int TEAM_NUBMER = 2;

SPIDMD dmd(HORIZONTAL_PANEL_NUMBER, VERTICAL_PANEL_NUMBER, 15, 16, 12, 0); // DMD controls the entire display

int SCORE[2] = {0, 0};
String NAME[2] = {"Team 1", "Team 2"};
int TIME[2] = {0, 0};
bool isTime = false;
long prevMillis = 0;
long last_sec = 0;
String jsonStr = "";
DynamicJsonDocument json(1024);

void refresh() {
  dmd.clearScreen();
  int x = ZERO;
  int y = ZERO;
  int width = HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - ONE;
  int height = VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER - ONE;
  dmd.drawBox(width, height, x, y);
  printName(ZERO, ONE, SystemFont5x7);
  printName(ONE, 3, SystemFont5x7);
  printScore(ZERO, ONE, Droid_Sans_24);
  printScore(ONE, 3, Droid_Sans_24);
  printTime(Droid_Sans_16);
}

void printTime(const uint8_t* font) {
  dmd.drawFilledBox(HORIZONTAL_PANEL_PIXEL_NUMBER, VERTICAL_PANEL_PIXEL_NUMBER * 2, HORIZONTAL_PANEL_PIXEL_NUMBER * 3 - 1, VERTICAL_PANEL_PIXEL_NUMBER * 3 - 1, GRAPHICS_OFF);
  dmd.selectFont(font);
  String dot = (TIME[1] % 2 == 0) ? ":":"";
  
  char buffer[40];
  sprintf(buffer, "%02d%s", TIME[0], dot);

  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - CHARACTER_WIDTH * 3;
  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2;
  dmd.drawString(x, y, buffer);

  sprintf(buffer, "%02d", TIME[1]);

  x = HORIZONTAL_PANEL_PIXEL_NUMBER * 2;
  y = VERTICAL_PANEL_PIXEL_NUMBER * 2;
  dmd.drawString(x, y, buffer);
}

void printScore(int team_number, int k, const uint8_t* font) {
  dmd.selectFont(font);
  int score = SCORE[team_number];
  char buffer[40];
  sprintf(buffer, "%02d", score);
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - 12;
  int y = VERTICAL_PANEL_PIXEL_NUMBER/2 + MARGIN_IN_PX;
  dmd.drawString(x, y, buffer);
}

void printName(int team_number, int k, const uint8_t* font) {
  dmd.selectFont(font);
  String value = NAME[team_number];
  int length = value.length();
  int nameSize = length > 8 ? 8 : length;
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH * nameSize/2;
  int y = VERTICAL_PANEL_PIXEL_NUMBER/4;
  dmd.drawString(x, y, value);
}

void match() {
  NAME[0] = String(json["name1"]);
  NAME[1] = String(json["name2"]);
  
  SCORE[0] = json["score1"];
  SCORE[1] = json["score2"];
  
  isTime = json["isTime"];
  last_sec = millis();
  
  if (isTime && prevMillis == -1) {
    prevMillis = millis();
  } else if (!isTime) {
    prevMillis = -1;
    TIME[1] = 0;
    TIME[0] = 0;
    printTime(Droid_Sans_16);
  }
  
  refresh();
}

void names(long start_time) {
  for (int i = 0; i < json["length"]; i++) {
    int x = HORIZONTAL_PANEL_PIXEL_NUMBER;
    int y = MARGIN_IN_PX + CHARACTER_HEIGHT * i;
    dmd.drawString(x, y, String(json["name" + String(i)]));
  }
}

void matches() {
  dmd.selectFont(SystemFont5x7);
  dmd.clearScreen();
  dmd.drawString(CHARACTER_WIDTH, MARGIN_IN_PX, "#");
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 0.5, MARGIN_IN_PX, "Team 1");
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 + 2, MARGIN_IN_PX, "Team 2");
  dmd.drawLine(0, MARGIN_IN_PX + CHARACTER_HEIGHT, HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER, MARGIN_IN_PX + CHARACTER_HEIGHT);
  dmd.drawLine(HORIZONTAL_PANEL_PIXEL_NUMBER/2 - 4, 0, HORIZONTAL_PANEL_PIXEL_NUMBER/2 - 4, VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER);
  dmd.drawLine(HORIZONTAL_PANEL_PIXEL_NUMBER * 2,0, HORIZONTAL_PANEL_PIXEL_NUMBER * 2, VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER);
  
  for (int i = 0; i < json["length"]; i++) {
    int x = CHARACTER_WIDTH;
    int y = CHARACTER_HEIGHT * (i+1) * 1.5 + MARGIN_IN_PX;
    dmd.drawString(CHARACTER_WIDTH, CHARACTER_HEIGHT * (i+1) * 1.2 + MARGIN_IN_PX, String(i+1));
    dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 0.5, CHARACTER_HEIGHT * (i+1) * 1.2 + MARGIN_IN_PX, String(json["MATCH" + String(i)][0]));
    dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 + 2, CHARACTER_HEIGHT * (i+1) * 1.2 + MARGIN_IN_PX, String(json["MATCH" + String(i)][1]));
  }
}

void first() {
  dmd.selectFont(Droid_Sans_24);
  String arr[] = {"Connect to WiFi ...","SSID: scoreboard","Password:12345678"};
  
  for (int i = 0; i < 3; i++) {
    int x = HORIZONTAL_PANEL_PIXEL_NUMBER/4;
    int y = VERTICAL_PANEL_PIXEL_NUMBER/2 + i * CHARACTER_HEIGHT * 2;
    dmd.selectFont(SystemFont5x7);
  
    dmd.drawString(x, y, arr[i]);
  }  
}

// the setup routine runs once when you press reset:
void setup() {
  delay(1000);
  
  Serial.begin(115200);
  Serial.println();

  dmd.setBrightness(10);
  dmd.begin();
  
  
  first();

}

void loop() {
  
  if(Serial.available()) {
    jsonStr = Serial.readString();
    jsonStr.trim();
    Serial.println(jsonStr);
    
    deserializeJson(json, jsonStr);
    
    if (String(json["type"]).equals("MATCH_INFO")){
      match();
    } else if (String(json["type"]).equals("NAMES")){
      names(millis());
    } else if (String(json["type"]).equals("MATCHES")){
      isTime = false;
      matches();
    } else if (String(json["type"]).equals("RESET")){
      first();
      isTime = false;
    }
  }

  if (isTime && millis() - last_sec > 1000) {
      int seconds = (millis() - prevMillis) / 1000;
      int minutes = seconds / 60;
      seconds %= 60;

      if (!(seconds == TIME[1]) || !(minutes == TIME[0])) {
        TIME[1] = seconds;
        TIME[0] = minutes;
        printTime(Droid_Sans_16);
      } 
      last_sec = millis();
  }
}

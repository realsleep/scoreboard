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



#include <SPI.h>
#include <DMD2.h>
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
const int CHARACTER_WIDTH = 5;
const int CHARACTER_HEIGHT = 7;

const int ONE = 1;
const int ZERO = 0;

const int TEAM_NUBMER = 2;

SPIDMD dmd(HORIZONTAL_PANEL_NUMBER, VERTICAL_PANEL_NUMBER, 15, 16, 12, 0); // DMD controls the entire display

String SCORE[2] = {"0", "0"};
String NAME[2] = {"TEAM 1", "TEAM 2"};
//String TIME[2] = {"0", "0"};
String ARR[4] = {"TEAM 1", "0", "TEAM 2", "0"};

//bool timer_mode = false;
//long start_time = 0;

void refresh() {
  dmd.clearScreen();
  int x = ZERO;
  int y = ZERO;
  int width = HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - ONE;
  int height = VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER - ONE;
  dmd.drawBox(width, height, x, y);
  printName(ZERO, ONE);
  printName(ONE, 3);
  printScore(ZERO, ONE);
  printScore(ONE, 3);
//  printTime();
}

//void printTime() {
//  dmd.selectFont(Droid_Sans_16);
////  dmd.drawString(getCenterX(HORIZONTAL_PANEL_PIXEL_NUMBER * 2, 4), 3, "TIME");
//  String min = TIME[0];
//  if (min.length() != 2) min = "0" + min;
//  String sec = TIME[1];
//  if (sec.length() != 2) sec = "0" + sec;
//  String value = min + ":" + sec;
//  int length = value.length();
//  int nameSize = length;// > 7 ? 7 : length;
//
//  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - (CHARACTER_WIDTH+1) * (nameSize - 1)/2;
//  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2;
//  dmd.drawString(x, y, value);
//
//}

void printScore(int team_number, int k) {
  dmd.selectFont(Droid_Sans_24);
  String score = SCORE[team_number];
  if (score.length() != 2) score = "0" + score;
  int length = score.length();
  int nameSize = length;// > 7 ? 7 : length;
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - (CHARACTER_WIDTH+1) * (nameSize - 1);
  int y = 3 + CHARACTER_HEIGHT;
  dmd.drawString(x, y, score);
}

void printName(int team_number, int k) {
  dmd.selectFont(SystemFont5x7);
  String value = NAME[team_number];
  int length = value.length();
  int nameSize = length;// > 7 ? 7 : length;
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - (CHARACTER_WIDTH+1) * (nameSize - 1)/2;
  int y = 3;
  dmd.drawString(x, y, value);
}

void split(String str) {
   int index = -1;
   for (int i = 0; i < 4; i++) {
      int start_i = index + 1;
      index = str.indexOf('\n', index + 1);
      String sub_S = str.substring(start_i,index);
      ARR[i] = sub_S;
   }
}

//void mar(String value) {
//  
//}

// the setup routine runs once when you press reset:
void setup() {
  delay(1000);
  Serial.begin(115200);

  dmd.setBrightness(10);
  dmd.begin();
  dmd.selectFont(Droid_Sans_24);
  String value = "Technopark";
//  mar(value);

  int length = value.length();
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER;
  int y = VERTICAL_PANEL_PIXEL_NUMBER * 3 / 2;
  for (int index = 0; index < HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - 7; index++) {
    Serial.println(index);
    dmd.clearScreen();
    dmd.drawString(x - index, y, value);
  }

  x = x - HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - 8;
  
  dmd.clearScreen();
  dmd.drawString(x, y, value);
  dmd.clearScreen();
  dmd.drawString(x, y, value);
  
  for (int index = 0; index < HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - 7; index++) {
    Serial.println(index);
    dmd.clearScreen();
    dmd.drawString(x - index, y, value);
  }
w
  value = "Connect to WIFI";
  length = value.length();
  x = HORIZONTAL_PANEL_PIXEL_NUMBER/2;
  y = VERTICAL_PANEL_PIXEL_NUMBER * 2 - 8;
  dmd.selectFont(SystemFont5x7);

  dmd.drawString(x, y, value);
  
  refresh();
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available())  {
    String value = Serial.readString();
    value.trim();
    split(value);
    NAME[0] = ARR[0];
    NAME[1] = ARR[1];
    SCORE[0] = ARR[2];
    SCORE[1] = ARR[3];
    refresh();
  }

//  if (timer_mode) {
//    int seconds = millis() / 1000;
//    int minutes = seconds / 60;
//    seconds %= 60;
//  
//    TIME[1] = String(seconds);
//    TIME[0] = String(minutes);
//    printTime();
//    delay(1000); 
//  }
}

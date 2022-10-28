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
String NAME[2] = {"TEAMTEAM", "TEAM"};
String ARR[4] = {"TEAM 1", "0", "TEAM 2", "0"};

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
}

void printScore(int team_number, int k) {
  String score = SCORE[team_number];
  if (score.length() != 2) score = "0" + score;
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH;
  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2 + CHARACTER_HEIGHT;
  dmd.drawString(x, y, score);
}

void printName(int team_number, int k) {
  String value = NAME[team_number];
  int length = value.length();
  int nameSize = length;// > 7 ? 7 : length;
  int x = HORIZONTAL_PANEL_PIXEL_NUMBER * k - (CHARACTER_WIDTH+1) * (nameSize - 1)/2;
  int y = VERTICAL_PANEL_PIXEL_NUMBER * 2;
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

// the setup routine runs once when you press reset:
void setup() {
  delay(1000);
  Serial.begin(115200);

  dmd.setBrightness(20);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  refresh();
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available())  {
    String value = Serial.readString();
    value.trim();
    split(value);
    NAME[0] = ARR[0];
    SCORE[0] = ARR[1];
    NAME[1] = ARR[2];
    SCORE[1] = ARR[3];
    refresh();
  }
}

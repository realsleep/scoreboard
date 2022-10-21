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
#include <ESP8266WebServer.h>


#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>

#ifndef STASSID
#define STASSID "TechnoPark"
#define STAPSK  "beksbeks"
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
const String HTML ="<!DOCTYPE html> <html> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Scoreboard</title> <style type=\"text/css\"> .container { display: flex; justify-content: space-around; text-align: center; } .score_container { display: flex; justify-content: space-evenly; align-items: center; } </style> </head> <body> <div class=\"container\"> <div class=\"team_0\"> <div class=\"name_container\"> <h1 class=\"name\"></h1> </div> <div class=\"name_input_container\"> <label for=\"name_input\">Team name:</label> <input type=\"text\" name=\"name_input\" maxlength=\"10\" value=\"\" class=\"name_input\"> </div> <div class=\"score_container\"> <button class=\"add_btn\">+</button> <p class=\"score\"></p> <button class=\"subtract_btn\">-</button> </div> </div> <div class=\"team_1\"> <div class=\"name_container\"> <h1 class=\"name\"></h1> </div> <div class=\"name_input_container\"> <label for=\"name_input\">Team name:</label> <input type=\"text\" name=\"name_input\" maxlength=\"10\" value=\"\" class=\"name_input\"> </div> <div class=\"score_container\"> <button class=\"add_btn\">+</button> <p class=\"score\"></p> <button class=\"subtract_btn\">-</button> </div> </div> </div> <script type=\"text/javascript\"> const team_score = document.getElementsByClassName(\"score\"); const team_name = document.getElementsByClassName(\"name\"); const team_name_input = document.getElementsByClassName(\"name_input\"); const team_add_btn = document.getElementsByClassName(\"add_btn\"); const team_subtract_btn = document.getElementsByClassName(\"subtract_btn\"); for (var i = 0; i < team_name_input.length; i++) { team_name_input[i].addEventListener(\"input\", nameEventHandler); } for (var i = 0; i < team_add_btn.length; i++) { team_add_btn[i].addEventListener(\"click\", scoreEventHandler); } for (var i = 0; i < team_subtract_btn.length; i++) { team_subtract_btn[i].addEventListener(\"click\", scoreEventHandler); } for (var i = 0; i < 2; i++) { updateScore(\"/score\", i); } for (var i = 0; i < 2; i++) { updateName(\"/name\", i, \"\"); } async function nameEventHandler(event) { var indexOfTeam = event.path[2].className.at(5); var input_from_user = event.target.value; var path = event.srcElement.className; updateName(path, indexOfTeam, input_from_user); } async function updateName(path, indexOfTeam, input_from_user) { const newName = await requestNewName(path, indexOfTeam, input_from_user); team_name[indexOfTeam].textContent = newName; } async function requestNewName(path, indexOfTeam, input_from_user) { const response = await fetch(path + \"?team_number=\"+ indexOfTeam + \"&name=\" + input_from_user); const newName = await response.text(); return newName; } async function scoreEventHandler(event) { var indexOfTeam = event.path[2].className.at(5); var path = event.srcElement.className; updateScore(path, indexOfTeam); } async function updateScore(path, indexOfTeam) { var score = team_score[indexOfTeam]; var newScore = await requestNewScore(path, indexOfTeam); score.textContent = newScore; } async function requestNewScore(path, indexOfTeam) { const response = await fetch(path + \"?team_number=\"+ indexOfTeam); const newScore = await response.text(); return newScore; } </script> </body> </html>";

ESP8266WebServer server(80);

SPIDMD dmd(HORIZONTAL_PANEL_NUMBER,VERTICAL_PANEL_NUMBER);  // DMD controls the entire display

int SCORE[2] = {0,0};
String NAME[2] = {"Team 1", "Team 2"};
int TIME[2] = {0,0};

void refresh() {
  dmd.clearScreen();
  dmd.drawBox(HORIZONTAL_PANEL_PIXEL_NUMBER * HORIZONTAL_PANEL_NUMBER - ONE, VERTICAL_PANEL_PIXEL_NUMBER * VERTICAL_PANEL_NUMBER - ONE,ZERO,ZERO);
  printName(ZERO,ONE);
  printName(ONE,3);
  printScore(ZERO,ONE);
  printScore(ONE,3);
  printTime();
}

int getCenterX(int end, int size) {
  return end/2 - CHARACTER_WIDTH/2 * size;
}

void printTime() {
  dmd.drawString(getCenterX(HORIZONTAL_PANEL_PIXEL_NUMBER * 2, 4), 3, "TIME");
  String min = String(TIME[0]);
  if (min.length() != 2) min = "0" + min;
  String sec = String(TIME[1]);
  if (sec.length() != 2) sec = "0" + sec;

  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 - CHARACTER_WIDTH * 2 - 1, CHARACTER_HEIGHT + 3, min);
  dmd.setPixel(66,12);
  dmd.setPixel(66,13);
  dmd.setPixel(65,12);
  dmd.setPixel(65,13);
  dmd.setPixel(66,15);
  dmd.setPixel(66,16);
  dmd.setPixel(65,15);
  dmd.setPixel(65,16);
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * 2 + 4, CHARACTER_HEIGHT + 3, sec);
  
}

void handleRoot() {
  server.send(200, "text/html", HTML);
}

void addScoreTeam() {
  int team_number = server.arg(QUERY_VAR_NAMES[ZERO]).toInt();
  SCORE[team_number]++;
  server.send(200, "text/html", String(SCORE[team_number]));
  refresh();
}

void subtractScoreTeam() {
  int team_number = server.arg(QUERY_VAR_NAMES[ZERO]).toInt();
  SCORE[team_number]--;
  if (SCORE[team_number] < ZERO) SCORE[team_number] = ZERO;
  server.send(200, "text/html", String(SCORE[team_number]));
  refresh();
}

void updateName() {
  int team_number = server.arg(QUERY_VAR_NAMES[ZERO]).toInt();
  NAME[team_number] = server.arg(QUERY_VAR_NAMES[ONE]);
  if (NAME[team_number].length() == ZERO) NAME[team_number] = "Team " + String(team_number+1);
  server.send(200, "text/html", String(NAME[team_number]));
  refresh();
}

void printScore(int team_number, int k) {
  String score = String(SCORE[team_number]);
  if (score.length() != 2) score = "0" + score;
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH, VERTICAL_PANEL_PIXEL_NUMBER * 2 + CHARACTER_HEIGHT, score);
}

void printName(int team_number, int k) {
  String name = NAME[team_number];
  int nameSize = name.length() > 7 ? 7 : name.length();
  dmd.drawString(HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH * (nameSize - 1), VERTICAL_PANEL_PIXEL_NUMBER * 2, name);
  Serial.print(HORIZONTAL_PANEL_PIXEL_NUMBER * k - CHARACTER_WIDTH * (nameSize - 1));
  Serial.print(" ");
  Serial.println(VERTICAL_PANEL_PIXEL_NUMBER * 2);
}

void getScore() {
  int team_number = server.arg(QUERY_VAR_NAMES[0]).toInt();
  server.send(200, "text/html", String(SCORE[team_number]));  
}

void getName() {
  int team_number = server.arg(QUERY_VAR_NAMES[0]).toInt();
  server.send(200, "text/html", String(NAME[team_number]));
}

// the setup routine runs once when you press reset:
void setup() {
  delay(1000);
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/add_btn", addScoreTeam);
  server.on("/subtract_btn", subtractScoreTeam);
  server.on("/name_input", updateName);
  server.on("/score", getScore);
  server.on("/name", getName);
    
  server.begin();
  Serial.println("HTTP server started");

  dmd.setBrightness(255);
  dmd.selectFont(SystemFont5x7);
  dmd.begin();
  refresh();
}

// the loop routine runs over and over again forever:
void loop() {
  server.handleClient(); 
}

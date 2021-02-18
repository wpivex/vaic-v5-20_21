/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) Innovation First 2020 All rights reserved.                */
/*    Licensed under the MIT license.                                         */
/*                                                                            */
/*    Module:     dashboard.cpp                                               */
/*    Author:     James Pearman                                               */
/*    Created:    20 August 2020                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include <math.h>
#include "vex.h"
#include "map.h"

#include<string>
#include<sstream>

using namespace vex;

const color grey = vex::color(0x404040);
const color darkred = vex::color(0x800000);
const color darkgreen = vex::color(0x008000);

const int PX_PER_FT = 20; // 240px for 12ft

// Draws the static objects on the field, the grey background and the nine goals.
static void drawFieldBackground() {
  // Draw Field Carpet
  Brain.Screen.setPenColor(black);
  Brain.Screen.drawRectangle(0, 0, 240, 240, grey);

  // Draw 9 goals
  for (int xGoal = 0; xGoal < 3; xGoal++) { // lower numbers are to the left
    for (int yGoal = 0; yGoal < 3; yGoal++) { // lower numbers are towards the top
      int xCenter = xGoal * 120 + (1 - xGoal) * 9;
      int yCenter = yGoal * 120 + (1 - yGoal) * 9;

      Brain.Screen.drawCircle(xCenter, yCenter, 8, black);
      Brain.Screen.drawCircle(xCenter, yCenter, 6, grey);
    }
  }
}

// Draws packet stats from the Jetson on the right side of the screen.
static void drawJetsonStats() {
  static MAP_RECORD local_map;

  static int32_t last_packets = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;

  jetson_comms.get_data(&local_map);

  // Once per second, update data rate stats
  if(Brain.Timer.system() > update_time) {
    update_time = Brain.Timer.system() + 1000;
    total_packets = jetson_comms.get_packets() - last_packets;
    last_packets = jetson_comms.get_packets();
  }

  // Draw Stats screen background
  Brain.Screen.setPenColor(yellow);
  Brain.Screen.drawRectangle(240, 0, 240, 240, black);

  Brain.Screen.drawRectangle(240, 0, 240, 20, grey);
  Brain.Screen.setFillColor(grey);
  Brain.Screen.printAt(250, 15, "Stats");

  Brain.Screen.setFillColor(black);
  Brain.Screen.printAt(250, 155, "Local Location:");

  // Color = status: red = no packets, green = packets
  Brain.Screen.setPenColor(total_packets == 0 ? red : green); 
  Brain.Screen.printAt(250, 35, true, "Jetson%s:", total_packets == 0 ? "(Disconnected)" : "(Connected)");

  // Print stats on the right
  Brain.Screen.setPenColor(yellow);

  int xText = 260, yText = 50;

  Brain.Screen.printAt(xText, yText, "pkts/sec  %d", total_packets);
  Brain.Screen.printAt(xText, yText += 15, "Errors    %d", jetson_comms.get_errors());    
  Brain.Screen.printAt(xText, yText += 15, "Timeouts  %d", jetson_comms.get_timeouts());
} // drawJetsonStats()

// Draws the manager and balls on the map as well as the manager's coords on the right side
void drawManagerAndBalls(Map* map) {
  int xText = 260, yText = 80;

  RobotCoord posData = map->getManagerCoords();

  Brain.Screen.printAt(xText, yText += 90, "x(ft): %.2f  y(ft): %.2f", posData.x / 12, posData.y / 12);
  Brain.Screen.printAt(xText, yText += 15, "heading(deg): %.2f", posData.deg);

  // Draw map w/ balls and manager robot
  for(int i = 0; i < map->getNumBalls(); i++) {
    // positionX and positionY have 0,0 in the middle of the field w/ +x right and +y down
    int xCenter = (int) (map->getBallCoords()[i].x / 12 * PX_PER_FT + 119); // in -> px
    int yCenter = (int) (map->getBallCoords()[i].y / 12 * PX_PER_FT + 119); // in -> px

    color ballColor = map->getBallCoords()[i].colorID == 0 ? red : blue; // class ID 0 = red and 1 = blue
    Brain.Screen.setPenColor(ballColor); 
    Brain.Screen.setFillColor(ballColor);
    Brain.Screen.drawCircle(xCenter, yCenter, 5);

    // TODO do something with age?
    // TODO do something with height?
  }

  Brain.Screen.setFillColor(black);

  // Draw a rectangle for robot and line from the center for heading
  // TODO draw rotated rectangle to display orientation instead of a line
  Brain.Screen.setPenColor(black);

  int xRobot = (int) (posData.x / 12 * PX_PER_FT + 119); // coords for center of robot
  int yRobot = (int) (posData.y / 12 * PX_PER_FT + 119);
  Brain.Screen.drawRectangle(xRobot - 20, yRobot - 20, 40, 40, green); // 24in robot
  // TODO fix below based on field data and az conventions
  Brain.Screen.drawLine(xRobot, yRobot, xRobot + (int) (40 * cos(posData.deg * M_PI / 360)), yRobot - (int) (40 * sin(posData.deg * M_PI / 360)));
} // drawManager(Map)

// Draws map and stats for worker robot as well as general Vex Link data
static void drawWorker(Map* map) {
  static int32_t last_packets = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;

  // Once per second, update data rate stats
  if( Brain.Timer.system() > update_time ) {
    update_time = Brain.Timer.system() + 1000;
    total_packets = link.get_packets() - last_packets;
    last_packets = link.get_packets();
  }

  int xText = 260, yText = 200;

  // Only draw alliance robot and print remote robot data if vex link is working
  if (map->getWorkerCoords().robotID != -1) {
    Brain.Screen.setPenColor(green);
    Brain.Screen.printAt(250, 95, "Vex Link(Connected):");

    float x = map->getWorkerCoords().x * 12;
    float y = map->getWorkerCoords().y * 12;
    float deg = map->getWorkerCoords().deg;
    float rad = deg * M_PI / 360;

    // Print remote robot data
    Brain.Screen.setPenColor(yellow);

    Brain.Screen.printAt(xText - 10, yText, "Remote Location:");
    Brain.Screen.printAt(xText, yText += 15, "x(ft): %.2f  y(ft): %.2f", x, y);
    Brain.Screen.printAt(xText, yText += 15, "heading(deg): %.2f", deg);

    // Draw a rectangle for robot and line from the center for heading
    // TODO draw rotated rectangle to display orientation instead of a line
    Brain.Screen.setPenColor(black);
    // positionX and positionY have 0,0 in the middle of the field w/ +x right and +y down
    // TODO check signs
    int xRobot = (int) (x * PX_PER_FT + 119); // coords for top left corner
    int yRobot = (int) (y * PX_PER_FT + 119);
    Brain.Screen.drawRectangle(xRobot - 13, yRobot - 13, 26, 26, cyan); // 15in robot
    // TODO fix below based on field data and az conventions
    Brain.Screen.drawLine(xRobot, yRobot, xRobot + (int) (40 * cos(rad)), yRobot - (int) (40 * sin(rad)));
  } else {
    Brain.Screen.setPenColor(red);
    Brain.Screen.printAt(250, 95, "Vex Link(Disconnected):");
  }

  // Print out Vex Link general stats
  yText = 110;

  Brain.Screen.setPenColor(yellow);
  Brain.Screen.printAt(xText, yText, "pkts/sec  %d", total_packets);
  Brain.Screen.printAt(xText, yText += 15, "Errors    %d", link.get_errors());
  Brain.Screen.printAt(xText, yText += 15, "Timeouts  %d", link.get_timeouts());
} // drawWorker()

void updateMapObj(Map* map) {
  MAP_RECORD mapRecord; // Map from the Jetson

  jetson_comms.get_data(&mapRecord);

  // get ball data from mapRecord
  int numBalls = mapRecord.mapnum;
  BallCoord balls[numBalls];

  for (int i = 0; i < numBalls; i++) {
    float x = (mapRecord.mapobj[i].positionX / -25.4); // hopefully in to the right of (0,0), need to test on field
    float y = (mapRecord.mapobj[i].positionY / 25.4); // hopefully in above of (0,0), need to test on field
    balls[i] = {mapRecord.mapobj[i].age, mapRecord.mapobj[i].classID, x, y};
  }

  map->setBallCoords(balls, numBalls);

  // get manager robot data from mapRecord and worker data from vex link coords
  RobotCoord robots[2];
  int numRobots = 1;

  robots[0] = {
    0, // manager
    mapRecord.pos.x / -25.4f, // hopefully in to the right of (0,0), need to test on field
    mapRecord.pos.y / 25.4f, // hopefully in above of (0,0), need to test on field
    (float) (mapRecord.pos.az * 360 / (2 * M_PI) + 90), // hopefully starts at +x and increases counterclockwise, need to test on field
    24 // 24 in
  };

  if (link.isLinked()) {
    float workerX, workerY, workerHeading;
    link.get_remote_location(workerX, workerY, workerHeading);

    robots[1] = {
      1, // worker
      workerX / -25.4f, // hopefully in to the right of (0,0), need to test on field
      workerY / 25.4f, // hopefully in above of (0,0), need to test on field
      (float) (workerHeading * 360 / (2 * M_PI) + 90), // hopefully starts at +x and increases counterclockwise, need to test on field
      15 // 15 in
    };

    numRobots++;
  }

  map->setRobotCoords(robots, numRobots);
} // updateMapObj(Map)

void drawFromMap(Map* map) {
  drawFieldBackground();
  drawJetsonStats();

  drawManagerAndBalls(map);
  drawWorker(map);

  Brain.Screen.render(); 
} // drawFromMap(Map)

// Task to update screen with status
int dashboardTask() {
  Brain.Screen.setFont(mono15);

  Map map; // Internal map class

  while (true) {
    updateMapObj(&map);
    drawFromMap(&map);

    this_thread::sleep_for(16);
  }
  
  // while(true) {
  //   drawFieldBackground();
  //   drawFromJetson();
  //   drawFromVexLink();
  //   Brain.Screen.render(); 

  //   this_thread::sleep_for(16);
  // }

  return 0;
}
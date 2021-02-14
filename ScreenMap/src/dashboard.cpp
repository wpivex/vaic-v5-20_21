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
#include "vex.h"
#include <math.h>

using namespace vex;

const color grey = vex::color(0x404040);
const color darkred = vex::color(0x800000);
const color darkgreen = vex::color(0x008000);

const int PX_PER_FT = 20; // 240px for 12ft

// Draws everything which does not need to be redrawn
static void initialDraw() {
  Brain.Screen.setFont(mono15);

  // Draw stats screen to the right of the map
  Brain.Screen.setPenColor(yellow);
  Brain.Screen.drawRectangle(240, 0, 240, 240, black);

  Brain.Screen.drawRectangle(240, 0, 240, 20, grey);
  Brain.Screen.setFillColor(grey);
  Brain.Screen.printAt(250, 15, "Stats");

  Brain.Screen.setFillColor(black);
  Brain.Screen.printAt(250, 35, "Jetson:");
  Brain.Screen.printAt(250, 140, "Local Location:");
}

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

// Draws everything from the Jetson: 
// Map and stats for balls and local robot as well as general Jetson data
static void drawFromJetson() {
  static MAP_RECORD local_map;

  static int32_t last_packets = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;

  jetson_comms.get_data(&local_map);
  POS_RECORD posData = local_map.pos;

  // once per second, update data rate stats
  if(Brain.Timer.system() > update_time) {
    update_time = Brain.Timer.system() + 1000;
    total_packets = jetson_comms.get_packets() - last_packets;
    last_packets = jetson_comms.get_packets();
  }

  // Print stats on the right
  Brain.Screen.setPenColor(yellow);

  int xText = 260, yText = 50;

  Brain.Screen.printAt(xText, yText, "pkts/sec  %d", total_packets);
  Brain.Screen.printAt(xText, yText += 15, "Errors    %d", jetson_comms.get_errors());    
  Brain.Screen.printAt(xText, yText += 15, "Timeouts  %d", jetson_comms.get_timeouts());

  Brain.Screen.printAt(xText, yText += 75, "x: %.2f  y: %.2f", posData.x, posData.y);
  // TODO fix below based on field data and az conventions
  Brain.Screen.printAt(xText, yText += 15, "heading: %.2f", posData.az / (2 * M_PI ) * 360);

  // Draw map w/ balls and local robot
  for(int i = 0; i < local_map.mapnum; i++) {
    // positionX and positionY have 0,0 in the middle of the field w/ +x right and +y down
    int xCenter = (int) ((local_map.mapobj[i].positionX / -25.4) / 12 * PX_PER_FT + 119); // mm -> px
    int yCenter = (int) ((local_map.mapobj[i].positionY / -25.4) / 12 * PX_PER_FT + 119); // mm -> px

    color ballColor = local_map.mapobj[i].classID == 0 ? red : blue; // class ID 0 = red and 1 = blue
    Brain.Screen.setPenColor(ballColor); 
    Brain.Screen.setFillColor(ballColor);
    Brain.Screen.drawCircle(xCenter, yCenter, 5);

    // TODO do something with age?
    // TODO do something with height?
  }

  // Draw a rectangle for robot and line from the center for heading
  // TODO draw rotated rectangle to display orientation instead of a line
  Brain.Screen.setPenColor(black);
  // positionX and positionY have 0,0 in the middle of the field w/ +x right and +y down
  int xRobot = (int) ((posData.x / -25.4) / 12 * PX_PER_FT + 119); // coords for top left corner
  int yRobot = (int) ((posData.y / -25.4) / 12 * PX_PER_FT + 119);
  Brain.Screen.drawRectangle(xRobot - 20, yRobot - 20, 40, 40, green); // 24in robot
  // TODO fix below based on field data and az conventions
  Brain.Screen.drawLine(xRobot, yRobot, xRobot + (int) (40 * sin(posData.az)), yRobot - (int) (40 * cos(posData.az)));
} // drawFromJetson()

// Draws everything from Vex Link:
// Map and stats for other alliance robot as well as general Vex Link data
static void drawFromVexLink() {
  Brain.Screen.setPenColor(yellow);
  Brain.Screen.printAt(250, 95, "Vex Link:");
}

// Display various useful information about the Jetson
static void dashboardJetson( int ox, int oy, int width, int height ) {
  static int32_t last_data = 0;
  static int32_t last_packets = 0;
  static int32_t total_data = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;
  static MAP_RECORD local_map;

  Brain.Screen.setClipRegion( ox, oy, width, height);
  Brain.Screen.setFont( mono15 );
  // border and titlebar
  Brain.Screen.setPenColor( yellow );
  Brain.Screen.drawRectangle(ox, oy, width, height, black );
  Brain.Screen.drawRectangle( ox, oy, width, 20, grey );

  Brain.Screen.setPenColor( yellow );
  Brain.Screen.setFillColor( grey );
  Brain.Screen.printAt(ox + 10, oy + 15, "Jetson" );
  oy += 20;
  
  Brain.Screen.setPenColor( white );
  Brain.Screen.setFillColor( black );
  
  // get last map data
  jetson_comms.get_data( &local_map );

  Brain.Screen.printAt( ox + 10, oy += 15, "Packets   %d", jetson_comms.get_packets() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Errors    %d", jetson_comms.get_errors() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Timeouts  %d", jetson_comms.get_timeouts() );
  Brain.Screen.printAt( ox + 10, oy += 15, "data/sec  %d             ", total_data );
  Brain.Screen.printAt( ox + 10, oy += 15, "pkts/sec  %d             ", total_packets );
  Brain.Screen.printAt( ox + 10, oy += 15, "boxnum    %d", local_map.boxnum );
  Brain.Screen.printAt( ox + 10, oy += 15, "mapnum    %d", local_map.mapnum );

  // once per second, update data rate stats
  if( Brain.Timer.system() > update_time ) {
    update_time = Brain.Timer.system() + 1000;
    total_data = jetson_comms.get_total() - last_data;
    total_packets = jetson_comms.get_packets() - last_packets;
    last_data = jetson_comms.get_total();
    last_packets = jetson_comms.get_packets();
  }
  
  Brain.Screen.setFont( mono12 );

  for( int i = 0; i < 4; i++ ) {
    if( i < local_map.boxnum ) {
      Brain.Screen.printAt( ox + 10, oy += 12, "box %d: c:%d x:%d y:%d w:%d h:%d prob:%.1f", i,
          (local_map.boxobj[i].classID), //Class ID (0 = Red 1 = Blue 2 = Goal)
          (local_map.boxobj[i].x), //in pixels
          (local_map.boxobj[i].y), //in pixels
          (local_map.boxobj[i].width), //in pixels
          (local_map.boxobj[i].height), //in pixels
          (local_map.boxobj[i].prob)); //percent likely to be in this catagory
    } else {
      Brain.Screen.printAt( ox + 10, oy += 12, "---");
    }
  }

  for( int i = 0; i < 4; i++ ) {
    if( i < local_map.mapnum ) {
      Brain.Screen.printAt( ox + 10, oy += 12, "map %d: a:%4d c:%4d X:%.2f Y:%.2f Z:%.1f",i,
          local_map.mapobj[i].age,
          local_map.mapobj[i].classID,
          (local_map.mapobj[i].positionX / -25.4),  // mm -> inches
          (local_map.mapobj[i].positionY / -25.4),  // mm -> inches
          (local_map.mapobj[i].positionZ / 25.4)); // mm -> inches
    } else {
      Brain.Screen.printAt( ox + 10, oy += 12, "---");
    }
  }
}

// Display various useful information about VEXlink
static void dashboardVexlink( int ox, int oy, int width, int height ) {
  static int32_t last_data = 0;
  static int32_t last_packets = 0;
  static int32_t total_data = 0;
  static int32_t total_packets = 0;
  static uint32_t update_time = 0;

  Brain.Screen.setClipRegion( ox, oy, width, height);
  Brain.Screen.setFont( mono15 );

  // border and titlebar
  Brain.Screen.setPenColor( yellow );
  Brain.Screen.drawRectangle(ox, oy, width, height, black );
  Brain.Screen.drawRectangle( ox, oy, width, 20 );

  // Link status in titlebar
  if( link.isLinked() ) {
    Brain.Screen.setPenColor(darkgreen);
    Brain.Screen.setFillColor(darkgreen);
    Brain.Screen.drawRectangle( ox+1, oy+1, width-2, 18 );
    Brain.Screen.setPenColor(yellow);
    Brain.Screen.printAt( ox + 10, oy + 15, "VEXlink: Good" );
  } else {
    Brain.Screen.setPenColor(darkred);
    Brain.Screen.setFillColor(darkred);
    Brain.Screen.drawRectangle( ox+1, oy+1, width-2, 18 );
    Brain.Screen.setPenColor(yellow);
    Brain.Screen.printAt(ox + 10, oy + 15, "VEXlink: Disconnected" );
  }

  oy += 20;

  Brain.Screen.setFillColor(black);
  Brain.Screen.setPenColor(white);
  Brain.Screen.printAt( ox + 10, oy += 15, "Packets   %d", link.get_packets() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Errors    %d", link.get_errors() );
  Brain.Screen.printAt( ox + 10, oy += 15, "Timeouts  %d", link.get_timeouts() );
  Brain.Screen.printAt( ox + 10, oy += 15, "data/sec  %d  ", total_data);
  Brain.Screen.printAt( ox + 10, oy += 15, "pkts/sec  %d  ", total_packets);

  // once per second, update data rate stats
  if( Brain.Timer.system() > update_time ) {
    update_time = Brain.Timer.system() + 1000;    
    total_data = link.get_total() - last_data;
    total_packets = link.get_packets() - last_packets;
    last_data = link.get_total();
    last_packets = link.get_packets();
  }

  oy += 10;
  Brain.Screen.printAt( ox + 10, oy += 15, "Location: local");
  
  float x,y,heading;
  link.get_local_location(x, y, heading);
  
  Brain.Screen.printAt( ox + 10, oy += 15, " X:   %.2f", x / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " Y:   %.2f", y / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " H:   %.2f", 180 - (heading / (-2 * M_PI ) * 360) ); // rads to deg

  oy += 5;
  Brain.Screen.printAt( ox + 10, oy += 15, "Location: remote");
  
  link.get_remote_location(x, y, heading);
  
  Brain.Screen.printAt( ox + 10, oy += 15, " X:   %.2f", x / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " Y:   %.2f", y / -25.4);  // mm -> inches
  Brain.Screen.printAt( ox + 10, oy += 15, " H:   %.2f", 180 - (heading / (-2 * M_PI ) * 360) ); // rads to deg
}

// Task to update screen with status
int dashboardTask() {
  initialDraw();
  drawFieldBackground();
  drawFromJetson();
  drawFromVexLink();
  Brain.Screen.render();

  // while(true) {
  //   // status
  //   dashboardJetson( 0, 0, 280, 240 );
  //   dashboardVexlink( 279, 0, 201, 240 );
  //   // draw, at 30Hz
  //   Brain.Screen.render();
  //   this_thread::sleep_for(16);
  // }

  return 0;
}
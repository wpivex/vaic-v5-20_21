/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Mon Aug 31 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

#include <math.h>
#include "vex.h"
#include "Drive.h"
#include "commands.h"

using namespace vex;

// A global instance of competition
competition Competition;

Map* map = new Map();

// create instance of jetson class to receive location and other
// data from the Jetson nano
//
ai::jetson  jetson_comms;

/*----------------------------------------------------------------------------*/
// Create a robot_link on PORT1 using the unique name robot_32456_1
// The unique name should probably incorporate the team number
// and be at least 12 characters so as to generate a good hash
//
// The Demo is symetrical, we send the same data and display the same status on both
// manager and worker robots
// Comment out the following definition to build for the worker robot
#define  MANAGER_ROBOT    1

#if defined(MANAGER_ROBOT)
#pragma message("building for the manager")
ai::robot_link       link( PORT4, "robot_32456_1", linkType::manager );
#else
#pragma message("building for the worker")
ai::robot_link       link( PORT4, "robot_32456_1", linkType::worker );
#endif

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Auto_Isolation Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous isolation  */
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/
void auto_Isolation(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Auto_Interaction Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous interaction*/
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/
void auto_Interaction(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          AutonomousMain Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
bool firstAutoFlag = true;

void autonomousMain(void) {
  // ..........................................................................
  // The first time we enter this function we will launch our Isolation routine
  // When the field goes disabled after the isolation period this task will die
  // When the field goes enabled for the second time this task will start again
  // and we will enter the interaction period. 
  // ..........................................................................

  if(firstAutoFlag)
    auto_Isolation();
  else 
    auto_Interaction();

  firstAutoFlag = false;
}

/*
 * Updates the global Map obj declared in vex.h with data from the Jetson and vex link
 */
void updateMapObj() {
  MAP_RECORD mapRecord; // Map from the Jetson

  jetson_comms.get_data(&mapRecord);
  
  jetson_comms.request_map();

  // get ball data from mapRecord
  int numBalls = mapRecord.mapnum;
  BallCoord balls[numBalls];

  for (int i = 0; i < numBalls; i++) {
    float x = (mapRecord.mapobj[i].positionX / -25.4);
    float y = (mapRecord.mapobj[i].positionY / -25.4);
    balls[i] = {mapRecord.mapobj[i].age, mapRecord.mapobj[i].classID, x, y};
  }

  map->setBallCoords(balls, numBalls);

  // get manager robot data from mapRecord and worker data from vex link coords
  RobotCoord robots[2];
  int numRobots = 1;

  float managerHeading = (float) ((-mapRecord.pos.az - M_PI/2) * 360 / (2 * M_PI));
  float managerX = mapRecord.pos.x / -25.4f + POS_OFFSET * cos(managerHeading * M_PI / 180);
  float managerY = mapRecord.pos.y / -25.4f + POS_OFFSET * sin(managerHeading * M_PI / 180);

  robots[0] = {
    0, // manager
    managerX, // hopefully in to the right of (0,0), need to test on field
    managerY, // hopefully in above of (0,0), need to test on field
    managerHeading, // starts at +x and increases counterclockwise, range of (-270 : 90)
    24 // 24 in
  };

  link.set_remote_location(robots[0].x, robots[0].y, robots[0].deg);

  if (link.isLinked()) {
    float workerX, workerY, workerHeading;
    link.get_remote_location(workerX, workerY, workerHeading);

    // robots[1] = {
    //   1, // worker
    //   workerX / -25.4f, // hopefully in to the right of (0,0), need to test on field
    //   workerY / -25.4f, // hopefully in above of (0,0), need to test on field
    //   (float) (270 - ((workerHeading * 360 / (2 * M_PI)))), // hopefully starts at +x and increases counterclockwise, need to test on field
    //   15 // 15 in
    // };
    robots[1] = {
      1, // worker
      workerX, // hopefully in to the right of (0,0), need to test on field
      workerY, // hopefully in above of (0,0), need to test on field
      workerHeading, // hopefully starts at +x and increases counterclockwise, need to test on field
      15 // 15 in
    };

    numRobots++;
  }

  map->setRobotCoords(robots, numRobots);
} // updateMapObj()

int main() {
  vexcodeInit(); // Initializing Robot Configuration. DO NOT REMOVE!

  int32_t loop_time = 66; // Run at about 15Hz

  thread t1 = thread(dashboardTask); // start the status update display

  Competition.autonomous(autonomousMain); // Set up callbacks for autonomous and driver control periods.
    
  Drive* drive = new Drive();

  while(1) {
    updateMapObj();

    drive->setPose({ 
      map->getManagerCoords().x,
      map->getManagerCoords().y,
      map->getManagerCoords().deg
    });

    /*Brain.Screen.clearScreen();
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("Drive theta:");
    Brain.Screen.print(drive->getPose().theta);*/

    /*
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("Left Encoder:");
    Brain.Screen.print(leftInches());
    Brain.Screen.print(" Right Encoder:");
    Brain.Screen.print(rightInches());

    yeet.spin(directionType::fwd,100,percentUnits::pct);*/

    this_thread::sleep_for(loop_time); // Allow other tasks to run
  }
}

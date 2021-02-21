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

#include "vex.h"

#include "Drive.h"

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
ai::robot_link       link( PORT11, "robot_32456_1", linkType::manager );
#else
#pragma message("building for the worker")
ai::robot_link       link( PORT11, "robot_32456_1", linkType::worker );
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

void updateMapObj() {
  MAP_RECORD mapRecord; // Map from the Jetson

  jetson_comms.get_data(&mapRecord);

  link.set_remote_location(mapRecord.pos.x, mapRecord.pos.y, mapRecord.pos.az);
  
  jetson_comms.request_map();

  FILE *fp = fopen("/dev/serial2", "w");

  fprintf(fp, "%f %f\n", mapRecord.mapobj[0].positionX, mapRecord.mapobj[0].positionY);

  fclose(fp);

  // get ball data from mapRecord
  int numBalls = mapRecord.mapnum;
  BallCoord balls[numBalls];

  for (int i = 0; i < numBalls; i++) {
    float x = (mapRecord.mapobj[i].positionX / -25.4); // hopefully in to the right of (0,0), need to test on field
    float y = (mapRecord.mapobj[i].positionY / -25.4); // hopefully in above of (0,0), need to test on field
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
    (float) ((-mapRecord.pos.az - M_PI/2) * 360 / (2 * M_PI)), // starts at +x and increases counterclockwise, range of (-270 : 90)
    24 // 24 in
  };

  if (link.isLinked()) {
    float workerX, workerY, workerHeading;
    link.get_remote_location(workerX, workerY, workerHeading);

    robots[1] = {
      1, // worker
      workerX / -25.4f, // hopefully in to the right of (0,0), need to test on field
      workerY / -25.4f, // hopefully in above of (0,0), need to test on field
      (float) (270 - ((workerHeading * 360 / (2 * M_PI)))), // hopefully starts at +x and increases counterclockwise, need to test on field
      15 // 15 in
    };

    numRobots++;
  }

  map->setRobotCoords(robots, numRobots);
} // updateMapObj()

/*----------------------------------------------------------------------------*/

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Run at about 15Hz
  int32_t loop_time = 66;

  // start the status update display
  thread t1 = thread(dashboardTask);

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomousMain);
    
  // Drive* drive = new Drive();
  
  // Pose p;
  // p.x = 24;
  // p.y = 24;
  // p.theta = 0;

  //drive->goTo(p);

  while(1) {
    updateMapObj();

    // Allow other tasks to run
    this_thread::sleep_for(loop_time);

    /*
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("Left Encoder:");
    Brain.Screen.print(leftInches());
    Brain.Screen.print(" Right Encoder:");
    Brain.Screen.print(rightInches());

    yeet.spin(directionType::fwd,100,percentUnits::pct);

    this_thread::sleep_for(100);*/
  }
}

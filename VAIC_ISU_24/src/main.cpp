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
#include <vex_units.h>
#include "Drive.h"
#include "commands.h"

using namespace vex;

// A global instance of competition
competition Competition;

Map* map = new Map();
Drive* drive = new Drive();

// create instance of jetson class to receive location and other data from the Jetson nano
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

int main() {
  vexcodeInit(); // Initializing Robot Configuration. DO NOT REMOVE!

  int32_t loop_time = 33; // Run at about 15Hz

  thread t1 = thread(dashboardTask); // start the status update display

  Competition.autonomous(autonomousMain); // Set up callbacks for autonomous and driver control periods.

  State robotState = startup;

  FILE *fp = fopen("/dev/serial2", "w");

  while(1) {
    updateMapObj();

    drive->setPose({ 
      map->getManagerCoords().x,
      map->getManagerCoords().y,
      map->getManagerCoords().deg
    });

    switch (robotState) {
      case startup:
        robotState = lookForBalls;

        fprintf(fp, "%d\n", robotState);
        fflush(fp);

        break;
      case lookForBalls:
        //Find balls
        if(map->hasBall(0)) //if balls of color red are present
        {
          LeftDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
          RightDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
          robotState = collectingBalls;

          fprintf(fp, "%d\n", robotState);
          fflush(fp);
        } else {
          LeftDriveSmart.spin(directionType::rev, MIN_DRIVE_PERCENTAGE_TURN - 5, percentUnits::pct);
          RightDriveSmart.spin(directionType::fwd, MIN_DRIVE_PERCENTAGE_TURN - 5, percentUnits::pct);
        }
        break;
      case collectingBalls:
        getNearestBall(0); // red ball
        robotState = done;
        // robotState = scoreBalls;

        fprintf(fp, "%d\n", robotState);
        fflush(fp);
        break;
      case scoreBalls:
        //Score in goal
        goToNearestGoal();
        scoreAllBalls();
        robotState = done;

        fprintf(fp, "%d\n", robotState);
        fflush(fp);
        break;
    }

    this_thread::sleep_for(loop_time); // Allow other tasks to run
  }

  fclose(fp);
}

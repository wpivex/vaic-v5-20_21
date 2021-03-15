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

// Define strings for states for pprinting 
const char *PSTATE[5] = {"Startup", "Searching", "Collecting", "Scoring", "Done"};
void printStateChange(FILE* stream, State s) {
  static State prev = STATE_DONE;
  if(s != prev) {
    prev = s;
    fprintf(stream, "STATE: %s\tJetPkts: %ld\n", PSTATE[s], jetson_comms.get_packets());
    vex::controller::lcd().print("S: %s J: %ld", PSTATE[s], jetson_comms.get_packets());
    fflush(stream);
  }
}

int main() {
  vexcodeInit(); // Initializing Robot Configuration. DO NOT REMOVE!

  int32_t loop_time = 33; // Run at about 15Hz

  thread t1 = thread(dashboardTask); // start the status update display

  Competition.autonomous(autonomousMain); // Set up callbacks for autonomous and driver control periods.

  State robotState = STATE_STARTUP;
  robotState = STATE_STARTUP;

  FILE *fp = fopen("/dev/serial2", "w");

  bool firstRun = true;

  while(1) {
    updateMapObj(robotState == STATE_SEARCHING);

    drive->setPose({ 
      map->getManagerCoords().x,
      map->getManagerCoords().y,
      map->getManagerCoords().deg
    });

    // Print current state, then run transition
    printStateChange(fp, robotState);
    int c = 0;
    switch (robotState) {
      case STATE_STARTUP:
        // State machine initialized and running
        while(!vex::controller(primary).ButtonUp.pressing()) {
          updateMapObj(false);

          ++c; // lol
          if(c % 1 == 0) {
            fprintf(fp, "Awaiting start... %.2f %ld\n", map->getManagerCoords().deg, time(0));
            vex::controller::lcd().print("V5 Ready. Map Rot: %.2f", map->getManagerCoords().deg);
          } else if (c % 50 == 0) {
            fprintf(fp, "Press up on controller\n");
            vex::controller::lcd().print("Press up to start");
          }
          fflush(fp);
          this_thread::sleep_for(10);

          if(vex::controller(primary).ButtonLeft.pressing()) {
            drive->setPose({ 
              map->getManagerCoords().x,
              map->getManagerCoords().y,
              map->getManagerCoords().deg
            });
            // drive->goTo({36.0, -36.0, 0.0}, false);
            drive->turnDegreesGPS(180);
          }
        }

        drive->driveDistance(-30, false);
        drive->turnDegrees(-70);
        // Switch to search mode
        robotState = STATE_SEARCHING;
        break;
      case STATE_SEARCHING:
        //Find balls
        if(map->hasBall(1)) // if balls of color blue are present
        {
          LeftDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
          RightDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
          robotState = STATE_COLLECTING;
        } else {
          LeftDriveSmart.spin(directionType::rev, MIN_DRIVE_PERCENTAGE_TURN - 5, percentUnits::pct);
          RightDriveSmart.spin(directionType::fwd, MIN_DRIVE_PERCENTAGE_TURN - 5, percentUnits::pct);
        }
        break;
      case STATE_COLLECTING:
        getNearestBall(1); // blue ball
        robotState = STATE_SCORING;
        break;
      case STATE_SCORING:
        //Score in goal
        if (!firstRun) {
          drive->driveDistance(-20, false);
          goToNearestGoal();
          aimAndScore();
        }

        leftIntake.spin(directionType::fwd, 100, percentUnits::pct);
        rightIntake.spin(directionType::fwd, 100, percentUnits::pct);
        scoreAllBalls();
        leftIntake.stop();
        rightIntake.stop();
        
        robotState = STATE_DONE;
        break;
      case STATE_DONE:
        drive->driveDistance(-24, false);
        drive->turnDegrees(100);

        firstRun = false;
        robotState = STATE_SEARCHING;
        break;
    }

    this_thread::sleep_for(loop_time); // Allow other tasks to run
  }

  fclose(fp);
}

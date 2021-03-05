/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       commands.cpp                                              */
/*    Author:       Jatin                                                     */
/*    Created:      2/23/21                                                   */
/*    Description:  Implementation of high level robot commands               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "commands.h"
#include "math.h"
#include "robot-config.h"

/*
 * Gets the distance to the coordinate, where x and y are measured 
 * using normal xy plane conventions with the origin at the center of the field.
 * Assumes the robot is at drive's current pose
 */
float getDistanceToCoord(float x, float y) {
  float xDiff = drive->getPose().x - x;
  float yDiff = drive->getPose().y - y;

  return sqrt(xDiff * xDiff + yDiff * yDiff);
}

// Uses box objects from the camera to turn to, drive over, and pickup the nearest ball of a given color.
// colorID should be 0 for red and 1 for blue, should never be 2 (for goals)
void getNearestBall(int colorID) {
  // Find the box obj corresponding to the nearest ball, comparing by depth from the box objects
  MAP_RECORD mapRecord;
  jetson_comms.get_data(&mapRecord);
  jetson_comms.request_map();

  fifo_object_box nearestBall;
  float minDistance = -1;

  for (int i = 0; i < mapRecord.boxnum; i++) {
    fifo_object_box box = mapRecord.boxobj[i];

    if (box.classID == colorID) {
      float distance = box.depth / 25.4; // depth of object in inches

      if (minDistance == -1 || minDistance > distance) {
        nearestBall = box;
        minDistance = distance;
      }
    }
  }

  if (minDistance != -1) {
    drive->foldIntakes(false);

    // turn to ball
    drive->turnToBall(minDistance, colorID);

    this_thread::sleep_for(200);

    // drive to ball
    drive->driveDistance(minDistance - 3, true);

    // pickup ball
    drive->foldIntakes(true);
    drive->driveDistance(9, true);
    drive->foldIntakes(false);
  } else {
    FILE *fp = fopen("/dev/serial2", "w");

    fprintf(fp, "No balls of colorID %d were found.\n", colorID);

    fclose(fp);
  }
}

/*
 * Goes to the nearest goal using the given drive object.
 * 0 corresponds to a red ball and 1 corresponds to a blue ball.
 *
 * TODO for now, the goal is hardcoded to be the bottom right, the only one we have setup.
 */
void goToNearestGoal() {
  // TODO account for obstructions
  // TODO determine angle based on desired goal and starting location
  float minX, minY, angle, minDistance = -1;

  for (int xGoal = 0; xGoal < 3; xGoal++) { // lower numbers are to the left
    for (int yGoal = 0; yGoal < 3; yGoal++) { // lower numbers are towards the top
      float xCenter = (xGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - xGoal) * (GOAL_DIAMETER + 4);
      float yCenter = (yGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - yGoal) * (GOAL_DIAMETER + 4);
      float distance = getDistanceToCoord(xCenter, yCenter);

      if (minDistance == -1 || minDistance > distance) {
        minX = xCenter;
        minY = yCenter;
        minDistance = distance;
      }
    }
  }

  // TODO fix angle and position for middle goal, at 0,0
  // angle = (atan2(minY, minX) * 180 / 3.14);

  // hard code for the only goal we have setup rn, in the middle right.
  minX = (2 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 2) * (GOAL_DIAMETER + 10);
  minY = (1 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 1) * (GOAL_DIAMETER + 10);

  angle = 0;

  drive->goTo({
    minX,
    minY,
    angle
  });
}

void scoreAllBalls() {
  leftIntake.spin(directionType::fwd, 100, percentUnits::pct);
  rightIntake.spin(directionType::fwd, 100, percentUnits::pct);
  rollerBack.spin(directionType::fwd, 100, percentUnits::pct);
  yeet.spin(directionType::fwd, 100, percentUnits::pct);
  //Wait 2 seconds for now
  this_thread::sleep_for(2000);

  leftIntake.stop();
  rightIntake.stop();
  rollerBack.stop();
  yeet.stop();
}

void aimAndScore() {
  while(sonarLeft.distance(distanceUnits::in) == 0) {} // Wait for sensor to stop giving garbage

  drive->driveDistance(-2, false);
  drive->turnDegrees(25);
  
  while(sonarLeft.distance(distanceUnits::in) > 12.0) {
      LeftDriveSmart.spin(directionType::fwd, 5, percentUnits::pct);
      RightDriveSmart.spin(directionType::rev, 5, percentUnits::pct);
  }

  drive->driveDistance(2, false);
  
  LeftDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
  RightDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);

  scoreAllBalls();
}
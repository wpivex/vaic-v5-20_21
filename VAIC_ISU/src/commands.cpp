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
  // TODO make drive object global and remove parameter from above
  drive->foldIntakes(false);

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
    // turn to ball
    float ballPixel = (nearestBall.x - REALSENSE_HORZ_PIXELS / 2.0); // center of ball obj
    float angleToBall = ballPixel * REALSENSE_HORZ_FOV / REALSENSE_HORZ_PIXELS;
    drive->turnDegrees(angleToBall);

    // drive to ball
    // drive->driveDistance(minDistance, true);

    // pickup ball
    // foldIntakes(true);
    // driveDistance(12, true);
    // driveDistance(-12, false);
    // foldIntakes(false);
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
      float xCenter = (xGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - xGoal) * (GOAL_DIAMETER + 12);
      float yCenter = (yGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - yGoal) * (GOAL_DIAMETER + 12);
      float distance = getDistanceToCoord(xCenter, yCenter);

      if (minDistance == -1 || minDistance > distance) {
        minX = xCenter;
        minY = yCenter;
        minDistance = distance;
      }
    }
  }

  // hard code for the only goal we have setup rn, in the middle right.
  minX = (2 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 2) * (GOAL_DIAMETER + 12);
  minY = (1 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 1) * (GOAL_DIAMETER + 12);

  // FILE *fp = fopen("/dev/serial2", "w");

  // fprintf(fp, "%.2f %.2f\n", minX, minY);

  // fclose(fp);

  angle = 0;

  drive->goTo({
    minX,
    minY,
    angle
  });
}

void scoreAllBalls()
{
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
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
float getDistanceToCoord(Drive* drive, float x, float y) {
  float xDiff = drive->getPose().x - x;
  float yDiff = drive->getPose().y - y;

  return sqrt(xDiff * xDiff + yDiff * yDiff);
}

/*
 * Goes to the nearest ball of the desired color using the given drive object.
 * 0 corresponds to a red ball and 1 corresponds to a blue ball.
 */
void goToNearestBall(int desiredColor, Drive* drive) {
  // TODO make drive object global and remove parameter from above
  BallCoord* nearestBall;
  float minDistance = -1;

  for (int i = 0; i < map->getNumBalls(); i++) {
    BallCoord* ball = &map->getBallCoords()[i];
    float distance = getDistanceToCoord(drive, ball->x, ball->y);

    if (ball->colorID == desiredColor) {
      if (minDistance == -1 || minDistance > distance) {
        nearestBall = ball;
        minDistance = distance;
      }
    }
  }

  if (minDistance != -1) {
      drive->getBall({
        nearestBall->x,
        nearestBall->y,
        map->getManagerCoords().deg
    });
  }
}

/*
 * Goes to the nearest goal using the given drive object.
 * 0 corresponds to a red ball and 1 corresponds to a blue ball.
 *
 * TODO for now, the goal is hardcoded to be the bottom right, the only one we have setup.
 */
void goToNearestGoal(Drive* drive) {
  // TODO account for obstructions
  // TODO determine angle based on desired goal and starting location
  float minX, minY, angle, minDistance = -1;

  for (int xGoal = 0; xGoal < 3; xGoal++) { // lower numbers are to the left
    for (int yGoal = 0; yGoal < 3; yGoal++) { // lower numbers are towards the top
      float xCenter = (xGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - xGoal) * (GOAL_DIAMETER + 12);
      float yCenter = (yGoal - 1) * (FIELD_LENGTH_IN / 2) + (1 - yGoal) * (GOAL_DIAMETER + 12);
      float distance = getDistanceToCoord(drive, xCenter, yCenter);

      if (minDistance == -1 || minDistance > distance) {
        minX = xCenter;
        minY = yCenter;
        minDistance = distance;
      }
    }
  }

  // hard code for the only goal we have setup rn, in the middle right.
  minX = (2 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 2) * (GOAL_DIAMETER + 7);
  minY = (1 - 1) * (FIELD_LENGTH_IN / 2) + (1 - 1) * (GOAL_DIAMETER + 7);

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
void aimAndScore()
{
  while(sonarLeft.distance(distanceUnits::in)==0)
  {}//Wait for sensor to stop giving garbage
  
  while(sonarLeft.distance(distanceUnits::in) > 10.0){
      LeftDriveSmart.spin(directionType::fwd, 5, percentUnits::pct);
      RightDriveSmart.spin(directionType::rev, 5, percentUnits::pct);
  }
  LeftDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
  RightDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);

  scoreAllBalls();
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
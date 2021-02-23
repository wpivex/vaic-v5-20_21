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
      drive->goTo({
        nearestBall->x,
        nearestBall->y,
        map->getManagerCoords().deg
    });
  }
}

void goToNearestGoal(Drive* drive) {
  // TODO account for obstructions
  float minX, minY, minDistance = -1;

  for (int xGoal = 0; xGoal < 3; xGoal++) { // lower numbers are to the left
    for (int yGoal = 0; yGoal < 3; yGoal++) { // lower numbers are towards the top
      float xCenter = (xGoal - 1) * (FIELD_LENGTH_IN) + (1 - xGoal) * GOAL_RADIUS;
      float yCenter = (yGoal - 1) * (FIELD_LENGTH_IN) + (1 - yGoal) * GOAL_RADIUS;
      float distance = getDistanceToCoord(drive, xCenter, yCenter);

      if (minDistance == -1 || minDistance > distance) {
        minX = xCenter;
        minY = yCenter;
        minDistance = distance;
      }
    }
  }

  drive->goTo({
    minX,
    minY,
    map->getManagerCoords().deg // TODO fix angle
  });
}
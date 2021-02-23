#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "Drive.h"

float getDistanceToBall(Drive* drive, BallCoord* ball);
void goToNearestBall(int colorID, Drive* drive);

#endif
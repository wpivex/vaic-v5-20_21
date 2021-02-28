/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       commands.h                                                */
/*    Author:       Jatin                                                     */
/*    Created:      2/23/21                                                   */
/*    Description:  Header for high level robot commands                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "Drive.h"

const float FIELD_LENGTH_IN = 11.7 * 12;
const float GOAL_RADIUS = 11.29 / 2;

float getDistanceToCoord(Drive* drive, BallCoord* ball);
void goToNearestBall(int colorID, Drive* drive);
void goToNearestGoal(Drive* drive);
void scoreAllBalls();

#endif
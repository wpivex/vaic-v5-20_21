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
#include "vex.h"

float getDistanceToCoord(float x, float y);
void getNearestBall(int colorID);
void goToNearestGoal();
void scoreAllBalls();
void aimAndScore();

#endif
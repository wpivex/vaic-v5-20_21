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

const float FIELD_LENGTH_IN = 11.7 * 12;
const float GOAL_DIAMETER = 12; // actual: 11.29

// https://www.intel.com/content/www/us/en/support/articles/000030385/emerging-technologies/intel-realsense-technology.html
const float REALSENSE_HORZ_FOV = 69.4;
const float REALSENSE_HORZ_PIXELS = 320;

float getDistanceToCoord(float x, float y);
void getNearestBall(int colorID);
void goToNearestGoal();
void scoreAllBalls();

#endif
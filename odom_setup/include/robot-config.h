#pragma once

#include "cmath"

using namespace vex;

extern brain Brain;
//Motors and motorgroups to be used in other classes
extern motor_group LeftDriveSmart;
extern motor_group RightDriveSmart;

extern motor leftIntake;
extern motor rightIntake;

extern motor yeet;
extern motor rollerBack;

extern encoder encl;
extern encoder encr;

const static int TICKS_PER_ROTATION = 1020;
const static double ENCODER_WHEEL_DIAMETER = 2.75;
const static double DEG_TO_IN_FACTOR = M_PI * ENCODER_WHEEL_DIAMETER/TICKS_PER_ROTATION;
//Get inch values for left and right encoders
double leftInches();
double rightInches();
double velocityLeft();
double velocityRight();
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);
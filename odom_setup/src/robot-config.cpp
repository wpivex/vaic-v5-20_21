#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

// VEXcode device constructors
motor leftMotorA = motor(PORT1, ratio18_1, true);
motor leftMotorB = motor(PORT2, ratio18_1, false);
motor leftMotorC = motor(PORT3, ratio18_1, true);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB, leftMotorC);

motor rightMotorA = motor(PORT9, ratio18_1, false);
motor rightMotorB = motor(PORT10, ratio18_1, true);
motor rightMotorC = motor(PORT11, ratio18_1, false);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB, rightMotorC);

motor leftIntake = motor(PORT11, ratio6_1, true);
motor rightIntake = motor(PORT20, ratio6_1, true);

motor yeet = motor(PORT5,ratio6_1,true);
motor rollerBack = motor(PORT19, ratio6_1, true);

encoder encl = encoder(Brain.ThreeWirePort.C);
encoder encr = encoder(Brain.ThreeWirePort.G);

double leftInches()
{
  return encl.position(rotationUnits::deg)*DEG_TO_IN_FACTOR;
}
double rightInches()
{
  return encr.position(rotationUnits::deg)*DEG_TO_IN_FACTOR;
}
double velocityLeft()
{
  return encl.velocity(velocityUnits::dps)*DEG_TO_IN_FACTOR;
}
double velocityRight()
{
  return encr.velocity(velocityUnits::dps)*DEG_TO_IN_FACTOR;
}
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void) {
  // Nothing to initialize
}
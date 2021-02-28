#include "vex.h"
#include "Drive.h"
#include "cmath"

using namespace vex;

Drive::Drive() {
   myPose.x = 0.0;
   myPose.y = 0.0;
   myPose.theta = 0.0;
}

void Drive::setPose(Pose newPose){
  myPose.x = newPose.x;
  myPose.y = newPose.y;
  myPose.theta = newPose.theta;
}

void Drive::goTo(Pose newPose){
  if( std::abs(newPose.x - myPose.x) > .5 || std::abs(newPose.y - myPose.y) > .5 )
  {
    double dx = newPose.x - myPose.x;
    double dy = newPose.y - myPose.y;

    double turn1 = (atan2(dy, dx) * 180 / 3.14) - myPose.theta; //Calculate angle to new position, subtract current angle to know how much to turn

    while(turn1 > 180){
      turn1 -= 360;
    }
    while(turn1 < -180){
      turn1 += 360;
    }
    turnDegrees(turn1); //Heading is updated within this function

    double dist = sqrt(dx * dx + dy * dy);
    driveDistance(dist, false); //Position is updated within this function
  }

  double turn2 = newPose.theta - myPose.theta;

  while(turn2 > 180){
    turn2 -= 360;
  }
  while(turn2 < -180){
    turn2 += 360;
  }

  turnDegrees(turn2); //Heading is updated within this function
}

void Drive::getBall(Pose newPose){
  if( std::abs(newPose.x - myPose.x) > .5 || std::abs(newPose.y - myPose.y) > .5 )
  {
    double dx = newPose.x - myPose.x;
    double dy = newPose.y - myPose.y;

    double turn1 = (atan2(dy, dx) * 180 / 3.14) - myPose.theta; //Calculate angle to new position, subtract current angle to know how much to turn

    while(turn1 > 180){
      turn1 -= 360;
    }
    while(turn1 < -180){
      turn1 += 360;
    }
    turnDegrees(turn1); //Heading is updated within this function

    //Stop 6 inches late
    double dist = sqrt(dx * dx + dy * dy) + 6;
    driveDistance(dist,false); //Position is updated within this function

    //Go grab it!
    foldIntakes(true);
    driveDistance(12, true); //Position is updated within this function
    
    driveDistance(-12, false);
  }

  // turnDegrees(newPose.theta-myPose.theta); //Heading is updated within this function
  foldIntakes(false);
}

void Drive::turnDegrees(double angle){
  int timeout = ((std::abs(angle) / 10) + 2) * 1000;
  int maxTime = vex::timer::system()+timeout;//This is the maximum duration to try to turn before giving up

  double IN_PER_90 = 11; //What value the encoder reads for a 90 degree turn

  double targetL = leftInches() - IN_PER_90 * angle / 90.0;
  double targetR = rightInches() + IN_PER_90 * angle / 90.0;

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do
  {
    updateMapObj();

    leftError = -targetL + leftInches();
    rightError = targetR - rightInches();
    error = (leftError + rightError) / 2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpturn * error)), MIN_DRIVE_PERCENTAGE);
    
    if(error < 0)
      driveValue = -driveValue;

    LeftDriveSmart.spin(directionType::rev, driveValue, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);

  } while((std::abs(error) > .025 || velocityLeft() > 2 || velocityRight() > 2) && vex::timer::system()<maxTime);

  LeftDriveSmart.stop();
  RightDriveSmart.stop();

  myPose.theta = myPose.theta + angle;
}

void Drive::driveDistance(double inches, bool intaking){
  int timeout = (std::abs(inches)/10+2)*1000;
  int maxTime = vex::timer::system() + timeout;//This is the maximum duration to try to turn before giving up

  double targetL = leftInches() + inches;
  double targetR = rightInches() + inches;

  //Temporary code for smooth-ish acceleration
  for(int i = MIN_DRIVE_PERCENTAGE; i < MAX_DRIVE_PERCENTAGE; i++)
  {
    LeftDriveSmart.spin(directionType::fwd, i, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, i, percentUnits::pct);
    this_thread::sleep_for(10);
  }

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do
  {
    updateMapObj();

    leftError = targetL - leftInches();
    rightError = targetR - rightInches();
    error = (leftError + rightError) / 2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpstraight * error)), MIN_DRIVE_PERCENTAGE);
    if(error < 0)
      driveValue = -driveValue;

    LeftDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);

    if(intaking)
    {
      leftIntake.spin(directionType::fwd, 100, percentUnits::pct);
      rightIntake.spin(directionType::fwd, 100, percentUnits::pct);
    }

  } while((std::abs(error) > .5 || velocityLeft() > 2 || velocityRight() > 2) && vex::timer::system()<maxTime);

  leftIntake.stop();
  rightIntake.stop();
  
  LeftDriveSmart.stop();
  RightDriveSmart.stop();

  //Update pose
  double xDir = inches * cos(myPose.theta * M_PI/180);
  double yDir = inches * sin(myPose.theta * M_PI/180);

  myPose.x = myPose.x + xDir;
  myPose.y = myPose.y + yDir;
}

Pose Drive::getPose(){
  return myPose;
}

void Drive::foldIntakes(bool foldout) {
  if(foldout)
  {
    leftIntake.spin(directionType::fwd, 100, percentUnits::pct);
    rightIntake.spin(directionType::rev, 100, percentUnits::pct);
  }else{
    leftIntake.spin(directionType::rev, 100, percentUnits::pct);
    rightIntake.spin(directionType::fwd, 100, percentUnits::pct);
  }

  this_thread::sleep_for(500);

  leftIntake.stop();
  rightIntake.stop();
}
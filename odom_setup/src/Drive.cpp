#include "vex.h"
#include "Drive.h"
#include "cmath"

using namespace vex;

const double MAX_DRIVE_PERCENTAGE = 25;
const double MIN_DRIVE_PERCENTAGE = 2;
const double Kpstraight = 4;//We probably want a seperate Kp value for turning but its good enough for now. What we really need is better encoder wheel mounting points
const double Kpturn = 2;

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
  double dx = newPose.x-myPose.x;
  double dy = newPose.y-myPose.y;

  double turn1 = (atan2(dy, dx)*180/3.14) - myPose.theta;//Calculate angle to new position, subtract surrent angle to know how much to turn
  turnDegrees(turn1);//Heading is updated within this function

  double dist = sqrt(dx*dx+dy*dy);
  driveDistance(dist);//Position is updated within this function

  turnDegrees(newPose.theta-myPose.theta);//Heading is updated within this function
}


void Drive::turnDegrees(double angle){
  double IN_PER_90 = 10.3;//What value the encoder reads for a 90 degree turn

  double targetL = leftInches()-IN_PER_90*angle/90.0;
  double targetR = rightInches()+IN_PER_90*angle/90.0;

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do
  {
    leftError = -targetL+leftInches();
    rightError = targetR-rightInches();
    error = (leftError+rightError)/2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpturn*error)), MIN_DRIVE_PERCENTAGE);
    
    if(error<0)
      driveValue = - driveValue;

    LeftDriveSmart.spin(directionType::rev, driveValue, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);

  }while(std::abs(error)>.05 || velocityLeft()>2 || velocityRight()>2);

  LeftDriveSmart.stop();
  RightDriveSmart.stop();

  myPose.theta = myPose.theta+angle;
}



void Drive::driveDistance(double inches){
  double targetL = leftInches()+inches;
  double targetR = rightInches()+inches;

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do
  {
    leftError = targetL-leftInches();
    rightError = targetR-rightInches();
    error = (leftError+rightError)/2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpstraight*error)), MIN_DRIVE_PERCENTAGE);
    if(error<0)
      driveValue = - driveValue;

    LeftDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);


    leftIntake.spin(directionType::fwd,100,percentUnits::pct);
    rightIntake.spin(directionType::fwd,100,percentUnits::pct);

  }while(std::abs(error)>.5 || velocityLeft()>2 || velocityRight()>2);

  LeftDriveSmart.stop();
  RightDriveSmart.stop();

  //Update pose
  double xDir = inches*cos(myPose.theta);
  double yDir = inches*sin(myPose.theta);

  myPose.x = myPose.x + xDir;
  myPose.y = myPose.y + yDir;
}
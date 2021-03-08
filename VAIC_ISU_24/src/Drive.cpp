#include "vex.h"
#include "Drive.h"
#include "cmath"

using namespace vex;

Drive::Drive() {
   myPose.x = 0.0;
   myPose.y = 0.0;
   myPose.theta = 0.0;
}

void Drive::setPose(Pose newPose) {
  myPose.x = newPose.x;
  myPose.y = newPose.y;
  myPose.theta = newPose.theta;
}

void Drive::goTo(Pose newPose, bool toFinalAngle) {
  if( std::abs(newPose.x - myPose.x) > .5 || std::abs(newPose.y - myPose.y) > .5 )
  {
    double dx = newPose.x - myPose.x;
    double dy = newPose.y - myPose.y;

    double turn1 = (atan2(dy, dx) * 180 / 3.14) - myPose.theta; //Calculate angle to new position, subtract current angle to know how much to turn

    while(turn1 > 180) {
      turn1 -= 360;
    }
    while(turn1 < -180) {
      turn1 += 360;
    }
    turnDegrees(turn1); //Heading is updated within this function

    double dist = sqrt(dx * dx + dy * dy);
    driveDistance(dist, false); //Position is updated within this function
  }

  //Complete final turn if relevant
  if(toFinalAngle){
    double turn2 = newPose.theta - myPose.theta;

    while(turn2 > 180) {
      turn2 -= 360;
    }
    while(turn2 < -180) {
      turn2 += 360;
    }

    turnDegrees(turn2); //Heading is updated within this function
  }
}

// Turns to the desired ball given by the depth from the camera (in inches) and color
// These two parameters should be able to have the camera track the same ball while 
// turning, even if other balls come into view
void Drive::turnToBall(float desiredDepth, int colorID) {
  MAP_RECORD mapRecord;
  int error, xPixel;
  int sumError = 0;
  float minDiffDepth, output;

  int numCyclesAtTarget = 0;

  do {
    jetson_comms.get_data(&mapRecord);
    jetson_comms.request_map();

    minDiffDepth = 100;

    for (int i = 0; i < mapRecord.boxnum; i++) {
      float diffDepth = std::abs(mapRecord.boxobj[i].depth / 25.4 - desiredDepth);

      // check if colors match and the ball's depth is the closest match to what the depth was
      // before this method was called, from desiredDepth
      if (mapRecord.boxobj[i].classID == colorID && diffDepth < minDiffDepth) {
        xPixel = mapRecord.boxobj[i].x;
        minDiffDepth = diffDepth;
      }
    }

    error = BALL_TURN_SETPOINT - xPixel;

    if (abs(error) < BALL_TURN_KI_THRESHOLD)
      sumError += error;

    output = error * BALL_TURN_KP + sumError * BALL_TURN_KI;

    LeftDriveSmart.spin(directionType::rev, output, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, output, percentUnits::pct);

    if (abs(error) < BALL_TURN_MAX_ERROR)
      numCyclesAtTarget++;
    else
      numCyclesAtTarget = 0;

    this_thread::sleep_for(10);
    // FILE *fp = fopen("/dev/serial2", "w");

    // fprintf(fp, "error:%d cycles:%d\n", abs(error), numCyclesAtTarget);

    // fclose(fp);

  } while (abs(error) >= BALL_TURN_MAX_ERROR || numCyclesAtTarget <= 20);

  LeftDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
  RightDriveSmart.spin(directionType::fwd, 0, percentUnits::pct);
}

void Drive::turnDegrees(double angle) { 
  int timeout = ((std::abs(angle) / 10) + 2) * 1000; // 2 seconds + angle * 1s / 10 deg
  int maxTime = vex::timer::system() + timeout; // This is the maximum duration to try to turn before giving up

  double IN_PER_90 = 11; //What value the encoder reads for a 90 degree turn

  double targetL = leftInches() - IN_PER_90 * angle / 90.0;
  double targetR = rightInches() + IN_PER_90 * angle / 90.0;

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do {
    updateMapObj();

    leftError = -targetL + leftInches();
    rightError = targetR - rightInches();
    error = (leftError + rightError) / 2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpturn * error)), MIN_DRIVE_PERCENTAGE);
    
    if(error < 0)
      driveValue = -driveValue;

    LeftDriveSmart.spin(directionType::rev, driveValue, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);

  } while((std::abs(error) > .025 || velocityLeft() > 2 || velocityRight() > 2) && vex::timer::system() < maxTime);

  LeftDriveSmart.stop();
  RightDriveSmart.stop();

  myPose.theta = myPose.theta + angle;
}

void Drive::driveDistance(double inches, bool intaking) {
  int timeout = (std::abs(inches)/10+2)*1000; // 2 seconds + distance * 1s / 10 in
  int maxTime = vex::timer::system() + timeout; // This is the maximum duration to try to turn before giving up

  double targetL = leftInches() + inches;
  double targetR = rightInches() + inches;

  //Temporary code for smooth-ish acceleration
  for(int i = MIN_DRIVE_PERCENTAGE; i < MAX_DRIVE_PERCENTAGE; i++) {
    LeftDriveSmart.spin(directionType::fwd, i, percentUnits::pct);
    RightDriveSmart.spin(directionType::fwd, i, percentUnits::pct);
    this_thread::sleep_for(10);
  }

  //Multiply the proportional term by this
  double leftError;
  double rightError;
  double error;
  do {
    updateMapObj();

    leftError = targetL - leftInches();
    rightError = targetR - rightInches();
    error = (leftError + rightError) / 2;

    double driveValue = std::max(std::min(MAX_DRIVE_PERCENTAGE, std::abs(Kpstraight * error)), MIN_DRIVE_PERCENTAGE);
    if(error < 0)
      driveValue = -driveValue;

    double dist = sonarLeft.distance(distanceUnits::in);
    bool obstacleDetected = dist < (error - 6) && dist > 4;
    vex::controller::lcd().print("Dist --%f--\r", dist);
    vexDelay(20);

    if(!obstacleDetected)
    {
      LeftDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);
      RightDriveSmart.spin(directionType::fwd, driveValue, percentUnits::pct);
    }else{
      maxTime+=10;
      LeftDriveSmart.stop();
      RightDriveSmart.stop();
    }

    if(intaking) {
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

Pose Drive::getPose() {
  return myPose;
}

void Drive::foldIntakes(bool foldout) {
  if(foldout) {
    leftIntake.spin(directionType::fwd, 100, percentUnits::pct);
    rightIntake.spin(directionType::rev, 100, percentUnits::pct);
  } else {
    leftIntake.spin(directionType::rev, 100, percentUnits::pct);
    rightIntake.spin(directionType::fwd, 100, percentUnits::pct);
  }

  this_thread::sleep_for(500);

  leftIntake.stop();
  rightIntake.stop();
}
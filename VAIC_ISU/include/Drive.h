#pragma once

#include "vex.h"

using namespace vex;

const double MAX_DRIVE_PERCENTAGE = 25;
const double MIN_DRIVE_PERCENTAGE = 2;
const double MIN_DRIVE_PERCENTAGE_TURN = 10;
const double Kpstraight = 3.5; //We probably want a seperate Kp value for turning but its good enough for now. What we really need is better encoder wheel mounting points
const double Kpturn = 2.5;

struct Pose{double x; double y; double theta;};

class Drive {
	public:
		Drive();
		
    void setPose(Pose newPose);
    void goTo(Pose newPose);

    void driveDistance(double inches, bool intaking);
    void turnDegrees(double angle);

    void foldIntakes(bool foldout);

    Pose getPose();
  private:
    Pose myPose;
};
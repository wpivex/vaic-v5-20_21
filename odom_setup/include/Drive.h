#pragma once

#include "vex.h"

using namespace vex;

struct Pose{double x; double y; double theta;};

class Drive {
	public:
		Drive();
		
    void setPose(Pose newPose);
    void goTo(Pose newPose);

    void driveDistance(double inches);
    void turnDegrees(double angle);

    Pose getPose();
  private:
    Pose myPose;
};
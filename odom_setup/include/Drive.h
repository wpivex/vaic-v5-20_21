#pragma once

#include "vex.h"

using namespace vex;

struct Pose{double x; double y; double theta;};

class Drive {
	public:
		Drive();
		
    void setPose(Pose newPose);
    void goTo(Pose newPose);
    void getBall(Pose newPose);

    void driveDistance(double inches, bool intaking);
    void turnDegrees(double angle);

    void foldIntakes(bool foldout);

    Pose getPose();
  private:
    Pose myPose;
    bool intakesDeployed;
};
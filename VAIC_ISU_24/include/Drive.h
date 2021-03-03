#pragma once

#ifndef DRIVE_H_
#define DRIVE_H_

const double MAX_DRIVE_PERCENTAGE = 25;
const double MIN_DRIVE_PERCENTAGE = 2;
const double MIN_DRIVE_PERCENTAGE_TURN = 10;
const double Kpstraight = 3.5; //We probably want a seperate Kp value for turning but its good enough for now. What we really need is better encoder wheel mounting points
const double Kpturn = 2.5;

const int BALL_TURN_SETPOINT = 160; // # x pixels of the realsense camera / 2
const int BALL_TURN_MAX_ERROR = 7;
const int BALL_TURN_MAX_OUTPUT = 5;
const float BALL_TURN_KP = 0.03;
const float BALL_TURN_KI = 0.001;
const int BALL_TURN_KI_THRESHOLD = 30;

struct Pose{double x; double y; double theta;};

class Drive {
	public:
		Drive();
		
    void setPose(Pose newPose);
    void goTo(Pose newPose);

    void turnToBall(float desiredDepth, int colorID);

    void turnDegrees(double angle);
    void driveDistance(double inches, bool intaking);

    void foldIntakes(bool foldout);

    Pose getPose();
  private:
    Pose myPose;
};

#endif
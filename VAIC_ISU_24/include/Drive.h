#pragma once

#ifndef DRIVE_H_
#define DRIVE_H_

const double MAX_DRIVE_PERCENTAGE = 25;
const double MIN_DRIVE_PERCENTAGE = 2;
const double MIN_DRIVE_PERCENTAGE_TURN = 15;
const double Kpstraight = 3.5; //We probably want a seperate Kp value for turning but its good enough for now. What we really need is better encoder wheel mounting points
const double Kpturn = 2.5;
const double MAX_DRIVE_PERCENTAGE_TURN_GPS = 50;
const double MIN_DRIVE_PERCENTAGE_TURN_GPS = 4;
const double Kpturngps = 0.08;

const int BALL_TURN_SETPOINT = 160; // # x pixels of the realsense camera / 2
const int BALL_TURN_MAX_ERROR = 10;
const int BALL_TURN_MAX_OUTPUT = 1;
const float BALL_TURN_KP = 0.035;
const float BALL_TURN_KI = 0.0003;
const int BALL_TURN_KI_THRESHOLD = 20;

struct Pose{double x; double y; double theta;};

class Drive {
	public:
		Drive();
		
    void setPose(Pose newPose);
    void goTo(Pose newPose, bool toFinalAngle);

    void turnToBall(float desiredDepth, int colorID);

    void turnDegrees(double angle);
    void turnDegreesGPS(double angle);

    void driveDistance(double inches, bool intaking);

    void foldIntakes(bool foldout);

    Pose getPose();
  private:
    Pose myPose;
};

#endif
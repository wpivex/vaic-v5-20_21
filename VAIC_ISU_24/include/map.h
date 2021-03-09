/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       map.h                                                     */
/*    Author:       Jatin                                                     */
/*    Created:      2/17/21                                                   */
/*    Description:  Header for the robot's map of the field                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef MAP_H_
#define MAP_H_

// distance from FLIR camera to center of robot, with units of in
// The position reported from the jetson is the xy location of the FLIR camera
const float POS_OFFSET = 12; 

typedef struct {
  int32_t age; // number of frames since this ball was last mapped
  int colorID; // 0 = red, 1 = blue, -1 = invalid
  float x; // in above center of field (0,0)
  float y; // in right of center of field (0,0)
  float z; // in above floor
  int goal; // id of goal the ball is scored in. https://imgur.com/a/QyFIE9a. -1 is unscored
} BallCoord;

typedef struct {
  int robotID; // 0 = manager, 1 = worker, -1 = invalid
  float x; // in above center of field (0,0)
  float y; // in right of center of field (0,0)
  float deg; // heading in degrees, counterclockwise from +x
  int size; // width in in, 15 or 24 (or -1 if unknown for enemy bots)
} RobotCoord;

void updateMapObj(void);

class Map {
  public:
    const static int MAX_BALLS = 36;

    BallCoord* getUnscoredBallCoords(void);
    BallCoord* getScoredBallCoords(void);
    int getNumUnscoredBalls(void);
    int getNumScoredBalls(void);
    bool hasBall(int, bool); // int colorID and bool currently scored in a goal
    RobotCoord getManagerCoords(void);
    RobotCoord getWorkerCoords(void);

    // Methods to be used by manager/worker bots to add to the shared map.
    // Interpretation of new data and removal of old elements will be handled in this class,
    // and the bots will only report what they see.
    // void addBallCoord(BallCoord);
    // void addRobotCoord(RobotCoord);

    // Methods to be used by manager/worker bots to completely overwrite all map data.
    // Useful for testing mapping from one robot while the add methods are not finished.
    void setBallCoords(BallCoord*, BallCoord*, int, int);
    void setRobotCoords(RobotCoord*, int);

  private:
    int numUnscoredBalls = 0, numScoredBalls = 0; // number of each currently on the map
    BallCoord unscoredBalls[MAX_BALLS]; // not all elements will likely be used simultaneously
    BallCoord scoredBalls[MAX_BALLS]; // not all elements will likely be used simultaneously
    RobotCoord manager;
    RobotCoord worker;
};

#endif
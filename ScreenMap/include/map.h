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

class Map {
  public:
    typedef struct {
      int colorID; // 0 = red, 1 = blue
      float x; // in from top left
      float y; // in from top right
    } BallCoord;

    typedef struct {
      float robotID; // 0 = host, 1 = client, 2 = enemy
      float x; // in from top left
      float y; // in from top left
      float deg; // heading in degrees, counterclockwise from +x
      float size; // width in in, 15 or 24 (or -1 if unknown for enemy bots)
    } RobotCoord;

    Map();

    BallCoord* getBallCoords(void);
    int getNumBalls(void);
    RobotCoord getHostCoords(void);
    RobotCoord getClientCoords(void);
    RobotCoord* getEnemyCoords(void);
    int getNumEnemies(void);

    // Methods to be used by host/client bots to add to the shared map.
    // Interpretation of new data and removal of old elements will be handled in this class,
    // and the bots will only report what they see.
    void addBallCoord(BallCoord);
    void addRobotCoord(RobotCoord);

  private:
    int numBalls, numEnemies; // number of each currently on the map
    BallCoord balls[36]; // not all elements will likely be used simultaneously
    RobotCoord host;
    RobotCoord client;
    RobotCoord enemies[2];
};

#endif
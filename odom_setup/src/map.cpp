/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       map.cpp                                                   */
/*    Author:       Jatin                                                     */
/*    Created:      2/17/21                                                   */
/*    Description:  Implementation of the map class's methods                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "map.h"

using namespace vex;
using namespace ai;

BallCoord* Map::getBallCoords(void) {
  return balls;
}

int Map::getNumBalls(void) {
  return numBalls;
}

RobotCoord Map::getManagerCoords(void) {
  return manager;
}

RobotCoord Map::getWorkerCoords(void) {
  return worker;
}

RobotCoord* Map::getEnemyCoords(void) {
  return enemies;
}

int Map::getNumEnemies(void) {
  return numEnemies;
}

// void Map::addBallCoord(BallCoord coord) {
//   // TODO implement intelligent management of existing elements given new data
// }

// void Map::addRobotCoord(RobotCoord coord) {
//   // TODO implement intelligent management of existing elements given new data
// }

void Map::setBallCoords(BallCoord* coords, int numCoords) {
  for (int i = 0; i < MAX_BALLS; i++) {
    if (i < numCoords)
      balls[i] = coords[i];
    else 
      balls[i] = {0, -1, 0, 0};
  }

  numBalls = numCoords;
}

void Map::setRobotCoords(RobotCoord* coords, int numCoords) {
  manager = {-1, 0, 0, 0, 0};
  worker = {-1, 0, 0, 0, 0};
  
  for (int i = 0; i < MAX_ENEMIES; i++)
    enemies[i] = {-1, 0, 0, 0, 0};
  numEnemies = 0;

  for (int i = 0; i < numCoords; i++) {
    switch(coords[i].robotID) {
      case 0: 
        manager = coords[i];
        break;

      case 1:
        worker = coords[i];
        break;

      case 2:
        if (numEnemies < MAX_ENEMIES)
          enemies[numEnemies++] = coords[i];
        break;
    }
  }
}
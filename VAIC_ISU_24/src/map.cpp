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

/*
 * Updates the global Map obj declared in vex.h with data from the Jetson and vex link
 */
void updateMapObj(bool printBalls) {
  MAP_RECORD mapRecord; // Map from the Jetson

  jetson_comms.get_data(&mapRecord);
  
  jetson_comms.request_map();

  // get ball data from mapRecord
  int numBalls = mapRecord.mapnum;
  BallCoord balls[numBalls];

  FILE *fp = fopen("/dev/serial2", "w");
  for (int i = 0; i < numBalls; i++) {
    float x = (mapRecord.mapobj[i].positionX / -25.4);
    float y = (mapRecord.mapobj[i].positionY / -25.4);
    balls[i] = {mapRecord.mapobj[i].age, mapRecord.mapobj[i].classID, x, y};
    if(printBalls) {
      switch(mapRecord.mapobj[i].classID) {
        case 0:
          fprintf(fp, "*   Red ");
          break;
        case 1:
          fprintf(fp, "*  Blue ");
          break;
        default:
          fprintf(fp, "* Not a ");
          break;
      }
      fprintf(fp, "Ball: %f, %f\n\r", balls[i].x, balls[i].y );
    }
  }
  fflush(fp);
  fclose(fp);

  map->setBallCoords(balls, numBalls);

  // get manager robot data from mapRecord and worker data from vex link coords
  RobotCoord robots[2];
  int numRobots = 1;

  float managerHeading = (float) ((-mapRecord.pos.az - M_PI/2) * 360 / (2 * M_PI));
  float managerX = mapRecord.pos.x / -25.4f + POS_OFFSET * cos(managerHeading * M_PI / 180);
  float managerY = mapRecord.pos.y / -25.4f + POS_OFFSET * sin(managerHeading * M_PI / 180);

  robots[0] = {
    0, // manager
    managerX, // hopefully in to the right of (0,0), need to test on field
    managerY, // hopefully in above of (0,0), need to test on field
    managerHeading, // starts at +x and increases counterclockwise, range of (-270 : 90)
    24 // 24 in
  };

  link.set_remote_location(robots[0].x, robots[0].y, robots[0].deg);

  if (link.isLinked()) {
    float workerX, workerY, workerHeading;
    link.get_remote_location(workerX, workerY, workerHeading);

    robots[1] = {
      1, // worker
      workerX, // hopefully in to the right of (0,0), need to test on field
      workerY, // hopefully in above of (0,0), need to test on field
      workerHeading, // hopefully starts at +x and increases counterclockwise, need to test on field
      15 // 15 in
    };

    numRobots++;
  }

  map->setRobotCoords(robots, numRobots);
} // updateMapObj()

BallCoord* Map::getBallCoords(void) {
  return balls;
}

int Map::getNumBalls(void) {
  return numBalls;
}

bool Map::hasBall(int id){
  for(int i = 0; i<MAX_BALLS;i++)
  {
    if(id==balls[i].colorID)
      return true;
  }
  return false;
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
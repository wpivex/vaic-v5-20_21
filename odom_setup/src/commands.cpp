#include "commands.h"

/*
 * Goes to the nearest ball of the desired color using the given drive object.
 * 0 corresponds to a red ball and 1 corresponds to a blue ball.
 */
void goToNearestBall(int desiredColor, Drive* drive) {
  //TODO make drive object global and remove parameter from above
  int id = -1;

  for (int i = 0; i < map->getNumBalls(); i++) {
    if (map->getBallCoords()[i].colorID == desiredColor) {
      id = i;
      break;
    }
  }

  if (id != -1) {
      drive->goTo({
        map->getBallCoords()[id].x,
        map->getBallCoords()[id].y,
        map->getManagerCoords().deg
    });
  }
}
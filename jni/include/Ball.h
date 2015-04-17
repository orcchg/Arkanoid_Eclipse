#ifndef __ARKANOID_BALL__H__
#define __ARKANOID_BALL__H__

#include "BallPosition.h"
#include "Params.h"

namespace game {

struct Ball {
  BallPosition pose;  //!< Location of ball's center.
  GLfloat x_velocity, y_velocity;
  GLfloat spin;
  GLfloat mass;

  Ball()
    : pose()
    , x_velocity(0.0f), y_velocity(0.0f)
    , spin(0.0f)
    , mass(BallParams::ballMass) {
  }
};

}

#endif  // __ARKANOID_BALL__H__

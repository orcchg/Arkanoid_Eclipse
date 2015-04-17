#ifndef __ARKANOID_BALL__H__
#define __ARKANOID_BALL__H__

#include "BallPosition.h"
#include "Params.h"

namespace game {

struct Ball {
  BallPosition pose;  //!< Location of ball's center.
  GLfloat angle;  //!< Angle (radian) between velocity and positive X axis.
  GLfloat x_velocity, y_velocity;
  GLfloat spin;
  GLfloat mass;

  Ball()
    : pose()
    , x_velocity(BallParams::ballSpeed)
    , y_velocity(BallParams::ballSpeed)
    , spin(0.0f)
    , mass(BallParams::ballMass) {
  }
};

}

#endif  // __ARKANOID_BALL__H__

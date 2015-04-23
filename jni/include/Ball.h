#ifndef __ARKANOID_BALL__H__
#define __ARKANOID_BALL__H__

#include "BallDimens.h"
#include "BallPosition.h"
#include "Params.h"

namespace game {

struct Ball {
  BallDimens dimens;
  BallPosition pose;  //!< Location of ball's center.
  GLfloat angle;  //!< Angle (radian) between velocity and positive X axis.
  GLfloat velocity;

  Ball(GLfloat aspect = 1.0f)
    : dimens(aspect)
    , pose()
    , angle(0.0f)
    , velocity(BallParams::ballSpeed) {
  }
};

}

#endif  // __ARKANOID_BALL__H__

#ifndef __ARKANOID_BITE__H__
#define __ARKANOID_BITE__H__

#include "BiteDimens.h"
#include "Params.h"

namespace game {

struct Bite {
  BiteDimens dimens;
  GLfloat radius;
  GLfloat x_pose;
  GLfloat x_velocity;

  Bite(GLfloat aspect = 1.0f)
    : dimens(aspect)
    , radius(BiteParams::radius)
    , x_pose(0.0f)
    , x_velocity(0.0f) {
  }
};

}

#endif  // __ARKANOID_BITE__H__

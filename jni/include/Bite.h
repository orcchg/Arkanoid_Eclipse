#ifndef __ARKANOID_BITE__H__
#define __ARKANOID_BITE__H__

#include "BiteDimens.h"
#include "Params.h"

namespace game {

struct Bite {
  BiteDimens dimens;
  GLfloat x_pose;
  GLfloat x_velocity;
  GLfloat mass;

  Bite()
    : dimens()
    , x_pose(0.0f)
    , x_velocity(0.0f)
    , mass(BiteParams::biteMass) {
  }
};

}

#endif  // __ARKANOID_BITE__H__

#ifndef __ARKANOID_BITE__H__
#define __ARKANOID_BITE__H__

#include "BiteDimens.h"
#include "Params.h"

namespace game {

class Bite {
public:
  Bite(GLfloat width = 0.f, GLfloat height = 0.f)
    : m_dimens(width, height)
    , m_radius(BiteParams::radius)
    , m_pose(0.0f) {
  }

  inline const BiteDimens& getDimens() const { return m_dimens; }
  inline GLfloat getRadius() const { return m_radius; }
  inline GLfloat getXPose() const { return m_pose; }

  inline void setXPose(GLfloat pose) { m_pose = pose; }

private:
  BiteDimens m_dimens;
  GLfloat m_radius;
  GLfloat m_pose;
};

}

#endif  // __ARKANOID_BITE__H__

#ifndef __ARKANOID_BALL__H__
#define __ARKANOID_BALL__H__

#include "BallDimens.h"
#include "BallPosition.h"
#include "Params.h"

namespace game {

enum class BallEffect : int {
  NONE = 0,
  EASY = 1,
  EASY_T = 2,
  EXPLODE = 3,
  GOO = 4,
  MIRROR = 5,
  PIERCE = 6,
  RANDOM = 7,
  ZYGOTE = 8
};

class Ball {
public:
  Ball(GLfloat width = 0.f, GLfloat height = 0.f)
    : m_dimens(width, height)
    , m_pose()
    , m_angle(0.0f)
    , m_velocity(BallParams::ballSpeed)
    , m_effect(BallEffect::NONE) {
  }

  inline const BallDimens& getDimens() const { return m_dimens; }
  inline const BallPosition& getPose() const { return m_pose; }
  inline GLfloat getAngle() const { return m_angle; }
  inline GLfloat getVelocity() const { return m_velocity; }
  inline BallEffect getEffect() const { return m_effect; }

  inline void setXPose(GLfloat x_pose) { m_pose.setX(x_pose); }
  inline void setYPose(GLfloat y_pose) { m_pose.setY(y_pose); }
  inline void setAngle(GLfloat angle) { m_angle = angle; }
  inline void setEffect(BallEffect effect) { m_effect = effect; }

 private:
  BallDimens m_dimens;
  BallPosition m_pose;  //!< Location of ball's center.
  GLfloat m_angle;  //!< Angle (radian) between velocity and positive X axis.
  GLfloat m_velocity;
  BallEffect m_effect;
};

}

#endif  // __ARKANOID_BALL__H__

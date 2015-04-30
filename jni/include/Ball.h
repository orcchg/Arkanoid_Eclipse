#ifndef __ARKANOID_BALL__H__
#define __ARKANOID_BALL__H__

#include "BallDimens.h"
#include "BallPosition.h"
#include "Params.h"

namespace game {

class Ball {
public:
  Ball(GLfloat width = 0.f, GLfloat height = 0.f)
    : m_dimens(width, height)
    , m_pose()
    , m_angle(0.0f)
    , m_velocity(BallParams::ballSpeed) {
  }

  inline const BallDimens& getDimens() const { return m_dimens; }
  inline const BallPosition& getPose() const { return m_pose; }
  inline GLfloat getAngle() const { return m_angle; }
  inline GLfloat getVelocity() const { return m_velocity; }

  inline void setXPose(GLfloat x_pose) { m_pose.setX(x_pose); }
  inline void setYPose(GLfloat y_pose) { m_pose.setY(y_pose); }
  inline void setAngle(GLfloat angle) { m_angle = angle; }

 private:
  BallDimens m_dimens;
  BallPosition m_pose;  //!< Location of ball's center.
  GLfloat m_angle;  //!< Angle (radian) between velocity and positive X axis.
  GLfloat m_velocity;
};

}

#endif  // __ARKANOID_BALL__H__

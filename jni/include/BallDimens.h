#ifndef __ARKANOID_BALL_DIMENS__H__
#define __ARKANOID_BALL_DIMENS__H__

#include <GLES/gl.h>

#include "Params.h"

namespace game {

/// @brief Measured ball dimensions.
struct BallDimens {
  GLfloat width, height;

private:
  GLfloat aspect;
  GLfloat half_width, half_height;

public:
  BallDimens(GLfloat aspect = 1.0f, GLfloat w = BallParams::ballSize, GLfloat h = BallParams::ballSize)
    : aspect(aspect)
    , width(w)
    , height(h * aspect)
    , half_width(w * 0.5f)
    , half_height(height * 0.5f) {
  }

  inline GLfloat halfWidth() { return half_width; }
  inline GLfloat halfHeight() { return half_height; }
};

}

#endif  // __ARKANOID_BALL_DIMENS__H__

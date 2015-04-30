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

public:
  BallDimens(GLfloat aspect = 1.0f, GLfloat w = BallParams::ballSize, GLfloat h = BallParams::ballSize)
    : aspect(aspect)
    , width(w)
    , height(h * aspect) {
  }

  inline GLfloat halfWidth() { return width * 0.5f; }
  inline GLfloat halfHeight() { return height * 0.5f; }
};

}

#endif  // __ARKANOID_BALL_DIMENS__H__

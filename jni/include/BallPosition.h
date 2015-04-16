#ifndef __ARKANOID_BALLPOSITION__H__
#define __ARKANOID_BALLPOSITION__H__

#include <GLES/gl.h>

namespace game {

struct BallPosition {
  GLfloat x, y;

  BallPosition(GLfloat x = 0.0f, GLfloat y = 0.0f)
    : x(x)
    , y(y) {
  }
};

}

#endif /* __ARKANOID_BALLPOSITION__H__ */

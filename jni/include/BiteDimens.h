#ifndef __ARKANOID_BITE_DIMENS__H__
#define __ARKANOID_BITE_DIMENS__H__

#include <GLES/gl.h>

#include "Params.h"

namespace game {

/// @brief Measured bite dimensions.
struct BiteDimens {
  GLfloat width, height;

private:
  GLfloat aspect;

public:
  BiteDimens(GLfloat aspect = 1.0f, GLfloat w = BiteParams::biteWidth, GLfloat h = BiteParams::biteHeight)
    : aspect(aspect)
    , width(w)
    , height(h * aspect) {
  }

  inline GLfloat halfWidth() { return width * 0.5f; }
  inline GLfloat halfHeight() { return height * 0.5f; }
  inline GLfloat quarterWidth() { return width * 0.25f; }
  inline GLfloat quarterHeight() { return height * 0.25f; }
};

}

#endif  // __ARKANOID_BITE_DIMENS__H__

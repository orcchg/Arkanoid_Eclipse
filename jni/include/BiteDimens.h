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
};

}

#endif  // __ARKANOID_BITE_DIMENS__H__

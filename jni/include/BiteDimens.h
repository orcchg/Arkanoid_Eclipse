#ifndef __ARKANOID_BITE_DIMENS__H__
#define __ARKANOID_BITE_DIMENS__H__

#include <GLES/gl.h>

#include "Params.h"

namespace game {

struct BiteDimens {
  GLfloat width, height;

  BiteDimens(GLfloat w = BiteParams::biteWidth, GLfloat h = BiteParams::biteHeight)
    : width(w), height(h) {
  }
};

}

#endif  // __ARKANOID_BITE_DIMENS__H__

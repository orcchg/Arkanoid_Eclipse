#ifndef __ARKANOID_LEVEL_DIMENS__H__
#define __ARKANOID_LEVEL_DIMENS__H__

#include <GLES/gl.h>

namespace game {

/// @brief Measured level dimensions.
struct LevelDimens {
  constexpr static float blockWidth = 0.2f;
  constexpr static float blockHeight = 0.1f;

  GLfloat width, height, block_width, block_height;

  LevelDimens(
      GLfloat w,
      GLfloat h,
      GLfloat bw = blockWidth,
      GLfloat bh = blockHeight)
    : width(w)
    , height(h)
    , block_width(bw)
    , block_height(bh) {
  }
};

}

#endif  // __ARKANOID_LEVEL_DIMENS__H__

#ifndef __ARKANOID_LEVEL_DIMENS__H__
#define __ARKANOID_LEVEL_DIMENS__H__

#include <GLES/gl.h>

namespace game {

/// @brief Measured level dimensions.
struct LevelDimens {
  constexpr static float blockWidth = 0.2f;
  constexpr static float blockHeight = 1.1f;

  size_t rows, cols;
  GLfloat width, height, block_width, block_height;

  LevelDimens(
      size_t rows,
      size_t cols,
      GLfloat w,
      GLfloat h,
      GLfloat bw = blockWidth,
      GLfloat bh = blockHeight)
    : rows(rows)
    , cols(cols)
    , width(w)
    , height(h)
    , block_width(bw)
    , block_height(bh) {
  }

  void getBlockDimens(
      size_t row,
      size_t col,
      GLfloat* top_border,
      GLfloat* bottom_border,
      GLfloat* left_border,
      GLfloat* right_border);
};

}

#endif  // __ARKANOID_LEVEL_DIMENS__H__

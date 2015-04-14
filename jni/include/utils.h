#ifndef __ARKANOID_UTILS__H__
#define __ARKANOID_UTILS__H__

#include "rgbstruct.h"

namespace util {

template <typename T>
void setColor(const BGRA<T>& bgra, T* const color_buffer) {
  color_buffer[0] = bgra.b;
  color_buffer[1] = bgra.g;
  color_buffer[2] = bgra.r;
  color_buffer[3] = bgra.a;
}

}

#endif  // __ARKANOID_UTILS__H__

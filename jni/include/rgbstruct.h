#ifndef __ARKANOID_RGBSTRUCT__H__
#define __ARKANOID_RGBSTRUCT__H__

#include <GLES/gl.h>

namespace util {

template <typename T>
struct BGRA {
  T b, g, r, a;

  BGRA()
    : b(0), g(0), r(0), a(0) {
  }

  BGRA(const T* const bgra)
    : b(bgra[0]), g(bgra[1]), r(bgra[2]), a(bgra[3]) {
  }

  BGRA(T b, T g, T r, T a)
    : b(b), g(g), r(r), a(a) {
  }

  BGRA(const BGRA& obj)
    : b(obj.b), g(obj.g), r(obj.r), a(obj.a) {
  }

  BGRA& operator = (const BGRA& obj) {
    b = obj.b; g = obj.g; r = obj.r; a = obj.a;
    return *this;
  }
};

// @see http://stackoverflow.com/questions/13865842/does-static-constexpr-variable-make-sense
//constexpr static GLshort BLACK[4]   = { 0,   0,   0,   255 };
//constexpr static GLshort RED[4]     = { 255, 0,   0,   255 };
//constexpr static GLshort GREEN[4]   = { 0,   255, 0,   255 };
//constexpr static GLshort BLUE[4]    = { 0,   0,   255, 255 };
//constexpr static GLshort CYAN[4]    = { 0,   255, 255, 255 };
//constexpr static GLshort MAGENTA[4] = { 152, 0,   255, 255 };
//constexpr static GLshort PURPLE[4]  = { 152, 0,   102, 255 };
//constexpr static GLshort YELLOW[4]  = { 0,   255, 255, 255 };
//constexpr static GLshort ORANGE[4]  = { 0,   102, 255, 255 };
//constexpr static GLshort MESHLAB[4] = { 207, 102, 102, 255 };
//constexpr static GLshort TRANSPARENT[4] = { 0, 0, 0,   0};

constexpr static GLfloat BLACK[4]   = { 0.f,     0.f,  0.f,  1.f };
constexpr static GLfloat RED[4]     = { 1.f,     0.f,  0.f,  1.f };
constexpr static GLfloat GREEN[4]   = { 0.f,     1.f,  0.f,  1.f };
constexpr static GLfloat BLUE[4]    = { 0.f,     0.f,  1.f,  1.f };
constexpr static GLfloat CYAN[4]    = { 0.f,     1.f,  1.f,  1.f };
constexpr static GLfloat MAGENTA[4] = { 0.5961f, 0.f,  1.f,  1.f };
constexpr static GLfloat PURPLE[4]  = { 0.5961f, 0.f,  0.4f, 1.f };
constexpr static GLfloat YELLOW[4]  = { 0.f,     1.f,  1.f,  1.f };
constexpr static GLfloat ORANGE[4]  = { 0.f,     0.4f, 1.f,  1.f };
constexpr static GLfloat MESHLAB[4] = { 0.8118f, 0.4f, 0.4f, 1.f };
constexpr static GLfloat TRANSPARENT[4] = { 0.f, 0.f, 0.f,   0.f};

}

#endif  // __ARKANOID_RGBSTRUCT__H__

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
constexpr static GLfloat BLACK[4]   = { 0.f,     0.f,  0.f,  1.f };
constexpr static GLfloat RED[4]     = { 1.f,     0.f,  0.f,  1.f };
constexpr static GLfloat GREEN[4]   = { 0.f,     1.f,  0.f,  1.f };
constexpr static GLfloat BLUE[4]    = { 0.f,     0.f,  1.f,  1.f };
constexpr static GLfloat CYAN[4]    = { 0.f,     1.f,  1.f,  1.f };
constexpr static GLfloat MAGENTA[4] = { 0.5961f, 0.f,  1.f,  1.f };
constexpr static GLfloat PURPLE[4]  = { 0.5961f, 0.f,  0.4f, 1.f };
constexpr static GLfloat YELLOW[4]  = { 0.f,     1.f,  1.f,  1.f };
constexpr static GLfloat ORANGE[4]  = { 1.f,     0.4f, 0.f,  1.f };
constexpr static GLfloat MESHLAB[4] = { 0.8118f, 0.4f, 0.4f, 1.f };
constexpr static GLfloat TRANSPARENT[4] = { 0.f, 0.f, 0.f,   0.f};

// http://www.color-hex.com/color-names.html
constexpr static GLfloat ALUMINIUM[4] = {0.7843f, 0.7843f, 0.8f   , 1.f};
constexpr static GLfloat BRICK[4]     = {0.6980f, 0.1333f, 0.1333f, 1.f};
constexpr static GLfloat CLAY[4]      = {1.f,     0.8275,  0.6078f, 1.f};
constexpr static GLfloat DEATH[4]     = {0.f,     0.3922f, 0.f,     1.f};
constexpr static GLfloat ELECTRO[4]   = {0.f,     0.f,     0.9333f, 1.f};
constexpr static GLfloat FOG[4]       = {0.8784f, 0.9333f, 0.9333f, 1.f};
constexpr static GLfloat GLASS[4]     = {0.4980f, 1.f,     0.8314f, 1.f};
constexpr static GLfloat HYPER[4]     = {0.5412f, 0.1686f, 0.8863f, 1.f};
constexpr static GLfloat IRON[4]      = {0.5137f, 0.5451f, 0.5451f, 1.f};
constexpr static GLfloat JELLY[4]     = {0.8039f, 0.2f,    0.2f,    1.f};
constexpr static GLfloat KNOCK[4]     = {1.f,     0.4980f, 0.1412f, 1.f};
constexpr static GLfloat STEEL[4]     = {0.3255f, 0.5255f, 0.5451f, 1.f};
constexpr static GLfloat MAGIC[4]     = {0.6039f, 0.1961f, 0.8039f, 1.f};
constexpr static GLfloat NETWORK[4]   = {1.f,     0.8431f, 0.f,     1.f};
constexpr static GLfloat ORIGIN[4]    = {0.f,     0.5451f, 0.2706f, 1.f};
constexpr static GLfloat PLUMBUM[4]   = {0.5451f, 0.4824f, 0.5451f, 1.f};
constexpr static GLfloat QUICK[4]     = {0.1961f, 0.8039f, 0.1961f, 1.f};
constexpr static GLfloat ROLLING[4]   = {0.5455f, 0.2431f, 0.1843f, 1.f};
constexpr static GLfloat SIMPLE[4]    = {0.6353f, 0.8039f, 0.3529f, 1.f};
constexpr static GLfloat TITAN[4]     = {0.9333f, 0.6784f, 0.0549f, 1.f};
constexpr static GLfloat ULTRA[4]     = {0.5451f, 0.1333f, 0.3216f, 1.f};
constexpr static GLfloat INVUL[4]     = {0.4157f, 0.3529f, 0.8039f, 1.f};
constexpr static GLfloat WATER[4]     = {0.f,     0.6039f, 0.8039f, 1.f};
constexpr static GLfloat EXTRA[4]     = {0.5137f, 0.4353f, 1.f,     1.f};
constexpr static GLfloat YOGURT[4]    = {1.f,     0.9608f, 0.9333f, 1.f};
constexpr static GLfloat ZYGOTE[4]    = {0.8039f, 0.f,     0.f,     1.f};

}

#endif  // __ARKANOID_RGBSTRUCT__H__

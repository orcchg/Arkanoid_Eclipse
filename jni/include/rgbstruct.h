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
constexpr static GLfloat ALUMINIUM[4] = {0.7843f, 0.7843f, 0.8f   , 1.f};  // 200 200 204
constexpr static GLfloat ALUMINIUM_EDGE[4] = {};

constexpr static GLfloat BRICK[4]     = {0.6980f, 0.1333f, 0.1333f, 1.f};  // 178 34  34
constexpr static GLfloat BRICK_EDGE[4] = {};

constexpr static GLfloat CLAY[4]      = {1.f,     0.8275,  0.6078f, 1.f};  // 255 211 155
constexpr static GLfloat CLAY_EDGE[4] = {};

constexpr static GLfloat DEATH[4]     = {0.f,     0.3922f, 0.f,     1.f};  // 0   100 0
constexpr static GLfloat DEATH_EDGE[4] = {};

constexpr static GLfloat ELECTRO[4]   = {0.f,     0.f,     0.9333f, 1.f};  // 0   0   238
constexpr static GLfloat ELECTRO_EDGE[4] = {};

constexpr static GLfloat FOG[4]       = {0.8784f, 0.9333f, 0.9333f, 1.f};  // 224 238 238
constexpr static GLfloat FOG_EDGE[4] = {};

constexpr static GLfloat GLASS[4]     = {0.4980f, 1.f,     0.8314f, 1.f};  // 127 255 212
constexpr static GLfloat GLASS_EDGE[4] = {};

constexpr static GLfloat HYPER[4]     = {0.5412f, 0.1686f, 0.8863f, 1.f};  // 138 43  226
constexpr static GLfloat HYPER_EDGE[4] = {};

constexpr static GLfloat IRON[4]      = {0.5137f, 0.5451f, 0.5451f, 1.f};  // 131 139 139
constexpr static GLfloat IRON_EDGE[4] = {};

constexpr static GLfloat JELLY[4]     = {0.8039f, 0.2f,    0.2f,    1.f};  // 205 51  51
constexpr static GLfloat JELLY_EDGE[4] = {};

constexpr static GLfloat KNOCK[4]     = {1.f,     0.4980f, 0.1412f, 1.f};  // 255 127 36
constexpr static GLfloat KNOCK_EDGE[4] = {};

constexpr static GLfloat STEEL[4]     = {0.3255f, 0.5255f, 0.5451f, 1.f};  // 83  134 139
constexpr static GLfloat STEEL_EDGE[4] = {};

constexpr static GLfloat MAGIC[4]     = {0.6039f, 0.1961f, 0.8039f, 1.f};  // 154 50  205
constexpr static GLfloat MAGIC_EDGE[4] = {};

constexpr static GLfloat MIDAS[4]     = {0.5451f, 0.4588f, 0.f,     1.f};  // 139 117 0
constexpr static GLfloat MIDAS_EDGE[4] = {};

constexpr static GLfloat NETWORK[4]   = {1.f,     0.8431f, 0.f,     1.f};  // 255 215 0
constexpr static GLfloat NETWORK_EDGE[4] = {};

constexpr static GLfloat ORIGIN[4]    = {0.f,     0.5451f, 0.2706f, 1.f};  // 0   139 69
constexpr static GLfloat ORIGIN_EDGE[4] = {};

constexpr static GLfloat PLUMBUM[4]   = {0.5451f, 0.4824f, 0.5451f, 1.f};  // 139 123 139
constexpr static GLfloat PLUMBUM_EDGE[4] = {};

constexpr static GLfloat QUICK[4]     = {0.1961f, 0.8039f, 0.1961f, 1.f};  // 50  205 50
constexpr static GLfloat QUICK_EDGE[4] = {};

constexpr static GLfloat ROLLING[4]   = {0.5455f, 0.2431f, 0.1843f, 1.f};  // 139 62  47
constexpr static GLfloat ROLLING_EDGE[4] = {};

constexpr static GLfloat SIMPLE[4]    = {0.6353f, 0.8039f, 0.3529f, 1.f};  // 162 205 90
constexpr static GLfloat SIMPLE_EDGE[4] = {};

constexpr static GLfloat TITAN[4]     = {0.9333f, 0.6784f, 0.0549f, 1.f};  // 238 173 14
constexpr static GLfloat TITAN_EDGE[4] = {};

constexpr static GLfloat ULTRA[4]     = {0.5451f, 0.1333f, 0.3216f, 1.f};  // 139 34  82
constexpr static GLfloat ULTRA_EDGE[4] = {};

constexpr static GLfloat INVUL[4]     = {0.4157f, 0.3529f, 0.8039f, 1.f};  // 106 90  205
constexpr static GLfloat INVUL_EDGE[4] = {};

constexpr static GLfloat WATER[4]     = {0.f,     0.6039f, 0.8039f, 1.f};  // 0   154 205
constexpr static GLfloat WATER_EDGE[4] = {};

constexpr static GLfloat EXTRA[4]     = {0.5137f, 0.4353f, 1.f,     1.f};  // 131 111 255
constexpr static GLfloat EXTRA_EDGE[4] = {};

constexpr static GLfloat YOGURT[4]    = {1.f,     0.9608f, 0.9333f, 1.f};  // 255 245 238
constexpr static GLfloat YOGURT_EDGE[4] = {};

constexpr static GLfloat ZYGOTE[4]    = {0.8039f, 0.f,     0.f,     1.f};  // 205 0   0
constexpr static GLfloat ZYGOTE_EDGE[4] = {};

}

#endif  // __ARKANOID_RGBSTRUCT__H__

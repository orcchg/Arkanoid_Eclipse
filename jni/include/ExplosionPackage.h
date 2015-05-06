#ifndef __ARKANOID_EXPLOSION_PACKAGE__H__
#define __ARKANOID_EXPLOSION_PACKAGE__H__

#include <GLES/gl.h>
#include "rgbstruct.h"

namespace game {

enum class Kind : int {
  DIVERGE = 0,
  CONVERGE = 1
};

class ExplosionPackage {
public:
  ExplosionPackage();
  ExplosionPackage(GLfloat x, GLfloat y, const util::BGRA<GLfloat>& color, Kind kind);
  virtual ~ExplosionPackage();

  inline GLfloat getX() const { return x; }
  inline GLfloat getY() const { return y; }
  inline const util::BGRA<GLfloat>& getColor() const { return color; }
  inline Kind getKind() const { return kind; }

private:
  GLfloat x, y;
  util::BGRA<GLfloat> color;
  Kind kind;
};

}

#endif  // __ARKANOID_EXPLOSION_PACKAGE__H__

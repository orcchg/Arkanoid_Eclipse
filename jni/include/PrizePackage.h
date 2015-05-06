#ifndef __ARKANOID_PRIZE_PACKAGE__H__
#define __ARKANOID_PRIZE_PACKAGE__H__

#include <GLES/gl.h>
#include "Prize.h"

namespace game {

class PrizePackage {
public:
  PrizePackage();
  PrizePackage(GLfloat x, GLfloat y, Prize prize);
  virtual ~PrizePackage();

  inline GLfloat getX() const { return x; }
  inline GLfloat getY() const { return y; }
  inline Prize getPrize() const { return prize; }

private:
  GLfloat x, y;
  Prize prize;
};

}

#endif  // __ARKANOID_PRIZE_PACKAGE__H__

#ifndef __ARKANOID_PRIZE_PACKAGE__H__
#define __ARKANOID_PRIZE_PACKAGE__H__

#include <atomic>
#include <GLES/gl.h>

#include "Prize.h"

namespace game {

class PrizePackage {
public:
  PrizePackage();
  PrizePackage(GLfloat x, GLfloat y, Prize prize);
  virtual ~PrizePackage();
  PrizePackage(const PrizePackage& obj);
  PrizePackage(PrizePackage&& rval_obj);
  PrizePackage& operator= (PrizePackage rhs);

  inline int getID() const { return id; }
  inline GLfloat getX() const { return x; }
  inline GLfloat getY() const { return y; }
  inline Prize getPrize() const { return prize; }

private:
  static std::atomic<int> initialId;

  int id;
  GLfloat x, y;
  Prize prize;
};

}

#endif  // __ARKANOID_PRIZE_PACKAGE__H__

#ifndef __ARKANOID_PRIZE_PACKAGE__H__
#define __ARKANOID_PRIZE_PACKAGE__H__

#include <atomic>
#include <GLES/gl.h>

#include "Prize.h"

namespace game {

class PrizePackage {
public:
  friend class GameProcessor;

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
  inline bool hasGone() const { return has_gone; }

  inline void setY(GLfloat new_y) { y = new_y; }
  inline void setGone(bool gone) { has_gone = gone; }

private:
  int id;
  GLfloat x, y;
  Prize prize;
  bool has_gone;

  PrizePackage(int id, GLfloat x, GLfloat y, Prize prize);
};

}

#endif  // __ARKANOID_PRIZE_PACKAGE__H__

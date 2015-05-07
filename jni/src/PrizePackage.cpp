#include <algorithm>
#include "PrizePackage.h"

namespace game {

// http://stackoverflow.com/questions/18175962/cant-use-static-stdatomic-and-dont-know-how-to-initialize-it
std::atomic<int> PrizePackage::initialId(0);

PrizePackage::PrizePackage(GLfloat x, GLfloat y, Prize prize)
  : id(initialId++), x(x), y(y), prize(prize) {
}

PrizePackage::~PrizePackage() {
}

PrizePackage::PrizePackage(const PrizePackage& obj)
  : id(obj.id), x(obj.x), y(obj.y), prize(obj.prize) {
}

PrizePackage::PrizePackage(PrizePackage&& rval_obj)
  : id(rval_obj.id), x(rval_obj.x), y(rval_obj.y), prize(rval_obj.prize) {
  rval_obj.id = -1;
  rval_obj.x = 0.0f;
  rval_obj.y = 0.0f;
  rval_obj.prize = Prize::NONE;
}

PrizePackage& PrizePackage::operator = (PrizePackage rhs) {
  PrizePackage temp = rhs;
  std::swap(id, temp.id);
  std::swap(x, temp.x);
  std::swap(y, temp.y);
  std::swap(prize, temp.prize);
  return *this;
}

PrizePackage::PrizePackage()
  : id(-1), x(0.0f), y(0.0f), prize(Prize::NONE) {
}

}

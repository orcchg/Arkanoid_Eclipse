#include "PrizePackage.h"

namespace game {

PrizePackage::PrizePackage()
  : x(0.0f), y(0.0f), prize(Prize::NONE) {
}

PrizePackage::PrizePackage(GLfloat x, GLfloat y, Prize prize)
  : x(x), y(y), prize(prize) {
}

PrizePackage::~PrizePackage() {
}

}

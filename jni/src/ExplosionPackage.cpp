#include "ExplosionPackage.h"

namespace game {

ExplosionPackage::ExplosionPackage()
  : x(0.0f), y(0.0f), color(util::TRANSPARENT) {
}

ExplosionPackage::ExplosionPackage(
    GLfloat x,
    GLfloat y,
    const util::BGRA<GLfloat>& color)
  : x(x), y(y), color(color) {
}

ExplosionPackage::~ExplosionPackage() {
}

}

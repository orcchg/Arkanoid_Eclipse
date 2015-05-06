#include "ExplosionPackage.h"

namespace game {

ExplosionPackage::ExplosionPackage()
  : x(0.0f), y(0.0f), color(util::TRANSPARENT), kind(Kind::DIVERGE) {
}

ExplosionPackage::ExplosionPackage(
    GLfloat x,
    GLfloat y,
    const util::BGRA<GLfloat>& color,
    Kind kind)
  : x(x), y(y), color(color), kind(kind) {
}

ExplosionPackage::~ExplosionPackage() {
}

}

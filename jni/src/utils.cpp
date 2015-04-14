#include "utils.h"

namespace util {

void printBuffer2D(const GLfloat* const buffer, size_t size) {
  for (size_t i = 0; i < size; i += 2) {
    MSG("%lf %lf", buffer[i], buffer[i + 1]);
  }
}

void printBuffer3D(const GLfloat* const buffer, size_t size) {
  for (size_t i = 0; i < size; i += 3) {
    MSG("%lf %lf %lf", buffer[i], buffer[i + 1], buffer[i + 2]);
  }
}

void printBuffer4D(const GLfloat* const buffer, size_t size) {
  for (size_t i = 0; i < size; i += 4) {
    MSG("%lf %lf %lf %lf", buffer[i], buffer[i + 1], buffer[i + 2], buffer[i + 3]);
  }
}

}

#include "utils.h"

namespace util {

void rectangleIndices(GLushort* const indices, size_t size) {
  size_t j = 0;
  for (size_t i = 0; i < size; i += 6, ++j) {
    indices[i + 0] = 0 + 4 * j;
    indices[i + 1] = 3 + 4 * j;
    indices[i + 2] = 2 + 4 * j;
    indices[i + 3] = 0 + 4 * j;
    indices[i + 4] = 1 + 4 * j;
    indices[i + 5] = 3 + 4 * j;
  }
}

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

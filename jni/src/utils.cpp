#include "utils.h"

namespace util {

void setColor(const GLfloat* const bgra, GLfloat* const color_buffer, size_t size) {
  for (size_t i = 0; i < size; i += 4) {
    color_buffer[i + 0] = bgra[0];
    color_buffer[i + 1] = bgra[1];
    color_buffer[i + 2] = bgra[2];
    color_buffer[i + 3] = bgra[3];
  }
}

void setRectangleVertices(
    GLfloat* const array,
    GLfloat width,
    GLfloat height,
    GLfloat x_offset,
    GLfloat y_offset,
    size_t cols,
    size_t rows) {

  int cols16 = cols * 16;
  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      int index = (c * 16) + (r * cols16);
      // upper left corner
      array[index + 0] = x_offset + width * c;
      array[index + 1] = y_offset - height * r;
      array[index + 2] = 0.0f;
      array[index + 3] = 1.0f;
      // upper right corner
      array[index + 4] = x_offset + width * (c + 1);
      array[index + 5] = y_offset - height * r;
      array[index + 6] = 0.0f;
      array[index + 7] = 1.0f;
      // lower left corner
      array[index + 8] = x_offset + width * c;
      array[index + 9] = y_offset - height * (r + 1);
      array[index + 10] = 0.0f;
      array[index + 11] = 1.0f;
      // lower right corner
      array[index + 12] = x_offset + width * (c + 1);
      array[index + 13] = y_offset - height * (r + 1);
      array[index + 14] = 0.0f;
      array[index + 15] = 1.0f;
    }
  }
}

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

/*
 * Level.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: Maxim Alov <alovmax@yandex.ru>
 */

#include <algorithm>
#include <iterator>
#include <sstream>

#include "Level.h"
#include "logger.h"
#include "utils.h"

namespace game {

Block charToBlock(char ch) {
  switch (ch) {
  case 'O':
    return Block::SIMPLE;
  default:
    return Block::NONE;
  }
}

char blockToChar(Block block) {
  switch (block) {
  case Block::SIMPLE:
    return 'O';
  case Block::NONE:
  default:
    return ' ';
  }
}

Level::Ptr Level::fromStringArray(const std::vector<std::string>& array, size_t length) {
  size_t* widths = new size_t[length];
  for (size_t i = 0; i < length; ++i) {
    widths[i] = array[i].length();
  }
  // all strings which are less than the longer one will be populated
  // with blank characters
  size_t max_width = *std::max_element(widths, widths + length);

  Level::Ptr level = std::shared_ptr<Level>(new Level(length, max_width));
  for (size_t r = 0; r < level->rows; ++r) {
    for (size_t c = 0; c < level->cols; ++c) {
      if (c < widths[r]) {
        level->blocks[r][c] = charToBlock(array[r][c]);
      } else {
        level->blocks[r][c] = Block::NONE;
      }
    }
  }

  delete [] widths;
  widths = nullptr;

  return level;
}

size_t Level::toStringArray(std::vector<std::string>* array) const {
  for (size_t r = 0; r < rows; ++r) {
    std::string line = "";
    for (size_t c = 0; c < cols; ++c) {
      line += blockToChar(blocks[r][c]);
    }
    array->emplace_back(line);
  }
  return rows;
}

void Level::toVertexArray2D(
    GLfloat width,
    GLfloat height,
    GLfloat x_offset,
    GLfloat y_offset,
    GLfloat* const array) const {

  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      int index = (c * 8) + (r * cols * 8);
      // upper left corner
      array[index + 0] = x_offset + width * c;
      array[index + 1] = y_offset + height * r;
      // upper right corner
      array[index + 2] = x_offset + width * (c + 1);
      array[index + 3] = y_offset + height * r;
      // lower left corner
      array[index + 4] = x_offset + width * c;
      array[index + 5] = y_offset + height * (r + 1);
      // lower right corner
      array[index + 6] = x_offset + width * (c + 1);
      array[index + 7] = y_offset + height * (r + 1);
    }
  }
}

void Level::toVertexArray3D(
    GLfloat width,
    GLfloat height,
    GLfloat x_offset,
    GLfloat y_offset,
    GLfloat* const array) const {

  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      int index = (c * 12) + (r * cols * 12);
      // upper left corner
      array[index + 0] = x_offset + width * c;
      array[index + 1] = y_offset + height * r;
      array[index + 2] = 0.0f;
      // upper right corner
      array[index + 3] = x_offset + width * (c + 1);
      array[index + 4] = y_offset + height * r;
      array[index + 5] = 0.0f;
      // lower left corner
      array[index + 6] = x_offset + width * c;
      array[index + 7] = y_offset + height * (r + 1);
      array[index + 8] = 0.0f;
      // lower right corner
      array[index + 9] = x_offset + width * (c + 1);
      array[index + 10] = y_offset + height * (r + 1);
      array[index + 11] = 0.0f;
    }
  }
}

//template <>
//void Level::fillColorArray<GLshort>(GLshort* const array) const {
//  for (size_t r = 0; r < rows; ++r) {
//    for (size_t c = 0; c < cols; ++c) {
//      int index = (c * 4) + (r * cols * 4);
//      switch (blocks[r][c]) {
//        default:
//        case Block::NONE:
//          util::setColor<GLshort>(util::glshort::TRANSPARENT, &array[index]);
//          break;
//        case Block::SIMPLE:
//          util::setColor<GLshort>(util::glshort::GREEN, &array[index]);
//          break;
//      }
//    }
//  }
//}

template <>
void Level::fillColorArray<GLfloat>(GLfloat* const array) const {
  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      int upper_left_i  = 0  + 16 * (r * cols + c);
      int upper_right_i = 4  + 16 * (r * cols + c);
      int lower_left_i  = 8  + 16 * (r * cols + c);
      int lower_right_i = 12 + 16 * (r * cols + c);
      switch (blocks[r][c]) {
        default:
        case Block::NONE:
          util::setColor<GLfloat>(util::TRANSPARENT, &array[upper_left_i]);
          util::setColor<GLfloat>(util::TRANSPARENT, &array[upper_right_i]);
          util::setColor<GLfloat>(util::TRANSPARENT, &array[lower_left_i]);
          util::setColor<GLfloat>(util::TRANSPARENT, &array[lower_right_i]);
          break;
        case Block::SIMPLE:
          util::setColor<GLfloat>(util::GREEN, &array[upper_left_i]);
          util::setColor<GLfloat>(util::GREEN, &array[upper_right_i]);
          util::setColor<GLfloat>(util::GREEN, &array[lower_left_i]);
          util::setColor<GLfloat>(util::GREEN, &array[lower_right_i]);
          break;
      }
    }
  }
}

void Level::print() const {
  std::vector<std::string> array;
  array.reserve(rows);
  toStringArray(&array);
  std::ostringstream oss;
  oss.str("\n");
  std::copy(array.begin(), array.end(), std::ostream_iterator<std::string>(oss, "\n"));
  MSG("%s", oss.str().c_str());
}

Level::Level(size_t rows, size_t cols)
  : rows(rows), cols(cols), blocks(new Block*[rows]) {
  for (size_t r = 0; r < rows; ++r) {
    blocks[r] = new Block[cols];
  }
  DBG("Level::ctor(), size: %i", size());
}

Level::~Level() {
  for (size_t r = 0; r < rows; ++r) {
    delete [] blocks[r];
    blocks[r] = nullptr;
  }
  delete [] blocks;
  blocks = nullptr;
}

}  // namespace game

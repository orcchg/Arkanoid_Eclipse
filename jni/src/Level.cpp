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
    case 'A':
      return Block::ALUMINIUM;
    case 'B':
      return Block::BRICK;
    case 'C':
      return Block::CLAY;
    case 'D':
      return Block::DEATH;
    case 'E':
      return Block::ELECTRO;
    case 'F':
      return Block::FOG;
    case 'G':
      return Block::GAS;
    case 'H':
      return Block::HYPER;
    case 'I':
      return Block::IRON;
      // XXX: add new
    case 'L':
      return Block::STEEL;
      // XXX: add new
    case 'R':
      return Block::ROLLING;
    case 'S':
      return Block::SIMPLE;
    case 'T':
      return Block::TITAN;
      // XXX: add new
    case 'W':
      return Block::WATER;
    default:
      return Block::NONE;
  }
}

char blockToChar(Block block) {
  switch (block) {
    case Block::ALUMINIUM:
      return 'A';
    case Block::BRICK:
      return 'B';
    case Block::CLAY:
      return 'C';
    case Block::DEATH:
      return 'D';
    case Block::ELECTRO:
      return 'E';
    case Block::FOG:
      return 'F';
    case Block::GAS:
      return 'G';
    case Block::HYPER:
      return 'H';
    case Block::IRON:
      return 'I';
      // XXX: add new
    case Block::STEEL:
      return 'L';
      // XXX: add new
    case Block::ROLLING:
      return 'R';
    case Block::SIMPLE:
      return 'S';
    case Block::TITAN:
      return 'T';
      // XXX: add new
    case Block::WATER:
      return 'W';
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
  level->initial_cardinality = level->calculateCardinality();

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

void Level::toVertexArray(
    GLfloat width,
    GLfloat height,
    GLfloat x_offset,
    GLfloat y_offset,
    GLfloat* const array) const {

  util::setRectangleVertices(array, width, height, x_offset, y_offset, cols, rows);
}

void Level::fillColorArray(GLfloat* const array) const {
  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      fillColorArrayAtBlock(array, r, c);
    }
  }
}

void Level::fillColorArrayAtBlock(GLfloat* const array, size_t row, size_t col) const {
  int upper_left_i  = 0  + 16 * (row * cols + col);
  int upper_right_i = 4  + 16 * (row * cols + col);
  int lower_left_i  = 8  + 16 * (row * cols + col);
  int lower_right_i = 12 + 16 * (row * cols + col);
  util::BGRA<GLfloat> bgra;
  switch (blocks[row][col]) {
    default:
    case Block::NONE:
      bgra = util::BGRA<GLfloat>(util::TRANSPARENT);
      break;
    case Block::SIMPLE:
      bgra = util::BGRA<GLfloat>(util::GREEN);
      break;
    case Block::BRICK:
      bgra = util::BGRA<GLfloat>(util::RED);
      break;
    case Block::TITAN:
      bgra = util::BGRA<GLfloat>(util::CYAN);
      break;
  }
  util::setColor(bgra, &array[upper_left_i], 4);
  util::setColor(bgra, &array[upper_right_i], 4);
  util::setColor(bgra, &array[lower_left_i], 4);
  util::setColor(bgra, &array[lower_right_i], 4);
}

int Level::calculateCardinality() const {
  size_t cardinality = 0;
  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      switch (blocks[r][c]) {
        default:
        case Block::TITAN:
        case Block::NONE:
          break;
        case Block::SIMPLE:
          ++cardinality;
          break;
        case Block::BRICK:
          cardinality += 2;
          break;
      }
    }
  }
  return cardinality;
}

void Level::setBlockImpacted(size_t row, size_t col) {
  Block block = getBlock(row, col);
  switch (block) {
    case Block::SIMPLE:
      setBlock(row, col, Block::NONE);
      break;
    case Block::BRICK:
      setBlock(row, col, Block::SIMPLE);
      break;
    case Block::TITAN:
    case Block::NONE:
    default:
      // no-op
      break;
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
  : rows(rows)
  , cols(cols)
  , initial_cardinality(0)
  , blocks(new Block*[rows]) {
  for (size_t r = 0; r < rows; ++r) {
    blocks[r] = new Block[cols];
  }
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

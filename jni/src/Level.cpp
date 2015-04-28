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
    case 'a':
    case 'A':
      return Block::ALUMINIUM;
    case 'b':
    case 'B':
      return Block::BRICK;
    case 'c':
    case 'C':
      return Block::CLAY;
    case 'd':
    case 'D':
      return Block::DEATH;
    case 'e':
    case 'E':
      return Block::ELECTRO;
    case 'f':
    case 'F':
      return Block::FOG;
    case 'g':
    case 'G':
      return Block::GLASS;
    case 'h':
    case 'H':
      return Block::HYPER;
    case 'i':
    case 'I':
      return Block::IRON;
    case 'j':
    case 'J':
      return Block::JELLY;
    case 'k':
    case 'K':
      return Block::KNOCK;
    case 'l':
    case 'L':
      return Block::STEEL;
    case 'm':
    case 'M':
      return Block::MAGIC;
    case 'n':
    case 'N':
      return Block::NETWORK;
    case 'o':
    case 'O':
      return Block::ORIGIN;
    case 'p':
    case 'P':
      return Block::PLUMBUM;
    case 'q':
    case 'Q':
      return Block::QUICK;
    case 'r':
    case 'R':
      return Block::ROLLING;
    case 's':
    case 'S':
      return Block::SIMPLE;
    case 't':
    case 'T':
      return Block::TITAN;
    case 'u':
    case 'U':
      return Block::ULTRA;
    case 'v':
    case 'V':
      return Block::INVUL;
    case 'w':
    case 'W':
      return Block::WATER;
    case 'x':
    case 'X':
      return Block::EXTRA;
    case 'y':
    case 'Y':
      return Block::YOGURT;
    case 'z':
    case 'Z':
      return Block::ZYGOTE;
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
    case Block::GLASS:
      return 'G';
    case Block::HYPER:
      return 'H';
    case Block::IRON:
      return 'I';
    case Block::JELLY:
      return 'J';
    case Block::KNOCK:
      return 'K';
    case Block::STEEL:
      return 'L';
    case Block::MAGIC:
      return 'M';
    case Block::NETWORK:
      return 'N';
    case Block::ORIGIN:
      return 'O';
    case Block::PLUMBUM:
      return 'P';
    case Block::QUICK:
      return 'Q';
    case Block::ROLLING:
      return 'R';
    case Block::SIMPLE:
      return 'S';
    case Block::TITAN:
      return 'T';
    case Block::ULTRA:
      return 'U';
    case Block::INVUL:
      return 'V';
    case Block::WATER:
      return 'W';
    case Block::EXTRA:
      return 'X';
    case Block::YOGURT:
      return 'Y';
    case Block::ZYGOTE:
      return 'Z';
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
    case Block::ALUMINIUM:
      bgra = util::BGRA<GLfloat>(util::ALUMINIUM);
      break;
    case Block::BRICK:
      bgra = util::BGRA<GLfloat>(util::BRICK);
      break;
    case Block::CLAY:
      bgra = util::BGRA<GLfloat>(util::CLAY);
      break;
    case Block::DEATH:
      bgra = util::BGRA<GLfloat>(util::DEATH);
      break;
    case Block::ELECTRO:
      bgra = util::BGRA<GLfloat>(util::ELECTRO);
      break;
    case Block::FOG:
      bgra = util::BGRA<GLfloat>(util::FOG);
      break;
    case Block::GLASS:
      bgra = util::BGRA<GLfloat>(util::GLASS);
      break;
    case Block::HYPER:
      bgra = util::BGRA<GLfloat>(util::HYPER);
      break;
    case Block::IRON:
      bgra = util::BGRA<GLfloat>(util::IRON);
      break;
    case Block::JELLY:
      bgra = util::BGRA<GLfloat>(util::JELLY);
      break;
    case Block::KNOCK:
      bgra = util::BGRA<GLfloat>(util::KNOCK);
      break;
    case Block::STEEL:
      bgra = util::BGRA<GLfloat>(util::STEEL);
      break;
    case Block::MAGIC:
      bgra = util::BGRA<GLfloat>(util::MAGIC);
      break;
    case Block::NETWORK:
    case Block::NETWORK_1:
      bgra = util::BGRA<GLfloat>(util::NETWORK);
      break;
    case Block::ORIGIN:
      bgra = util::BGRA<GLfloat>(util::ORIGIN);
      break;
    case Block::PLUMBUM:
      bgra = util::BGRA<GLfloat>(util::PLUMBUM);
      break;
    case Block::QUICK:
    case Block::QUICK_1:
    case Block::QUICK_2:
      bgra = util::BGRA<GLfloat>(util::QUICK);
      break;
    case Block::ROLLING:
      bgra = util::BGRA<GLfloat>(util::ROLLING);
      break;
    case Block::SIMPLE:
      bgra = util::BGRA<GLfloat>(util::SIMPLE);
      break;
    case Block::TITAN:
      bgra = util::BGRA<GLfloat>(util::TITAN);
      break;
    case Block::ULTRA:
    case Block::ULTRA_1:
    case Block::ULTRA_2:
    case Block::ULTRA_3:
    case Block::ULTRA_4:
      bgra = util::BGRA<GLfloat>(util::ULTRA);
      break;
    case Block::INVUL:
      bgra = util::BGRA<GLfloat>(util::INVUL);
      break;
    case Block::WATER:
      bgra = util::BGRA<GLfloat>(util::WATER);
      break;
    case Block::EXTRA:
      bgra = util::BGRA<GLfloat>(util::EXTRA);
      break;
    case Block::YOGURT:
      bgra = util::BGRA<GLfloat>(util::YOGURT);
      break;
    case Block::ZYGOTE:
    case Block::ZYGOTE_1:
      bgra = util::BGRA<GLfloat>(util::ZYGOTE);
      break;
  }
  util::setColor(bgra, &array[upper_left_i], 4);
  util::setColor(bgra, &array[upper_right_i], 4);
  util::setColor(bgra, &array[lower_left_i], 4);
  util::setColor(bgra, &array[lower_right_i], 4);
}

void Level::setBlockImpacted(size_t row, size_t col) {
  Block block = getBlock(row, col);
  switch (block) {
    case Block::ULTRA:
      setBlock(row, col, Block::ULTRA_4);
      break;
    case Block::ULTRA_4:
      setBlock(row, col, Block::ULTRA_3);
      break;
    case Block::ULTRA_3:
      setBlock(row, col, Block::ULTRA_2);
      break;
    case Block::ULTRA_2:
      setBlock(row, col, Block::ULTRA_1);
      break;
    // ----------------------
    case Block::PLUMBUM:
      setBlock(row, col, Block::STEEL);
      break;
    // ----------------------
    case Block::IRON:
    case Block::STEEL:
      setBlock(row, col, Block::BRICK);
      break;
    // ----------------------
    case Block::QUICK:
      setBlock(row, col, Block::QUICK_2);
      break;
    case Block::QUICK_2:
      setBlock(row, col, Block::QUICK_1);
      break;
    // ----------------------
    case Block::BRICK:
      setBlock(row, col, Block::SIMPLE);
      break;
    // ----------------------
    case Block::NETWORK:
      setBlock(row, col, Block::NETWORK_1);
      break;
    case Block::ZYGOTE:
      setBlock(row, col, Block::ZYGOTE_1);
      break;
    // ----------------------
    case Block::ALUMINIUM:
    case Block::CLAY:
    case Block::DEATH:
    case Block::ELECTRO:
    case Block::FOG:
    case Block::GLASS:
    case Block::HYPER:
    case Block::JELLY:
    case Block::KNOCK:
    case Block::MAGIC:
    case Block::ORIGIN:
    case Block::ROLLING:
    case Block::SIMPLE:
    case Block::WATER:
    case Block::EXTRA:
    case Block::YOGURT:
    // ----------------------
    case Block::NETWORK_1:
    case Block::QUICK_1:
    case Block::ULTRA_1:
    case Block::ZYGOTE_1:
      setBlock(row, col, Block::NONE);
      break;
    // ----------------------
    case Block::TITAN:
    case Block::INVUL:
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

// ----------------------------------------------------------------------------
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

int Level::calculateCardinality() const {
  size_t cardinality = 0;
  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      switch (blocks[r][c]) {
        case Block::ULTRA:
          cardinality += 5;
          break;
        case Block::PLUMBUM:
          cardinality += 4;
          break;
        case Block::IRON:
        case Block::STEEL:
        case Block::QUICK:
          cardinality += 3;
          break;
        case Block::BRICK:
        case Block::NETWORK:
        case Block::ZYGOTE:
          cardinality += 2;
          break;
        case Block::ALUMINIUM:
        case Block::CLAY:
        case Block::DEATH:
        case Block::ELECTRO:
        case Block::FOG:
        case Block::GLASS:
        case Block::HYPER:
        case Block::JELLY:
        case Block::KNOCK:
        case Block::MAGIC:
        case Block::ORIGIN:
        case Block::ROLLING:
        case Block::SIMPLE:
        case Block::WATER:
        case Block::EXTRA:
        case Block::YOGURT:
          ++cardinality;
          break;

        default:
        case Block::TITAN:
        case Block::INVUL:
        case Block::NONE:
          break;
      }
    }
  }
  return cardinality;
}

}  // namespace game

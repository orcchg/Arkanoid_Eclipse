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

Level::Ptr Level::fromStringArray(const std::vector<std::string>& array, size_t length) {
  size_t* widths = new size_t[length];
  for (size_t i = 0; i < length; ++i) {
    widths[i] = array[i].length();
  }
  // all strings which are less than the longer one will be populated
  // with blank characters
  size_t max_width = *std::max_element(widths, widths + length);

  Level::Ptr level = std::shared_ptr<Level>(new Level(length, max_width));
  for (int r = 0; r < level->rows; ++r) {
    for (int c = 0; c < level->cols; ++c) {
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
  for (int r = 0; r < rows; ++r) {
    std::string line = "";
    for (int c = 0; c < cols; ++c) {
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
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      fillColorArrayAtBlock(array, r, c);
    }
  }
}

void Level::fillColorArrayAtBlock(GLfloat* const array, int row, int col) const {
  int upper_left_i  = 0  + 16 * (row * cols + col);
  int upper_right_i = 4  + 16 * (row * cols + col);
  int lower_left_i  = 8  + 16 * (row * cols + col);
  int lower_right_i = 12 + 16 * (row * cols + col);

  util::BGRA<GLfloat> bgra, bgra_edge;
  switch (blocks[row][col]) {
    default:
    case Block::NONE:
      bgra = util::BGRA<GLfloat>(util::TRANSPARENT);
      bgra_edge = util::BGRA<GLfloat>(util::TRANSPARENT);
      break;
    case Block::ALUMINIUM:
      bgra = util::BGRA<GLfloat>(util::ALUMINIUM);
      bgra_edge = util::BGRA<GLfloat>(util::ALUMINIUM_EDGE);
      break;
    case Block::BRICK:
      bgra = util::BGRA<GLfloat>(util::BRICK);
      bgra_edge = util::BGRA<GLfloat>(util::BRICK_EDGE);
      break;
    case Block::CLAY:
      bgra = util::BGRA<GLfloat>(util::CLAY);
      bgra_edge = util::BGRA<GLfloat>(util::CLAY_EDGE);
      break;
    case Block::DEATH:
      bgra = util::BGRA<GLfloat>(util::DEATH);
      bgra_edge = util::BGRA<GLfloat>(util::DEATH_EDGE);
      break;
    case Block::ELECTRO:
      bgra = util::BGRA<GLfloat>(util::ELECTRO);
      bgra_edge = util::BGRA<GLfloat>(util::ELECTRO_EDGE);
      break;
    case Block::FOG:
      bgra = util::BGRA<GLfloat>(util::FOG);
      bgra_edge = util::BGRA<GLfloat>(util::FOG_EDGE);
      break;
    case Block::GLASS:
    case Block::GLASS_1:
      bgra = util::BGRA<GLfloat>(util::GLASS);
      bgra_edge = util::BGRA<GLfloat>(util::GLASS_EDGE);
      break;
    case Block::HYPER:
      bgra = util::BGRA<GLfloat>(util::HYPER);
      bgra_edge = util::BGRA<GLfloat>(util::HYPER_EDGE);
      break;
    case Block::IRON:
      bgra = util::BGRA<GLfloat>(util::IRON);
      bgra_edge = util::BGRA<GLfloat>(util::IRON_EDGE);
      break;
    case Block::JELLY:
      bgra = util::BGRA<GLfloat>(util::JELLY);
      bgra_edge = util::BGRA<GLfloat>(util::JELLY_EDGE);
      break;
    case Block::KNOCK_VERTICAL:
    case Block::KNOCK_HORIZONTAL:
      bgra = util::BGRA<GLfloat>(util::KNOCK);
      bgra_edge = util::BGRA<GLfloat>(util::KNOCK_EDGE);
      break;
    case Block::STEEL:
      bgra = util::BGRA<GLfloat>(util::STEEL);
      bgra_edge = util::BGRA<GLfloat>(util::STEEL_EDGE);
      break;
    case Block::MAGIC:
      bgra = util::BGRA<GLfloat>(util::MAGIC);
      bgra_edge = util::BGRA<GLfloat>(util::MAGIC_EDGE);
      break;
    case Block::MIDAS:
      bgra = util::BGRA<GLfloat>(util::MIDAS);
      bgra_edge = util::BGRA<GLfloat>(util::MIDAS_EDGE);
      break;
    case Block::NETWORK:
    case Block::NETWORK_1:
      bgra = util::BGRA<GLfloat>(util::NETWORK);
      bgra_edge = util::BGRA<GLfloat>(util::NETWORK_EDGE);
      break;
    case Block::ORIGIN:
      bgra = util::BGRA<GLfloat>(util::ORIGIN);
      bgra_edge = util::BGRA<GLfloat>(util::ORIGIN_EDGE);
      break;
    case Block::PLUMBUM:
      bgra = util::BGRA<GLfloat>(util::PLUMBUM);
      bgra_edge = util::BGRA<GLfloat>(util::PLUMBUM_EDGE);
      break;
    case Block::QUICK:
    case Block::QUICK_1:
    case Block::QUICK_2:
      bgra = util::BGRA<GLfloat>(util::QUICK);
      bgra_edge = util::BGRA<GLfloat>(util::QUICK_EDGE);
      break;
    case Block::ROLLING:
      bgra = util::BGRA<GLfloat>(util::ROLLING);
      bgra_edge = util::BGRA<GLfloat>(util::ROLLING_EDGE);
      break;
    case Block::SIMPLE:
      bgra = util::BGRA<GLfloat>(util::SIMPLE);
      bgra_edge = util::BGRA<GLfloat>(util::SIMPLE_EDGE);
      break;
    case Block::TITAN:
      bgra = util::BGRA<GLfloat>(util::TITAN);
      bgra_edge = util::BGRA<GLfloat>(util::TITAN_EDGE);
      break;
    case Block::ULTRA:
    case Block::ULTRA_1:
    case Block::ULTRA_2:
    case Block::ULTRA_3:
    case Block::ULTRA_4:
      bgra = util::BGRA<GLfloat>(util::ULTRA);
      bgra_edge = util::BGRA<GLfloat>(util::ULTRA_EDGE);
      break;
    case Block::INVUL:
      bgra = util::BGRA<GLfloat>(util::INVUL);
      bgra_edge = util::BGRA<GLfloat>(util::INVUL_EDGE);
      break;
    case Block::WATER:
      bgra = util::BGRA<GLfloat>(util::WATER);
      bgra_edge = util::BGRA<GLfloat>(util::WATER_EDGE);
      break;
    case Block::EXTRA:
      bgra = util::BGRA<GLfloat>(util::EXTRA);
      bgra_edge = util::BGRA<GLfloat>(util::EXTRA_EDGE);
      break;
    case Block::YOGURT:
    case Block::YOGURT_1:
      bgra = util::BGRA<GLfloat>(util::YOGURT);
      bgra_edge = util::BGRA<GLfloat>(util::YOGURT_EDGE);
      break;
    case Block::ZYGOTE:
    case Block::ZYGOTE_1:
      bgra = util::BGRA<GLfloat>(util::ZYGOTE);
      bgra_edge = util::BGRA<GLfloat>(util::ZYGOTE_EDGE);
      break;
    case Block::ZYGOTE_SPAWN:
      bgra = util::BGRA<GLfloat>(util::ZYGOTE_SPAWN);
      bgra_edge = util::BGRA<GLfloat>(util::ZYGOTE_SPAWN_EDGE);
      break;
  }
  util::setColor(bgra, &array[upper_left_i], 4);
  util::setColor(bgra_edge, &array[upper_right_i], 4);
  util::setColor(bgra_edge, &array[lower_left_i], 4);
  util::setColor(bgra_edge, &array[lower_right_i], 4);
}

void Level::setVulnerableBlock(int row, int col, Block value) {
  if (blocks[row][col] != Block::TITAN &&
      blocks[row][col] != Block::INVUL) {
    setBlock(row, col, value);
    initial_cardinality += getCardinalityCost(value);
  }
}

int Level::getCardinalityCost(Block block) {
  switch (block) {
    case Block::ULTRA:
      return 5;
    case Block::PLUMBUM:
    case Block::ULTRA_4:
      return 4;
    case Block::IRON:
    case Block::STEEL:
    case Block::QUICK:
    case Block::ULTRA_3:
      return 3;
    case Block::BRICK:
    case Block::GLASS:
    case Block::NETWORK:
    case Block::ZYGOTE:
    case Block::QUICK_2:
    case Block::ULTRA_2:
      return 2;
    case Block::ALUMINIUM:
    case Block::CLAY:
    case Block::DEATH:
    case Block::ELECTRO:
    case Block::FOG:
    case Block::GLASS_1:
    case Block::HYPER:
    case Block::JELLY:
    case Block::KNOCK_VERTICAL:
    case Block::KNOCK_HORIZONTAL:
    case Block::MAGIC:
    case Block::ORIGIN:
    case Block::ROLLING:
    case Block::SIMPLE:
    case Block::WATER:
    case Block::YOGURT:
    case Block::YOGURT_1:
    case Block::NETWORK_1:
    case Block::QUICK_1:
    case Block::ULTRA_1:
    case Block::ZYGOTE_1:
    case Block::ZYGOTE_SPAWN:
      return 1;

    default:
    case Block::MIDAS:
    case Block::TITAN:
    case Block::INVUL:
    case Block::EXTRA:
    case Block::NONE:
      return 0;
  }
}

void Level::setBlockImpacted(int row, int col) {
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
    case Block::GLASS:
      setBlock(row, col, Block::GLASS_1);
      break;
    // ----------------------
    case Block::NETWORK:
      setBlock(row, col, Block::NETWORK_1);
      break;
    case Block::YOGURT:
      setBlock(row, col, Block::YOGURT_1);
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
    case Block::GLASS_1:
    case Block::HYPER:
    case Block::JELLY:
    case Block::KNOCK_VERTICAL:
    case Block::KNOCK_HORIZONTAL:
    case Block::MAGIC:
    case Block::ORIGIN:
    case Block::ROLLING:
    case Block::SIMPLE:
    case Block::WATER:
    // ----------------------
    case Block::NETWORK_1:
    case Block::QUICK_1:
    case Block::ULTRA_1:
    case Block::YOGURT_1:
    case Block::ZYGOTE_1:
    case Block::ZYGOTE_SPAWN:
      setBlock(row, col, Block::NONE);
      break;
    // ----------------------
    case Block::MIDAS:
      setBlock(row, col, Block::TITAN);
      break;
    case Block::EXTRA:
      setBlock(row, col, Block::INVUL);
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

void Level::modifyBlocksAround(int row, int col, Block type, std::vector<RowCol>* output) {
  if (row - 2 >= 0) {
    initial_cardinality -= getCardinalityCost(getBlock(row - 2, col));
    setVulnerableBlock(row - 2, col, type);
    output->emplace_back(row - 2, col);
  }

  if (row - 1 >= 0) {
    initial_cardinality -= getCardinalityCost(getBlock(row - 1, col));
    setVulnerableBlock(row - 1, col, type);
    output->emplace_back(row - 1, col);
    if (col - 1 >= 0) {
      initial_cardinality -= getCardinalityCost(getBlock(row - 1, col - 1));
      setVulnerableBlock(row - 1, col - 1, type);
      output->emplace_back(row - 1, col - 1);
    }
    if (col + 1 < cols) {
      initial_cardinality -= getCardinalityCost(getBlock(row - 1, col + 1));
      setVulnerableBlock(row - 1, col + 1, type);
      output->emplace_back(row - 1, col + 1);
    }
  }

  if (row + 1 < rows) {
    initial_cardinality -= getCardinalityCost(getBlock(row + 1, col));
    setVulnerableBlock(row + 1, col, type);
    output->emplace_back(row + 1, col);
    if (col - 1 >= 0) {
      initial_cardinality -= getCardinalityCost(getBlock(row + 1, col - 1));
      setVulnerableBlock(row + 1, col - 1, type);
      output->emplace_back(row + 1, col - 1);
    }
    if (col + 1 < cols) {
      initial_cardinality -= getCardinalityCost(getBlock(row + 1, col + 1));
      setVulnerableBlock(row + 1, col + 1, type);
      output->emplace_back(row + 1, col + 1);
    }
  }

  if (row + 2 < rows) {
    initial_cardinality -= getCardinalityCost(getBlock(row + 2, col));
    setVulnerableBlock(row + 2, col, type);
    output->emplace_back(row + 2, col);
  }

  // ------------------------
  if (col - 2 >= 0) {
    initial_cardinality -= getCardinalityCost(getBlock(row, col - 2));
    setVulnerableBlock(row, col - 2, type);
    output->emplace_back(row, col - 2);
  }
  if (col - 1 >= 0) {
    initial_cardinality -= getCardinalityCost(getBlock(row, col - 1));
    setVulnerableBlock(row, col - 1, type);
    output->emplace_back(row, col - 1);
  }
  if (col + 1 < cols) {
    initial_cardinality -= getCardinalityCost(getBlock(row, col + 1));
    setVulnerableBlock(row, col + 1, type);
    output->emplace_back(row, col + 1);
  }
  if (col + 2 < cols) {
    initial_cardinality -= getCardinalityCost(getBlock(row, col + 2));
    setVulnerableBlock(row, col + 2, type);
    output->emplace_back(row, col + 2);
  }
}

void Level::destroyBlocksAround(int row, int col, std::vector<RowCol>* output) {
  modifyBlocksAround(row, col, Block::NONE, output);
}

void Level::modifyBlocksBehind(int row, int col, Block type, Direction direction, std::vector<RowCol>* output) {
  switch (direction) {
    case Direction::UP:
      --row;
      while (row >= 0) {
        initial_cardinality -= getCardinalityCost(getBlock(row, col));
        setVulnerableBlock(row, col, type);
        output->emplace_back(row, col);
        --row;
      }
      break;
    case Direction::DOWN:
      ++row;
      while (row < rows) {
        initial_cardinality -= getCardinalityCost(getBlock(row, col));
        setVulnerableBlock(row, col, type);
        output->emplace_back(row, col);
        ++row;
      }
      break;
    case Direction::RIGHT:
      ++col;
      while (col < cols) {
        initial_cardinality -= getCardinalityCost(getBlock(row, col));
        setVulnerableBlock(row, col, type);
        output->emplace_back(row, col);
        ++col;
      }
      break;
    case Direction::LEFT:
      --col;
      while (col >= 0) {
        initial_cardinality -= getCardinalityCost(getBlock(row, col));
        setVulnerableBlock(row, col, type);
        output->emplace_back(row, col);
        --col;
      }
      break;
    case Direction::NONE:
    default:
      break;
  }
}

void Level::destroyBlocksBehind(int row, int col, Direction direction, std::vector<RowCol>* output) {
  modifyBlocksBehind(row, col, Block::NONE, direction, output);
}

RowCol Level::modifyBlockNear(int row, int col, Block type) {
  if (row - 1 >= 0) {
    if (col - 1 >= 0) {
      auto block = getBlock(row - 1, col - 1);
      if (block == Block::NONE) {
        setVulnerableBlock(row - 1, col - 1, type);
        return RowCol(row - 1, col - 1);
      }
    }
    if (col + 1 < cols) {
      auto block = getBlock(row - 1, col + 1);
      if (block == Block::NONE) {
        setVulnerableBlock(row - 1, col + 1, type);
        return RowCol(row - 1, col + 1);
      }
    }
  }
  if (row + 1 < rows) {
    if (col - 1 >= 0) {
      auto block = getBlock(row + 1, col - 1);
      if (block == Block::NONE) {
        setVulnerableBlock(row + 1, col - 1, type);
        return RowCol(row + 1, col - 1);
      }
    }
    if (col + 1 < cols) {
      auto block = getBlock(row + 1, col + 1);
      if (block == Block::NONE) {
        setVulnerableBlock(row + 1, col + 1, type);
        return RowCol(row + 1, col + 1);
      }
    }
  }
  if (row - 1 >= 0){
    auto block = getBlock(row - 1, col);
    if (block == Block::NONE) {
      setVulnerableBlock(row - 1, col, type);
      return RowCol(row - 1, col);
    }
  }
  if (row + 1 < rows) {
    auto block = getBlock(row + 1, col);
    if (block == Block::NONE) {
      setVulnerableBlock(row + 1, col, type);
      return RowCol(row + 1, col);
    }
  }
  if (col - 1 >= 0) {
    auto block = getBlock(row, col - 1);
    if (block == Block::NONE) {
      setVulnerableBlock(row, col - 1, type);
      return RowCol(row, col - 1);
    }
  }
  if (col + 1 < cols) {
    auto block = getBlock(row, col + 1);
    if (block == Block::NONE) {
      setVulnerableBlock(row, col + 1, type);
      return RowCol(row, col + 1);
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

// ----------------------------------------------------------------------------
Level::Level(int rows, int cols)
  : rows(rows)
  , cols(cols)
  , initial_cardinality(0)
  , blocks(new Block*[rows])
  , generator() {
  for (int r = 0; r < rows; ++r) {
    blocks[r] = new Block[cols];
  }
}

Level::~Level() {
  for (int r = 0; r < rows; ++r) {
    delete [] blocks[r];
    blocks[r] = nullptr;
  }
  delete [] blocks;
  blocks = nullptr;
}

int Level::calculateCardinality() const {
  int cardinality = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
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
        case Block::GLASS:
        case Block::NETWORK:
        case Block::ZYGOTE:
          cardinality += 2;
          break;
        case Block::ALUMINIUM:
        case Block::CLAY:
        case Block::DEATH:
        case Block::ELECTRO:
        case Block::FOG:
        case Block::HYPER:
        case Block::JELLY:
        case Block::KNOCK_VERTICAL:
        case Block::KNOCK_HORIZONTAL:
        case Block::MAGIC:
        case Block::ORIGIN:
        case Block::ROLLING:
        case Block::SIMPLE:
        case Block::WATER:
        case Block::YOGURT:
        case Block::ZYGOTE_SPAWN:
          ++cardinality;
          break;

        default:
        case Block::MIDAS:
        case Block::TITAN:
        case Block::INVUL:
        case Block::EXTRA:
        case Block::NONE:
          break;
      }
    }
  }
  return cardinality;
}

}  // namespace game

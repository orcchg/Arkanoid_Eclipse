/*
 * Level.h
 *
 *  Created on: Apr 7, 2015
 *      Author: Maxim Alov <alovmax@yandex.ru>
 */

#ifndef INCLUDE_LEVEL_H_
#define INCLUDE_LEVEL_H_

#include <memory>
#include <string>
#include <vector>

#include <GLES/gl.h>

namespace game {

enum class Block : int {
  NONE = 0,        // ' ' - not disturbing
  ALUMINIUM = 1,   // 'A' - one impact to destroy
  BRICK = 2,       // 'B' - two impacts to destroy
  CLAY = 3,        // 'C' - one impact to destroy, small disturbing
  DEATH = 4,       // 'D' - one impact to destroy, lost ball automatically
  ELECTRO = 5,     // 'E' - one impact to destroy, destroys nearrest blocks
  FOG = 6,         // 'F' - one impact to destroy, not disturbing
  GAS = 7,         // 'G' - one impact to destroy, not disturbing
  HYPER = 8,       // 'H' - one impact to destroy, teleports ball randomly (not lost)
  IRON = 9,        // 'I' - three impacts to destroy
  // XXX: add new
  STEEL = 10,      // 'L' - three impacts to destroy
  // XXX: add new
  ROLLING = 11,    // 'R' - one impact to destroy, random disturbing
  SIMPLE = 12,     // 'S' - one impact to destroy
  TITAN = 13,      // 'T' - invulnerable
  // XXX: add new
  WATER = 14,      // 'W' - one impact to destroy, small disturbing
};

Block charToBlock(char ch);
char blockToChar(Block block);

/**
 * @class Level Level.h "include/Level.h"
 * @brief Represents game level.
 */
class Level {
public:
  typedef std::shared_ptr<Level> Ptr;

  ~Level() noexcept;

  /** @defgroup Convert Convert functions to other data types.
   * @{
   */
  /// @brief Decode string array and get Level instance from it.
  /// @param array Input string array.
  /// @param length Size of imput string array.
  /// @return Level instance.
  static Level::Ptr fromStringArray(const std::vector<std::string>& array, size_t length);

  /// @brief Converts this Level instance to string array.
  /// @param array Pointer to an output string array.
  /// @return Size of output array.
  size_t toStringArray(std::vector<std::string>* array) const;

  /// @brief Converts this Level instance to vertex array.
  /// @param width Width of each block to display.
  /// @param height Height of each block to display.
  /// @param x_offset Horizontal offset from zero for each vertex
  /// in OpenGL coordinate system.
  /// @param y_offset Vertical offset from zero for each vertex
  /// in OpenGL coordinate system.
  /// @param array Output vertex array.
  /// @return Size of output array.
  /// @details Memory for output array should be allocated manually
  /// by Client, required size for allocation is 16 * cols * rows.
  /// @note Each vertex has 2 coordinates (x, y, z=0, w=1).
  void toVertexArray(
      GLfloat width,
      GLfloat height,
      GLfloat x_offset,
      GLfloat y_offset,
      GLfloat* const array) const;
  /** @} */  // end of Convert group

  /// @brief Fills input array with color values corresponding to
  /// blocks in this Level instance.
  /// @param array Output color array.
  /// @details Memory for output array should be allocated manually
  /// by Client, required size for allocation is 16 * cols * rows.
  void fillColorArray(GLfloat* const array) const;
  /// @brief Fills input array with color values at position corresponding
  /// to the specified block in this Level instance.
  /// @param array Output color array.
  /// @param row Row index of specified block.
  /// @param col Column index of specified block.
  /// @details Memory for output array should be allocated manually
  /// by Client, required size for allocation is 16 * cols * rows.
  void fillColorArrayAtBlock(GLfloat* const array, size_t row, size_t col) const;

  /// @brief Returns height of level.
  inline size_t numRows() const { return rows; }
  /// @brief Returns width of level.
  inline size_t numCols() const { return cols; }
  /// @brief Returns total blocks of level.
  inline size_t size() const { return rows * cols; }
  /// @brief Gets block by row and column indices.
  inline Block getBlock(size_t row, size_t col) const { return blocks[row][col]; }
  /// @brief Sets the block by row and column indices.
  inline void setBlock(size_t row, size_t col, Block value) { blocks[row][col] = value; }
  /// @brief Gets recorded cardinality.
  inline int getCardinality() const { return initial_cardinality; }
  /// @brief Decrements recorded cardinality due to impact of some block.
  /// @return Updated cardinality.
  inline int blockImpact() { return --initial_cardinality; }
  /// @brief Calculates current cardinality of this Level instance.
  int calculateCardinality() const;
  /// @brief Updates specified block after single impact.
  /// @param row Row index of impacted block.
  /// @param col Column index of impacted block.
  void setBlockImpacted(size_t row, size_t col);

  void print() const;

private:
  Level(size_t rows, size_t cols);

  size_t rows, cols;
  int initial_cardinality;
  Block** blocks;
};

}  // namespace game

#endif /* INCLUDE_LEVEL_H_ */

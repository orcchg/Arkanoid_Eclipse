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
  NONE = 0,    // ' '
  SIMPLE = 1,  // 'S'
  BRICK = 2    // 'B'
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
  inline size_t getCardinality() const { return initial_cardinality; }
  /// @brief Decrements recorded cardinality due to impact of some block.
  /// @return Updated cardinality.
  inline size_t blockImpact() { return --initial_cardinality; }
  /// @brief Calculates current cardinality of this Level instance.
  size_t calculateCardinality() const;

  void print() const;

private:
  Level(size_t rows, size_t cols);

  size_t rows, cols;
  size_t initial_cardinality;
  Block** blocks;
};

}  // namespace game

#endif /* INCLUDE_LEVEL_H_ */

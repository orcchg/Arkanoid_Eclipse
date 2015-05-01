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

#include "logger.h"
#include "RowCol.h"

namespace game {

enum class Block : int {
  NONE = 0,        //! ' ' - not disturbing

  /* Action blocks */
  DEATH = 1,               //! 'D' - [ 1 ] lost ball automatically
  ELECTRO = 2,             //! 'E' - [ 1 ] destroys nearest blocks
  HYPER = 3,               // 'H' - [ 1 ] teleports ball randomly (not lost)
  KNOCK_VERTICAL = 4,      //! 'K' - [ 1 ] destroys blocks behind
  KNOCK_HORIZONTAL = 5,    //! '#' - [ 1 ] destroys blocks behind
  MAGIC = 6,               // 'M' - [ 1 ] transforms nearest blocks
  NETWORK = 7,             // 'N' - [ 2 ] destroys all other NETWORK blocks
  ORIGIN = 8,              //! 'O' - [ 1 ] puts ball into initial position
  QUICK = 9,               // 'Q' - [ 3 ] transform all other blocks
  ULTRA = 10,              //! 'U' - [ 5 ] win level
  YOGURT = 11,             //! 'Y' - [ 1 ] transforms nearest to YOUGURT blocks
  ZYGOTE = 12,             // 'Z' - [ 2 ] produces additional blocks

  /* Invulnerable blocks */
  TITAN = 13,      //! 'T' - invulnerable
  INVUL = 14,      //! 'V' - invulnerable
  EXTRA = 15,      //! 'X' - [ 1 ] transforms to INVUL
  MIDAS = 16,      // '$' - [ 1 ] transforms nearest to TITAN

  /* Auxiliary blocks */
  GLASS_1 = 17,
  NETWORK_1 = 18,
  QUICK_2 = 19, QUICK_1 = 20,
  ULTRA_4 = 21, ULTRA_3 = 22, ULTRA_2 = 23, ULTRA_1 = 24,
  YOGURT_1 = 25,
  ZYGOTE_1 = 26,

  /* Ordinary blocks */
  ALUMINIUM = 27,   //! 'A' - [ 1 ]
  BRICK = 28,       //! 'B' - [ 2 ]
  CLAY = 29,        //! 'C' - [ 1 ] small disturbing
  FOG = 30,         //! 'F' - [ 1 ] not disturbing
  GLASS = 31,       //! 'G' - [ 2 ], not disturbing
  IRON = 32,        //! 'I' - [ 3 ]
  JELLY = 33,       //! 'J' - [ 1 ] large disturbing
  STEEL = 34,       //! 'L' - [ 3 ]
  PLUMBUM = 35,     //! 'P' - [ 4 ]
  ROLLING = 36,     //! 'R' - [ 1 ] random disturbing
  SIMPLE = 37,      //! 'S' - [ 1 ]
  WATER = 38,       //! 'W' - [ 1 ] small disturbing
};

constexpr static int totalBlockTypes = 36;

Block charToBlock(char ch);
char blockToChar(Block block);

enum class Direction : int {
  NONE = 0,
  UP = 1,
  DOWN = 2,
  RIGHT = 3,
  LEFT = 4
};

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
  /// @param row Row index of specified block.block != Block::
  /// @param col Column index of specified block.
  /// @details Memory for output array should be allocated manually
  /// by Client, required size for allocation is 16 * cols * rows.
  void fillColorArrayAtBlock(GLfloat* const array, int row, int col) const;

  /// @brief Returns height of level.
  inline int numRows() const { return rows; }
  /// @brief Returns width of level.
  inline int numCols() const { return cols; }
  /// @brief Returns total blocks of level.
  inline int size() const { return rows * cols; }
  /// @brief Gets block by row and column indices.
  inline Block getBlock(int row, int col) const { return blocks[row][col]; }
  /// @brief Sets the block by row and column indices.
  inline void setBlock(int row, int col, Block value) { blocks[row][col] = value; }
  /// @brief Sets the block by row and column indices only
  /// in case it is vulnerable.
  void setVulnerableBlock(int row, int col, Block value);
  /// @brief Gets recorded cardinality.
  inline int getCardinality() const { return initial_cardinality; }
  /// @brief Forced way to drop cardinality for instant victory.
  inline void forceDropCardinality() { initial_cardinality = 1; }
  /// @brief Gets cardinality of block.
  static int getCardinalityCost(Block block);
  /// @brief Decrements recorded cardinality due to impact of some block.
  /// @return Updated cardinality.
  inline int blockImpact() { return --initial_cardinality; }

  /** @defgroup Modifiers Functions modifying blocks.
   * @{
   */
  /// @brief Updates specified block after single impact.
  /// @param row Row index of impacted block.
  /// @param col Column index of impacted block.
  void setBlockImpacted(int row, int col);
  /// @brief Modifies blocks around certain block.
  /// @param row Row index of certain block.
  /// @param col Column index of certain block.
  /// @param type Type the block will be modified to.
  /// @param output Array of valid indices of influenced blocks.
  void modifyBlocksAround(int row, int col, Block type, std::vector<RowCol>* output);
  /// @brief Destroys blocks around certain block.
  void destroyBlocksAround(int row, int col, std::vector<RowCol>* output);
  /// @brief Modifies blocks behind certain block.
  /// @param row Row index of certain block.
  /// @param col Column index of certain block.
  /// @param type Type the block will be modified to.
  /// @param direction Direction behind the block.
  /// @param output Array of valid indices of influenced blocks.
  void modifyBlocksBehind(int row, int col, Block type, Direction direction, std::vector<RowCol>* output);
  /// @brief Destroys blocks behind certain block.
  void destroyBlocksBehind(int row, int col, Direction direction, std::vector<RowCol>* output);
  /** @} */  // end of Modifiers group

  void print() const;

private:
  Level(int rows, int cols);

  /// @brief Calculates current cardinality of this Level instance.
  int calculateCardinality() const;

  int rows, cols;
  int initial_cardinality;
  Block** blocks;
};

}  // namespace game

#endif /* INCLUDE_LEVEL_H_ */

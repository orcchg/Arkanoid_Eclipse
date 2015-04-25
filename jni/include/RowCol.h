#ifndef __ARKANOID_ROWCOL__H__
#define __ARKANOID_ROWCOL__H__

namespace game {

struct RowCol {
  int row, col;

  RowCol(int row = 0, int col = 0)
    : row(row), col(col) {
  }
};

}

#endif  // __ARKANOID_ROWCOL__H__

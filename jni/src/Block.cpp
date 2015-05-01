#include "Block.h"

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
      return Block::KNOCK_VERTICAL;
    case '#':
      return Block::KNOCK_HORIZONTAL;
    case 'l':
    case 'L':
      return Block::STEEL;
    case 'm':
    case 'M':
      return Block::MAGIC;
    case '$':
      return Block::MIDAS;
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
    case '@':
      return Block::ZYGOTE_SPAWN;
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
    case Block::KNOCK_VERTICAL:
      return 'K';
    case Block::KNOCK_HORIZONTAL:
      return '#';
    case Block::STEEL:
      return 'L';
    case Block::MAGIC:
      return 'M';
    case Block::MIDAS:
      return '$';
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
    case Block::ZYGOTE_SPAWN:
      return '@';
    case Block::NONE:
    default:
      return ' ';
  }
}

BlockGenerator::BlockGenerator()
  : m_generator()
  , m_distribution(0, totalOrdinaryBlocks - 1) {
}

Block BlockGenerator::generateBlock() {
  int value = ordinaryBlockOffset + m_distribution(m_generator);
  return static_cast<Block>(value);
}

}

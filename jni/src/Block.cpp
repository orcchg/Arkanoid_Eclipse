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
      return Block::DESTROY;
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

    // ------------------------------------------
    case '[':
      return Block::GLASS_1;
    case ']':
      return Block::NETWORK_1;
    case '{':
      return Block::QUICK_2;
    case '}':
      return Block::QUICK_1;
    case '%':
      return Block::ULTRA_4;
    case '^':
      return Block::ULTRA_3;
    case '&':
      return Block::ULTRA_2;
    case '*':
      return Block::ULTRA_1;
    case '(':
      return Block::YOGURT_1;
    case ')':
      return Block::ZYGOTE_1;

    case ' ':
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
    case Block::DESTROY:
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

    // ------------------------------------------
    case Block::GLASS_1:
      return '[';
    case Block::NETWORK_1:
      return ']';
    case Block::QUICK_2:
      return '{';
    case Block::QUICK_1:
      return '}';
    case Block::ULTRA_4:
      return '%';
    case Block::ULTRA_3:
      return '^';
    case Block::ULTRA_2:
      return '&';
    case Block::ULTRA_1:
      return '*';
    case Block::YOGURT_1:
      return '(';
    case Block::ZYGOTE_1:
      return ')';

    case Block::NONE:
    default:
      return ' ';
  }
}

int getCardinalityCost(Block block) {
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
    case Block::DESTROY:
    case Block::MIDAS:
    case Block::TITAN:
    case Block::INVUL:
    case Block::EXTRA:
    case Block::NONE:
      return 0;
  }
}

int getBlockScore(Block block) {
  int score = 0;
  switch (block) {
    case Block::ULTRA:     score += 127;
    case Block::ULTRA_4:   score += 125;
    case Block::ULTRA_3:   score += 122;
    case Block::ULTRA_2:   score += 111;
    case Block::ULTRA_1:   score += 100;
      break;
    case Block::PLUMBUM:   score += 47;
    case Block::STEEL:     score += 39;
    case Block::IRON:      score += 37;
    case Block::ALUMINIUM: score += 25;
      break;
    case Block::QUICK:     score += 28;
    case Block::QUICK_2:   score += 27;
    case Block::QUICK_1:   score += 24;
      break;
    case Block::BRICK:     score += 24;
      break;
    case Block::ROLLING:   score += 5;
    case Block::JELLY:     score += 11;
    case Block::CLAY:      score += 12;
      break;
    case Block::WATER:     score += 5;
    case Block::SIMPLE:    score += 4;
      break;
    case Block::KNOCK_VERTICAL:
    case Block::KNOCK_HORIZONTAL:
      score += 15;
      break;
    case Block::ZYGOTE:    score += 3;
    case Block::ZYGOTE_1:  score += 2;
    case Block::YOGURT:    score += 7;
    case Block::YOGURT_1:  score += 1;
    case Block::ZYGOTE_SPAWN: score += 1;
      break;
    case Block::GLASS:     score += 1;
    case Block::GLASS_1:   score += 1;
    case Block::FOG:       score += 1;
      break;
    case Block::NETWORK:   score += 11;
    case Block::NETWORK_1: score += 1;
    case Block::ELECTRO:   score += 9;
      break;
    case Block::HYPER:     score += 1;
    case Block::MAGIC:     score += 8;
    case Block::ORIGIN:    score += 2;
      break;

    default:
    case Block::DESTROY:
    case Block::MIDAS:
    case Block::TITAN:
    case Block::INVUL:
    case Block::EXTRA:
    case Block::NONE:
      break;
  }
  return score;
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

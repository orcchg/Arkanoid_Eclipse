#ifndef __ARKANOID_PRIZE__H__
#define __ARKANOID_PRIZE__H__

#include <random>

namespace game {

enum class Prize : int {
  NONE = 0,
  BLOCK = 1,      // produces BRICK block (0 cardinality)
  CLIMB = 2,      // [ t ] climbs up the bite
  DESTROY = 3,    // lose ball
  DRAGON = 4,     // [ t ] summons dragon
  EASY = 5,       // [ 1 ] for all ordinary / action blocks
  EASY_T = 6,     // [ t ] same
  EVAPORATE = 7,  // transform all WATER to FOG
  EXPLODE = 8,    // explodes blocks nearest to ball
  EXTEND = 9,     // [ t ] extends bite
  FAST = 10,      // [ t ] speed-up ball
  FOG = 11,       // produces FOG blocks instead NONE (0 cardinality)
  GOO = 12,       // [ t ] glues ball and bite
  HYPER = 13,     // teleports ball to random place
  INIT = 14,      // reloads current level
  JUMP = 15,      // XXX:
  LASER = 16,     // [ t ] laser beam from bite
  MIRROR = 17,    // [ t ] all collisions from bite are mirrored
  PIERCE = 18,    // [ t ] ball not disturbing
  PROTECT = 19,   // [ t ] add protection blocks
  RANDOM = 20,    // [ t ] random angle for any bite collision
  SHORT = 21,     // [ t ] shortens bite
  SLOW = 22,      // [ t ] slow down ball
  UPGRADE = 23,   // improved all ordinary blocks (+ cardinality)
  DEGRADE = 24,   // decreases all ordinary blocks (- cardinality)
  VITALITY = 25,  // additional live
  WIN = 26,       // win level
  ZYGOTE = 27     // multiplies ball
};

class PrizeUtils {
public:
  constexpr static int totalPrizes = 28;
  constexpr static double prizeProbability = 0.415;
};

class PrizeGenerator {
public:
  PrizeGenerator();
  Prize generatePrize();  //!< Generates random prize of any type

private:
  std::default_random_engine m_generator;
  std::uniform_int_distribution<int> m_distribution;
  std::bernoulli_distribution m_success_distribution;
};

}

#endif  // __ARKANOID_PRIZE__H__

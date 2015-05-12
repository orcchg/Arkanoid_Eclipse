#include <chrono>
#include "Prize.h"

namespace game {

PrizeGenerator::PrizeGenerator()
  : m_generator(std::chrono::system_clock::now().time_since_epoch().count())
  , m_distribution(0, PrizeUtils::totalPrizes - 1)
  , m_success_distribution(PrizeUtils::prizeProbability)
  , m_win_distribution(PrizeUtils::winProbability) {
}

Prize PrizeGenerator::generatePrize() {
  int value = m_success_distribution(m_generator) ? m_distribution(m_generator) : 0;
  if (m_win_distribution(m_generator)) {
    return Prize::WIN;
  }
  return Prize::EXPLODE;
  return static_cast<Prize>(value);
}

}

#include "Prize.h"

namespace game {

PrizeGenerator::PrizeGenerator()
  : m_generator()
  , m_distribution(0, PrizeUtils::totalPrizes - 1)
  , m_success_distribution(PrizeUtils::prizeProbability) {
}

Prize PrizeGenerator::generatePrize() {
  int value = m_success_distribution(m_generator) ? m_distribution(m_generator) : 0;
  return static_cast<Prize>(value);
}

}

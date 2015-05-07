#include "logger.h"
#include "Params.h"
#include "PrizeProcessor.h"

namespace game {

PrizeProcessor::PrizeProcessor()
  : m_bite()
  , m_bite_upper_border(-BiteParams::neg_biteElevation) {
  DBG("enter PrizeProcessor ctor");
  m_init_bite_received.store(false);
  m_bite_location_received.store(false);
  m_prize_received.store(false);
  m_prize_location_received.store(false);
  m_prize_gone_received.store(false);
  DBG("exit PrizeProcessor ctor");
}

PrizeProcessor::~PrizeProcessor() {
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void PrizeProcessor::callback_initBite(Bite bite) {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_init_bite_received.store(true);
  m_bite = bite;
  interrupt();
}

void PrizeProcessor::callback_biteMoved(Bite moved_bite) {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  m_bite_location_received.store(true);
  m_bite = moved_bite;
  interrupt();
}

void PrizeProcessor::callback_prizeReceived(PrizePackage package) {
  std::unique_lock<std::mutex> lock(m_prize_mutex);
  m_prize_received.store(true);
  m_prize_packages[package.getID()] = package;
  interrupt();
}

void PrizeProcessor::callback_prizeLocated(PrizePackage package) {
  std::unique_lock<std::mutex> lock(m_prize_location_mutex);
  m_prize_location_received.store(true);
  // XXX:
  interrupt();
}

void PrizeProcessor::callback_prizeHasGone(PrizePackage package) {
  std::unique_lock<std::mutex> lock(m_prize_gone_mutex);
  m_prize_gone_received.store(true);
  // XXX:
  interrupt();
}

/* ActiveObject group */
// ----------------------------------------------------------------------------
void PrizeProcessor::onStart() {
}

void PrizeProcessor::onStop() {
}

bool PrizeProcessor::checkForWakeUp() {
  return m_init_bite_received.load() ||
         m_bite_location_received.load() ||
         m_prize_received.load() ||
         m_prize_location_received.load() ||
         m_prize_gone_received.load();
}

void PrizeProcessor::eventHandler() {
  if (m_init_bite_received.load()) {
    m_init_bite_received.store(false);
    process_initBite();
  }
  if (m_bite_location_received.load()) {
    m_bite_location_received.store(false);
    process_biteMoved();
  }
  if (m_prize_received.load()) {
    m_prize_received.store(false);
    process_prizeReceived();
  }
  if (m_prize_location_received.load()) {
    m_prize_location_received.store(false);
    process_prizeLocated();
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void PrizeProcessor::process_initBite() {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_bite_upper_border = -BiteParams::neg_biteElevation;
}

void PrizeProcessor::process_biteMoved() {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  // no-op
}

void PrizeProcessor::process_prizeReceived() {
  std::unique_lock<std::mutex> lock(m_prize_mutex);
  // no-op
}

void PrizeProcessor::process_prizeLocated() {
  std::unique_lock<std::mutex> lock(m_prize_location_mutex);
  // XXX:
}

void PrizeProcessor::process_prizeHasGone() {
  std::unique_lock<std::mutex> lock(m_prize_gone_mutex);
  // XXX:
}

/* LogicFunc group */
// ----------------------------------------------------------------------------

}

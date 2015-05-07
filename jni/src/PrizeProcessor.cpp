#include "logger.h"
#include "PrizeProcessor.h"

namespace game {

PrizeProcessor::PrizeProcessor()
  : m_bite() {
  DBG("enter PrizeProcessor ctor");
  m_bite_location_received.store(false);
  DBG("exit PrizeProcessor ctor");
}

PrizeProcessor::~PrizeProcessor() {
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void PrizeProcessor::callback_biteMoved(Bite moved_bite) {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  m_bite_location_received.store(true);
  m_bite = moved_bite;
  interrupt();
}

/* ActiveObject group */
// ----------------------------------------------------------------------------
void PrizeProcessor::onStart() {
}

void PrizeProcessor::onStop() {
}

bool PrizeProcessor::checkForWakeUp() {
  return m_bite_location_received.load();
}

void PrizeProcessor::eventHandler() {
  if (m_bite_location_received.load()) {
    m_bite_location_received.store(false);
    process_biteMoved();
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void PrizeProcessor::process_biteMoved() {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  // no-op
}

/* LogicFunc group */
// ----------------------------------------------------------------------------

}

#include "SoundProcessor.h"

namespace native {
namespace sound {

SoundProcessor::SoundProcessor() {
}

SoundProcessor::~SoundProcessor() {
}

/* Callbacks group */
// ----------------------------------------------------------------------------

/* ActiveObject group */
// ----------------------------------------------------------------------------
void SoundProcessor::onStart() {
}

void SoundProcessor::onStop() {
}

bool SoundProcessor::checkForWakeUp() {
  return true;  // TODO: implement
}

void SoundProcessor::eventHandler() {
  // TODO: implement
}

/* Processors group */
// ----------------------------------------------------------------------------

/* CoreFunc group */
// ----------------------------------------------------------------------------

}
}

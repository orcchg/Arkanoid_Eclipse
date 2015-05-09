#include <sstream>

#include "Exceptions.h"
#include "logger.h"
#include "SoundProcessor.h"

namespace native {
namespace sound {

SoundProcessor::SoundProcessor()
  : m_error_code(0)
  , m_engine(nullptr)
  , m_mixer(nullptr)
  , m_interface(nullptr) {
  if (!init()) {
    std::ostringstream oss;
    oss << "Sound processor exception, error code: " << m_error_code;
    throw SoundProcessorException(oss.str().c_str());
  }
}

SoundProcessor::~SoundProcessor() {
  destroy();
  m_resources = nullptr;
}

/* Callbacks group */
// ----------------------------------------------------------------------------

// ----------------------------------------------
void SoundProcessor::setResourcesPtr(game::Resources* resources) {
  m_resources = resources;
}

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
bool SoundProcessor::init() {
  const SLInterfaceID ids[1] {SL_IID_ENGINE};
  const SLboolean required[1] {SL_BOOLEAN_TRUE};
  const SLInterfaceID mix_ids[] {};
  const SLboolean mix_required[] {};
  int error_code = 0;

  SLresult result = slCreateEngine(&m_engine, 0, nullptr, 1, ids, required);
  if (result != SL_RESULT_SUCCESS) { error_code = 1; goto ERROR_SOUND; }
  result = (*m_engine)->Realize(m_engine, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS) { error_code = 2; goto ERROR_SOUND; }
  result = (*m_engine)->GetInterface(m_engine, SL_IID_ENGINE, &m_interface);
  if (result != SL_RESULT_SUCCESS) { error_code = 3; goto ERROR_SOUND; }
  result = (*m_interface)->CreateOutputMix(m_interface, &m_mixer, 0, mix_ids, mix_required);
  if (result != SL_RESULT_SUCCESS) { error_code = 4; goto ERROR_SOUND; }
  result = (*m_mixer)->Realize(m_mixer, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS) { error_code = 5; goto ERROR_SOUND; }
  return true;

  ERROR_SOUND:
    m_error_code = 2000 + error_code;
    ERR("Error while initializing sound processor: %i", m_error_code);
    destroy();
    return false;
}

void SoundProcessor::destroy() {
  if (m_mixer != nullptr) {
    (*m_mixer)->Destroy(m_mixer);
    m_mixer = nullptr;
  }
  if (m_engine != nullptr) {
    (*m_engine)->Destroy(m_engine);
    m_engine = nullptr;
  }
}

}
}

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
  , m_interface(nullptr)
  , m_player(nullptr)
  , m_player_interface(nullptr)
  , m_player_queue(nullptr) {

  DBG("enter SoundProcessor ctor");
  if (!init()) {
    std::ostringstream oss;
    oss << "Sound processor exception, error code: " << m_error_code;
    throw SoundProcessorException(oss.str().c_str());
  }

  m_load_resources_received.store(false);
  DBG("exit SoundProcessor ctor");
}

SoundProcessor::~SoundProcessor() {
  DBG("enter SoundProcessor ~dtor");
  destroy();
  m_resources = nullptr;
  DBG("exit SoundProcessor ~dtor");
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void SoundProcessor::callback_loadResources(bool /* dummy */) {
  std::unique_lock<std::mutex> lock(m_load_resources_mutex);
  m_load_resources_received.store(true);
  interrupt();
}

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
  return m_load_resources_received.load();
}

void SoundProcessor::eventHandler() {
  if (m_load_resources_received.load()) {
    m_load_resources_received.store(false);
    process_loadResources();
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void SoundProcessor::process_loadResources() {
  std::unique_lock<std::mutex> lock(m_load_resources_mutex);
  if (m_resources != nullptr) {
    for (auto it = m_resources->beginSound(); it != m_resources->endSound(); ++it) {
      DBG("Loading resources: %s %p", it->first.c_str(), it->second);
      it->second->load();
    }
  } else {
    ERR("Resources pointer was not set !");
  }
}

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

bool SoundProcessor::initPlayerQueue() {
  SLDataLocator_AndroidSimpleBufferQueue data_locator_in {
    SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
    1  // no more than one sound buffer in queue at any moment
  };

  SLDataFormat_PCM data_format {
    SL_DATAFORMAT_PCM,
    1 /* mono sound */,
    SL_SAMPLINGRATE_44_1,
    SL_PCMSAMPLEFORMAT_FIXED_16,
    SL_PCMSAMPLEFORMAT_FIXED_16,
    SL_SPEAKER_FRONT_CENTER,
    SL_BYTEORDER_LITTLEENDIAN
  };

  SLDataSource data_source {
    &data_locator_in,
    &data_format
  };

  SLDataLocator_OutputMix data_locator_out {
    SL_DATALOCATOR_OUTPUTMIX,
    m_mixer
  };

  SLDataSink data_sink {
    &data_locator_out,
    nullptr
  };

  const SLInterfaceID player_ids[2] { SL_IID_PLAY, SL_IID_BUFFERQUEUE };
  const SLboolean player_required[2] { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

  int error_code = 0;
  SLresult result = (*m_interface)->CreateAudioPlayer(m_interface, &m_player, &data_source, &data_sink, 2, player_ids, player_required);
  if (result != SL_RESULT_SUCCESS) { error_code = 6; goto ERROR_PLAYER; }
  result = (*m_player)->Realize(m_player, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS) { error_code = 7; goto ERROR_PLAYER; }
  result = (*m_player)->GetInterface(m_player, SL_IID_PLAY, &m_player_interface);
  if (result != SL_RESULT_SUCCESS) { error_code = 8; goto ERROR_PLAYER; }
  result = (*m_player)->GetInterface(m_player, SL_IID_BUFFERQUEUE, &m_player_queue);
  if (result != SL_RESULT_SUCCESS) { error_code = 9; goto ERROR_PLAYER; }
  result = (*m_player_interface)->SetPlayState(m_player_interface, SL_PLAYSTATE_PLAYING);
  if (result != SL_RESULT_SUCCESS) { error_code = 10; goto ERROR_PLAYER; }
  return true;

  ERROR_PLAYER:
    m_error_code = 2000 + error_code;
    ERR("Error while initializing sound buffers queue: %i", m_error_code);
    return false;
}

bool SoundProcessor::playSound(SoundBuffer* sound) {
  int error_code = 0;
  SLuint32 player_state = 0;
  (*m_player)->GetState(m_player, &player_state);

  if (player_state == SL_OBJECT_STATE_REALIZED) {
    int16_t* buffer = reinterpret_cast<int16_t*>(sound->getData());
    off_t length = sound->getLength();
    // stop all sounds in queue before playing a new one
    SLresult result = (*m_player_queue)->Clear(m_player_queue);
    if (result != SL_RESULT_SUCCESS) { error_code = 11; goto ERROR_PLAY; }
    result = (*m_player_queue)->Enqueue(m_player_queue, buffer, length);
    if (result != SL_RESULT_SUCCESS) { error_code = 12; goto ERROR_PLAY; }
  }
  return true;

  ERROR_PLAY:
    m_error_code = 2000 + error_code;
    ERR("Error while playing sound %s, code: %i", sound->getName(), m_error_code);
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
  if (m_player != nullptr) {
    (*m_player)->Destroy(m_player);
    m_player = nullptr;
    m_player_interface = nullptr;
    m_player_queue = nullptr;
  }
}

}
}

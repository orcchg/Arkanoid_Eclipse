#include "Exceptions.h"
#include "GameProcessor.h"

namespace game {

/* Public API */
// ----------------------------------------------
GameProcessor::GameProcessor(JavaVM* jvm)
  : m_jvm(jvm), m_jenv(nullptr) {
}

GameProcessor::~GameProcessor() {
  m_jvm = nullptr;  m_jenv = nullptr;
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void GameProcessor::callback_throwBall(bool /* dummy */) {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  m_throw_ball_received.store(true);
  interrupt();
}

/* *** Private methods *** */
/* JNIEnvironment group */
// ----------------------------------------------------------------------------
void GameProcessor::attachToJVM() {
  std::unique_lock<std::mutex> lock(m_jnienvironment_mutex);
  auto result = m_jvm->AttachCurrentThread(&m_jenv, nullptr /* thread args */);
  if (result != JNI_OK) {
    ERR("GameProcessor thread was not attached to JVM !");
    throw JNI_exception();
  }
}

void GameProcessor::detachFromJVM() {
  std::unique_lock<std::mutex> lock(m_jnienvironment_mutex);
  m_jvm->DetachCurrentThread();
}

/* ActiveObject group */
// ----------------------------------------------------------------------------
void GameProcessor::onStart() {
  attachToJVM();
}

void GameProcessor::onStop() {
  detachFromJVM();
}

bool GameProcessor::checkForWakeUp() {
  return m_throw_ball_received.load();
}

void GameProcessor::eventHandler() {
}

/* Processors group */
// ----------------------------------------------------------------------------
void GameProcessor::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  // XXX:
}

}

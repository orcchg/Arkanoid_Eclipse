#include <chrono>
#include <cmath>

#include "Exceptions.h"
#include "GameProcessor.h"

namespace game {

/* Public API */
// ----------------------------------------------
GameProcessor::GameProcessor(JavaVM* jvm)
  : m_jvm(jvm), m_jenv(nullptr)
  , m_ball_is_flying(false)
  , m_ball_location()
  , m_ball_angle(ballAngle)
  , m_ball_speed(ballSpeed) {

  m_throw_ball_received.store(false);
  m_init_ball_position_received.store(false);
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

void GameProcessor::callback_initBall(BallPosition init_position) {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  m_init_ball_position_received.store(true);
  m_ball_location = init_position;
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
  return m_ball_is_flying ||
      m_throw_ball_received.load() ||
      m_init_ball_position_received.load();
}

void GameProcessor::eventHandler() {
  if (m_throw_ball_received.load()) {
    m_throw_ball_received.store(false);
    process_throwBall();
  }
  if (m_init_ball_position_received.load()) {
    m_init_ball_position_received.store(false);
    process_initBall();
  }
  if (m_ball_is_flying) {
    moveBall();
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void GameProcessor::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  m_ball_is_flying = true;
}

void GameProcessor::process_initBall() {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  // restore ball's initial velocity
  m_ball_angle = ballAngle;
  m_ball_speed = ballSpeed;
}

/* LogicFunc group */
// ----------------------------------------------------------------------------
void GameProcessor::moveBall() {
  GLfloat new_x = m_ball_location.x + m_ball_speed * cos(m_ball_angle);
  GLfloat new_y = m_ball_location.y + m_ball_speed * sin(m_ball_angle);

  if (new_x >= 1.0f ||
      new_x <= -1.0f ||
      new_y >= 1.0f ||
      new_y <= -1.0f) {
    m_ball_angle += util::PI2;
  }

  new_x = m_ball_location.x + m_ball_speed * cos(m_ball_angle);
  new_y = m_ball_location.y + m_ball_speed * sin(m_ball_angle);
  m_ball_location.x = new_x;
  m_ball_location.y = new_y;

  move_ball_event.notifyListeners(m_ball_location);
  std::this_thread::sleep_for (std::chrono::milliseconds(50));
}

}

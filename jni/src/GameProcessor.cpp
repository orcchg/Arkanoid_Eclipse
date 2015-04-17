#include <chrono>
#include <cmath>

#include "Exceptions.h"
#include "GameProcessor.h"
#include "Params.h"

namespace game {

/* Public API */
// ----------------------------------------------
GameProcessor::GameProcessor(JavaVM* jvm)
  : m_jvm(jvm), m_jenv(nullptr)
  , m_ball_is_flying(false)
  , m_is_ball_lost(false)
  , m_ball()
  , m_bite()
  , m_bite_upper_border(-BiteParams::neg_biteElevation)
  , m_ball_angle(BallParams::ballAngle)
  , m_ball_speed(BallParams::ballSpeed)
  , m_level_lower_border(0.0f)
  , m_generator()
  , m_angle_distribution(util::PI4, util::PI16)
  , m_direction_distribution(0.5f) {

  m_throw_ball_received.store(false);
  m_init_ball_position_received.store(false);
  m_init_bite_received.store(false);
  m_level_lower_border_received.store(false);
  m_bite_location_received.store(false);
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

void GameProcessor::callback_initBall(Ball init_ball) {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  m_init_ball_position_received.store(true);
  m_ball = init_ball;
  interrupt();
}

void GameProcessor::callback_initBite(Bite bite) {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_init_bite_received.store(true);
  m_bite = bite;
  interrupt();
}

void GameProcessor::callback_loadLevel(float lower_border) {
  std::unique_lock<std::mutex> lock(m_level_lower_border_mutex);
  m_level_lower_border_received.store(true);
  m_level_lower_border = lower_border;
  interrupt();
}

void GameProcessor::callback_biteMoved(Bite moved_bite) {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  m_bite_location_received.store(true);
  m_bite = moved_bite;
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
      m_init_ball_position_received.load() ||
      m_init_bite_received.load() ||
      m_level_lower_border_received.load() ||
      m_bite_location_received.load();
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
  if (m_init_bite_received.load()) {
    m_init_bite_received.store(false);
    process_initBite();
  }
  if (m_level_lower_border_received.load()) {
    m_level_lower_border_received.store(false);
    process_loadLevel();
  }
  if (m_bite_location_received.load()) {
    m_bite_location_received.store(false);
    process_biteMoved();
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
  m_is_ball_lost = false;
}

void GameProcessor::process_initBall() {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  // restore ball's initial velocity
  m_ball_angle = m_angle_distribution(m_generator);  // BallParams::ballAngle;
  m_ball_angle += m_direction_distribution(m_generator) ? util::PI2 : 0.0f;
  m_ball_angle = std::fmod(m_ball_angle, util::_2PI);
  m_ball_speed = BallParams::ballSpeed;
}

void GameProcessor::process_initBite() {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_bite_upper_border = -BiteParams::neg_biteElevation + 2 * m_bite.dimens.height;
}

void GameProcessor::process_loadLevel() {
  std::unique_lock<std::mutex> lock(m_level_lower_border_mutex);
  // no-op
}

void GameProcessor::process_biteMoved() {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  // no-op
}

#include <cassert>
/* LogicFunc group */
// ----------------------------------------------------------------------------
void GameProcessor::moveBall() {
  // ball's position in the next frame
  GLfloat new_x = m_ball.pose.x + m_ball_speed * cos(m_ball_angle);
  GLfloat new_y = m_ball.pose.y + m_ball_speed * sin(m_ball_angle);

  if (m_is_ball_lost && new_y <= -1.0f) {
    lost_ball_event.notifyListeners(true);
    m_ball_is_flying = false;
    return;
  }

  collideLeftRightBorder(new_x);

  if (new_y < m_bite_upper_border) {
    if (!m_is_ball_lost) {
      m_is_ball_lost = !collideBite(new_x);
    }
  } else {
    if (new_y >= 1.0f - m_level_lower_border) {
      m_ball_angle = util::_2PI - m_ball_angle;
    }
    GLfloat sign = m_ball_angle >= 0.0f ? 1.0f : -1.0f;
    m_ball_angle = sign * std::fmod(std::fabs(m_ball_angle), util::_2PI);
  }

  new_x = m_ball.pose.x + m_ball_speed * cos(m_ball_angle);
  new_y = m_ball.pose.y + m_ball_speed * sin(m_ball_angle);
  m_ball.pose.x = new_x;
  m_ball.pose.y = new_y;

  move_ball_event.notifyListeners(m_ball);
  std::this_thread::sleep_for (std::chrono::milliseconds(50));
}

/* Maths group */
// ----------------------------------------------------------------------------
void GameProcessor::collideLeftRightBorder(GLfloat new_x) {
  if (new_x >= BallParams::neg_ballHalfSize) {
    if (m_ball_angle <= util::PI2) {
      m_ball_angle = util::PI - m_ball_angle;
    } else if (m_ball_angle >= util::_3PI2) {
      m_ball_angle = util::_3PI - m_ball_angle;
    }
  } else if (new_x <= -BallParams::neg_ballHalfSize) {
    if (m_ball_angle >= util::PI) {
      m_ball_angle = util::_3PI - m_ball_angle;
    } else if (m_ball_angle >= util::PI2) {
      m_ball_angle = util::PI - m_ball_angle;
    }
  }
  GLfloat sign = m_ball_angle >= 0.0f ? 1.0f : -1.0f;
  m_ball_angle = sign * std::fmod(std::fabs(m_ball_angle), util::_2PI);
}

// http://stackoverflow.com/questions/8063696/arkanoid-physics-projectile-physics-simulation
bool GameProcessor::collideBite(GLfloat new_x) {
  if (new_x >= -BiteParams::biteHalfWidth + m_bite.x_pose &&
      new_x <= BiteParams::biteHalfWidth + m_bite.x_pose) {
    if (m_ball_angle >= util::PI) {
      m_ball_angle = util::_2PI - m_ball_angle;
    }
    GLfloat sign = m_ball_angle >= 0.0f ? 1.0f : -1.0f;
    m_ball_angle = sign * std::fmod(std::fabs(m_ball_angle), util::_2PI);
  } else {
    return false;  // ball missed the bite
  }
  return true;
}

}

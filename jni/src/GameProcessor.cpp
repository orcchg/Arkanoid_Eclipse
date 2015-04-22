#include <cassert>
#include <chrono>
#include <cmath>

#include "Exceptions.h"
#include "GameProcessor.h"
#include "Params.h"

namespace game {

/* Public API */
// ----------------------------------------------
GameProcessor::GameProcessor(JavaVM* jvm, jobject master_object)
  : m_jvm(jvm), m_jenv(nullptr)
  , master_object(master_object)
  , fireJavaEvent_lostBall_id(nullptr)
  , fireJavaEvent_levelFinished_id(nullptr)
  , m_level(nullptr)
  , m_level_finished(false)
  , m_ball_is_flying(false)
  , m_is_ball_lost(false)
  , m_ball()
  , m_bite()
  , m_bite_upper_border(-BiteParams::neg_biteElevation)
  , m_level_dimens(0, 0, 0.0f, 0.0f)
  , m_generator()
  , m_angle_distribution(util::PI4, util::PI16)
  , m_direction_distribution(0.5f) {

  DBG("enter GameProcessor ctor");
  m_load_level_received.store(false);
  m_throw_ball_received.store(false);
  m_init_ball_position_received.store(false);
  m_init_bite_received.store(false);
  m_level_dimens_received.store(false);
  m_bite_location_received.store(false);
  DBG("exit GameProcessor ctor");
}

GameProcessor::~GameProcessor() {
  m_jvm = nullptr;  m_jenv = nullptr;
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void GameProcessor::callback_loadLevel(Level::Ptr level) {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  m_load_level_received.store(true);
  m_level = level;
  interrupt();
}

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

void GameProcessor::callback_levelDimens(LevelDimens level_dimens) {
  std::unique_lock<std::mutex> lock(m_level_dimens_mutex);
  m_level_dimens_received.store(true);
  m_level_dimens = level_dimens;
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
      m_load_level_received.load() ||
      m_throw_ball_received.load() ||
      m_init_ball_position_received.load() ||
      m_init_bite_received.load() ||
      m_level_dimens_received.load() ||
      m_bite_location_received.load();
}

void GameProcessor::eventHandler() {
  if (m_load_level_received.load()) {
    m_load_level_received.store(false);
    process_loadLevel();
  }
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
  if (m_level_dimens_received.load()) {
    m_level_dimens_received.store(false);
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
void GameProcessor::process_loadLevel() {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  // no-op
}

void GameProcessor::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  m_level_finished = false;
  m_ball_is_flying = true;
  m_is_ball_lost = false;
}

void GameProcessor::process_initBall() {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  // restore ball's initial velocity
  m_ball.angle = m_angle_distribution(m_generator);  // BallParams::ballAngle;
  m_ball.angle += m_direction_distribution(m_generator) ? util::PI2 : 0.0f;
  m_ball.angle = std::fmod(m_ball.angle, util::_2PI);
}

void GameProcessor::process_initBite() {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_bite_upper_border = -BiteParams::neg_biteElevation + 2 * m_bite.dimens.height;
}

void GameProcessor::process_levelDimens() {
  std::unique_lock<std::mutex> lock(m_level_dimens_mutex);
  // no-op
}

void GameProcessor::process_biteMoved() {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  // no-op
}

/* LogicFunc group */
// ----------------------------------------------------------------------------
void GameProcessor::moveBall() {
  if (m_level_finished) {
    m_ball_is_flying = false;  // stop flying before notify to avoid bugs
    level_finished_event.notifyListeners(true);
    onLevelFinished(true);
    return;
  }

  // ball's position in the next frame
  GLfloat new_x = m_ball.pose.x + m_ball.x_velocity * cos(m_ball.angle);
  GLfloat new_y = m_ball.pose.y + m_ball.y_velocity * sin(m_ball.angle);

  if (m_is_ball_lost && new_y <= -1.0f) {
    m_ball_is_flying = false;  // stop flying before notify to avoid bugs
    lost_ball_event.notifyListeners(true);
    onLostBall(true);
    return;
  }

  if (new_x >= BallParams::neg_ballHalfSize) {  // right border
    collideRightBorder();
  } else if (new_x <= -BallParams::neg_ballHalfSize) {  // left border
    collideLeftBorder();
  }

  if (new_y < m_bite_upper_border - BallParams::ballHalfSize) {
    if (!m_is_ball_lost) {
      m_is_ball_lost = !collideBite(new_x);
    }
  } else {
    if (collideBlocks(new_x, new_y)) {
      m_level_finished = (m_level->blockImpact() == 0);
    }
  }

  new_x = m_ball.pose.x + m_ball.x_velocity * cos(m_ball.angle);
  new_y = m_ball.pose.y + m_ball.y_velocity * sin(m_ball.angle);
  m_ball.pose.x = new_x;
  m_ball.pose.y = new_y;

  move_ball_event.notifyListeners(m_ball);
  std::this_thread::sleep_for (std::chrono::nanoseconds(1));
}

void GameProcessor::onLostBall(bool /* dummy */) {
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_lostBall_id);
}

void GameProcessor::onLevelFinished(bool /* dummy */) {
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_levelFinished_id);
}

/* Maths group */
// ----------------------------------------------------------------------------
void GameProcessor::collideLeftBorder() {
  if (m_ball.angle >= util::PI) {
    m_ball.angle = util::_3PI - m_ball.angle;
  } else if (m_ball.angle >= util::PI2) {
    m_ball.angle = util::PI - m_ball.angle;
  }
  GLfloat sign = m_ball.angle >= 0.0f ? 1.0f : -1.0f;
  m_ball.angle = sign * std::fmod(std::fabs(m_ball.angle), util::_2PI);
}

void GameProcessor::collideRightBorder() {
  if (m_ball.angle <= util::PI2) {
    m_ball.angle = util::PI - m_ball.angle;
  } else if (m_ball.angle >= util::_3PI2) {
    m_ball.angle = util::_3PI - m_ball.angle;
  }
  GLfloat sign = m_ball.angle >= 0.0f ? 1.0f : -1.0f;
  m_ball.angle = sign * std::fmod(std::fabs(m_ball.angle), util::_2PI);
}

void GameProcessor::collideHorizontalSurface() {
  m_ball.angle = util::_2PI - m_ball.angle;
  GLfloat sign = m_ball.angle >= 0.0f ? 1.0f : -1.0f;
  m_ball.angle = sign * std::fmod(std::fabs(m_ball.angle), util::_2PI);
}

// http://stackoverflow.com/questions/8063696/arkanoid-physics-projectile-physics-simulation
bool GameProcessor::collideBite(GLfloat new_x) {
  if (new_x >= -(BiteParams::biteHalfWidth + BallParams::ballHalfSize) + m_bite.x_pose &&
      new_x <= (BiteParams::biteHalfWidth + BallParams::ballHalfSize) + m_bite.x_pose) {
//    GLfloat mass_factor = m_bite.mass / m_ball.mass;
//    m_ball.x_velocity += BiteParams::neg_angularFactor * m_bite.x_velocity * mass_factor * m_bite.friction;
//    m_ball.y_velocity = -m_ball.y_velocity;
//    m_ball.spin += BiteParams::angularFactor * m_bite.x_velocity  * mass_factor * m_bite.friction;;
    if (m_ball.angle >= util::PI) {
      collideHorizontalSurface();
    }
  } else {
    return false;  // ball missed the bite
  }
  return true;
}

bool GameProcessor::collideBlocks(GLfloat new_x, GLfloat new_y) {
  if (new_y >= BallParams::neg_ballHalfSize - m_level_dimens.height && new_y < BallParams::neg_ballHalfSize) {
    size_t row = 0, col = 0;
    getImpactedBlock(new_x, new_y, &row, &col);

    GLfloat top_border = 0.0f, bottom_border = 0.0f, left_border = 0.0f, right_border = 0.0f;
    m_level_dimens.getBlockDimens(row, col, &top_border, &bottom_border, &left_border, &right_border);

    Block block = m_level->getBlock(row, col);
    switch (block) {
      case Block::NONE:
        // fly without disturbance
        return false;
      default:
        if (m_ball.pose.x + 1.0f >= left_border && m_ball.pose.x + 1.0f <= right_border) {
          collideHorizontalSurface();
        } else if (m_ball.pose.x + 1.0f < left_border) {
          collideRightBorder();
        } else if (m_ball.pose.x + 1.0f > right_border) {
          collideLeftBorder();
        }
        break;
    }
    block_impact_event.notifyListeners(std::make_pair(row, col));
    return true;

  } else if (new_y >= BallParams::neg_ballHalfSize) {
    collideHorizontalSurface();
  }
  return false;
}

void GameProcessor::getImpactedBlock(
    GLfloat ball_x,
    GLfloat ball_y,
    size_t* row,
    size_t* col) {

  *col = static_cast<size_t>(std::floor((ball_x + BallParams::neg_ballHalfSize) / m_level_dimens.block_width));
  *row = static_cast<size_t>(std::floor((BallParams::neg_ballHalfSize - ball_y) / m_level_dimens.block_height));
}

}

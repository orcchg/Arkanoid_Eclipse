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
  , m_throw_angle(60.0f)
  , m_aspect(1.0f)
  , m_level_finished(false)
  , m_ball_is_flying(false)
  , m_is_ball_lost(false)
  , m_is_ball_death(false)
  , m_ball_pose_corrected(false)
  , m_ball()
  , m_bite()
  , m_bite_upper_border(-BiteParams::neg_biteElevation)
  , m_level_dimens(0, 0, 0.0f, 0.0f, 0.0f, 0.0f)
  , m_total_lives(3)
  , m_generator()
  , m_angle_distribution(util::PI12, util::PI30)
  , m_direction_distribution(0.25f)
  , m_viscosity_distribution(101, 0.5) {

  DBG("enter GameProcessor ctor");
  m_aspect_ratio_received.store(false);
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
void GameProcessor::callback_aspectMeasured(float aspect) {
  std::unique_lock<std::mutex> lock(m_aspect_ratio_mutex);
  m_aspect_ratio_received.store(true);
  m_aspect = aspect;
  interrupt();
}

void GameProcessor::callback_loadLevel(Level::Ptr level) {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  m_load_level_received.store(true);
  m_level = level;
  INF("New level loaded, initial cardinality: %i", m_level->getCardinality());
  interrupt();
}

void GameProcessor::callback_throwBall(float angle) {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  m_throw_ball_received.store(true);
  m_throw_angle = angle;
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
      m_aspect_ratio_received.load() ||
      m_load_level_received.load() ||
      m_throw_ball_received.load() ||
      m_init_ball_position_received.load() ||
      m_init_bite_received.load() ||
      m_level_dimens_received.load() ||
      m_bite_location_received.load();
}

void GameProcessor::eventHandler() {
  if (m_aspect_ratio_received.load()) {
    m_aspect_ratio_received.store(false);
    process_aspectMeasured();
  }
  if (m_load_level_received.load()) {
    m_load_level_received.store(false);
    process_loadLevel();
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
  if (m_throw_ball_received.load()) {
    m_throw_ball_received.store(false);
    process_throwBall();
  }
  if (m_ball_is_flying) {
    moveBall();
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void GameProcessor::process_aspectMeasured() {
  std::unique_lock<std::mutex> lock(m_aspect_ratio_mutex);
  // no-op
}

void GameProcessor::process_loadLevel() {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  // no-op
}

void GameProcessor::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  if (!m_ball_is_flying) {
    m_ball.angle = m_throw_angle;
    m_level_finished = false;
    m_ball_is_flying = true;
    m_is_ball_lost = false;
    m_is_ball_death = false;
    m_ball_pose_corrected = false;
  }
  INF("Ball has been thrown");
}

void GameProcessor::process_initBall() {
  std::unique_lock<std::mutex> lock(m_init_ball_position_mutex);
  // no-op
}

void GameProcessor::process_initBite() {
  std::unique_lock<std::mutex> lock(m_init_bite_mutex);
  m_bite_upper_border = -BiteParams::neg_biteElevation;
}

void GameProcessor::process_levelDimens() {
  std::unique_lock<std::mutex> lock(m_level_dimens_mutex);
  // no-op
}

void GameProcessor::process_biteMoved() {
  std::unique_lock<std::mutex> lock(m_bite_location_mutex);
  if (!m_ball_is_flying) {  // move ball following the bite
    shiftBall(m_bite.x_pose, m_ball.pose.y /* unchanged */);
  }
}

/* LogicFunc group */
// ----------------------------------------------------------------------------
void GameProcessor::moveBall() {
  m_ball_pose_corrected = false;

  if (m_level_finished) {
    m_ball_is_flying = false;  // stop flying before notify to avoid bugs
    level_finished_event.notifyListeners(true);
    onLevelFinished(true);
    return;
  }

  // ball's position in the next frame
  GLfloat old_x = m_ball.pose.x;
  GLfloat old_y = m_ball.pose.y;
  GLfloat new_x = m_ball.pose.x + m_ball.velocity * cos(m_ball.angle);
  GLfloat new_y = m_ball.pose.y + m_ball.velocity * sin(m_ball.angle);

  if ((m_is_ball_lost && new_y <= -1.0f) || m_is_ball_death) {
    m_ball_is_flying = false;  // stop flying before notify to avoid bugs
    lost_ball_event.notifyListeners(true);
    onLostBall(true);
    return;
  }

  if (new_x >= 1.0f - m_ball.dimens.halfWidth()) {  // right border
    collideRightBorder();
    correctBallPosition(1.0f - m_ball.dimens.halfWidth(), new_y);
  } else if (new_x <= -1.0f + m_ball.dimens.halfWidth()) {  // left border
    collideLeftBorder();
    correctBallPosition(-1.0f + m_ball.dimens.halfWidth(), new_y);
  }

  if (new_y <= m_bite_upper_border + m_ball.dimens.halfHeight()) {
    if (!m_is_ball_lost) {
      m_is_ball_lost = !collideBite(new_x);
      correctBallPosition(new_x, m_bite_upper_border + m_ball.dimens.halfHeight());
    }
  } else if (collideBlock(new_x, new_y)) {
    m_level_finished = (m_level->blockImpact() == 0);
  }

  if (!m_ball_pose_corrected) {
    new_x = old_x + m_ball.velocity * cos(m_ball.angle);
    new_y = old_y + m_ball.velocity * sin(m_ball.angle);
    shiftBall(new_x, new_y);
  }
  std::this_thread::sleep_for (std::chrono::milliseconds(ProcessorParams::milliDelay));
}

void GameProcessor::shiftBall(GLfloat new_x, GLfloat new_y) {
  m_ball.pose.x = new_x;
  m_ball.pose.y = new_y;
  move_ball_event.notifyListeners(m_ball);
}

void GameProcessor::onLostBall(bool /* dummy */) {
  m_is_ball_lost = false;
  m_is_ball_death = false;
  --m_total_lives;
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_lostBall_id, m_total_lives);
}

void GameProcessor::onLevelFinished(bool /* dummy */) {
  m_level_finished = false;
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_levelFinished_id);
}

/* Collision group */
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

bool GameProcessor::collideCorner(
    GLfloat new_x,
    GLfloat new_y,
    GLfloat top_border,
    GLfloat bottom_border,
    GLfloat left_border,
    GLfloat right_border) {

//  if (((new_x + 1.0f <= left_border && new_x + 1.0f >= left_border - BallParams::ballHalfSize) ||
//      (new_x + 1.0f >= right_border && new_x + 1.0f <= right_border + BallParams::ballHalfSize)) &&
//      (m_ball.pose.y + 1.0f >= 2.0f - top_border || m_ball.pose.y + 1.0f <= 2.0f - bottom_border)) {
//    collideHorizontalSurface();
//    return true;
//  }
//  if (m_ball.pose.x + 1.0f <= left_border &&
//      ((new_y + 1.0f >= 2.0f - top_border && new_y + 1.0f <= 2.0f - top_border + BallParams::ballHalfSize) ||
//       (new_y + 1.0f <= 2.0f - bottom_border && new_y + 1.0f >= 2.0f - top_border - BallParams::ballHalfSize))) {
//    collideRightBorder();
//    return true;
//  }
//  if (m_ball.pose.x + 1.0f >= right_border &&
//      ((new_y + 1.0f >= 2.0f - top_border && new_y + 1.0f <= 2.0f - top_border + BallParams::ballHalfSize) ||
//       (new_y + 1.0f <= 2.0f - bottom_border && new_y + 1.0f >= 2.0f - top_border - BallParams::ballHalfSize))) {
//    collideLeftBorder();
//    return true;
//  }
  return false;
}

bool GameProcessor::collideBite(GLfloat new_x) {
  if (new_x >= -(m_bite.dimens.halfWidth() + m_ball.dimens.halfWidth()) + m_bite.x_pose &&
      new_x <= (m_bite.dimens.halfWidth() + m_ball.dimens.halfWidth()) + m_bite.x_pose) {

    GLfloat distance = std::fabs(new_x - m_bite.x_pose);
    GLfloat beta = std::fabs(std::atan(distance / m_bite.radius));

    if (new_x >= m_bite.x_pose + m_bite.dimens.quarterWidth()) {
      GLfloat normal = std::fabs(util::PI2 - beta);
      if (m_ball.angle >= util::_3PI2) {
        collideHorizontalSurface();
      } else if (m_ball.angle >= util::PI) {
        GLfloat gamma = util::_3PI2 - m_ball.angle;
        if (gamma <= beta) {
          GLfloat delta = std::fabs(gamma - 2 * beta + util::PI2);
          m_ball.angle = delta;
          smallAngleAvoid();
        } else {
          gamma = m_ball.angle - util::PI;
          GLfloat delta = std::fabs(gamma + 2 * beta - util::PI2);
          m_ball.angle = util::PI2 - delta;
          smallAngleAvoid();
        }
      }
    } else if (new_x <= m_bite.x_pose - m_bite.dimens.quarterWidth()) {
      GLfloat normal = std::fabs(util::PI2 + beta);
      if (m_ball.angle >= util::_3PI2) {
        GLfloat gamma = m_ball.angle - util::_3PI2;
        if (gamma <= beta) {
          GLfloat delta = std::fabs(gamma - 2 * beta + util::PI2);
          m_ball.angle = util::PI - delta;
          smallAngleAvoid();
        } else {
          gamma = util::_2PI - m_ball.angle;
          GLfloat delta = std::fabs(gamma + 2 * beta - util::PI2);
          m_ball.angle = util::PI2 + delta;
          smallAngleAvoid();
        }
      } else if (m_ball.angle >= util::PI) {
        collideHorizontalSurface();
      }
    } else {
      collideHorizontalSurface();
    }
    m_ball.angle = std::fmod(std::fabs(m_ball.angle), util::_2PI);
  } else {
    return false;  // ball missed the bite
  }
  return true;
}

bool GameProcessor::collideBlock(GLfloat new_x, GLfloat new_y) {
  if (new_y >= 1.0f - m_ball.dimens.halfHeight() - m_level_dimens.height &&
      new_y < 1.0f - m_ball.dimens.halfHeight()) {
    int row = 0, col = 0;
    if (!getImpactedBlock(new_x, new_y, &row, &col)) {
      return false;  // ball has left level boundaries
    }

    GLfloat top_border = 0.0f, bottom_border = 0.0f, left_border = 0.0f, right_border = 0.0f;
    m_level_dimens.getBlockDimens(row, col, &top_border, &bottom_border, &left_border, &right_border);

    Direction vertical_direction = Direction::NONE;
    Direction horizontal_direction = Direction::NONE;
    if (m_ball.pose.x + 1.0f > left_border &&
        m_ball.pose.x + 1.0f < right_border) {
      if (m_ball.pose.y >= 1.0f - top_border) {
        vertical_direction = Direction::DOWN;
      } else if (m_ball.pose.y <= 1.0f - bottom_border) {
        vertical_direction = Direction::UP;
      }
    }
    if (m_ball.pose.x + 1.0f <= left_border) {
      horizontal_direction = Direction::RIGHT;
    } else if (m_ball.pose.x + 1.0f >= right_border) {
      horizontal_direction = Direction::LEFT;
    }
    std::vector<RowCol> affected_blocks;
    affected_blocks.reserve(12);

    int viscosity = 0;
    Block block = m_level->getBlock(row, col);
    m_level->setBlockImpacted(row, col);
    switch (block) {
      case Block::NONE:
        // no impact and disturbance
        return false;
      // --------------------
      case Block::DEATH:
        m_is_ball_death = true;
        break;
      case Block::ULTRA_1:
        m_level->forceDropCardinality();
        break;
      // --------------------
      case Block::FOG:
      case Block::GLASS:
        // fly without disturbance
        break;
      // --------------------
      case Block::ELECTRO:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->destroyBlocksAround(row, col, &affected_blocks);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::KNOCK_VERTICAL:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->destroyBlocksBehind(row, col, vertical_direction, &affected_blocks);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::KNOCK_HORIZONTAL:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->destroyBlocksBehind(row, col, horizontal_direction, &affected_blocks);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::NETWORK_1:
        // XXX:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        break;
      // --------------------
      case Block::HYPER:
        // XXX: correctBallPosition();
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        break;
      case Block::ORIGIN:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_ball_is_flying = false;
        correctBallPosition(m_bite.x_pose, m_bite_upper_border + m_ball.dimens.halfHeight());
        break;
      // --------------------
      case Block::ROLLING:
        viscosity = m_viscosity_distribution(m_generator);
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, viscosity);
        break;
      // --------------------
      case Block::JELLY:
        viscosity += 35;
        // intend no break
      case Block::WATER:
        viscosity += 20;
        // intend no break
      case Block::YOGURT:
        viscosity += 10;
        // intend no break;
      case Block::CLAY:
        viscosity += 10;
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, viscosity);
        break;
      // --------------------
      case Block::MAGIC:
        // XXX:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        break;
      case Block::QUICK_1:
        // XXX:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        break;
      case Block::ZYGOTE_1:
        // XXX:
        // intend no break;
      // --------------------
      default:
        collideBlock(new_x, new_y, top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        break;
    }
    block_impact_event.notifyListeners(RowCol(row, col));
    return (block != Block::NONE &&
            block != Block::TITAN &&
            block != Block::INVUL &&
            block != Block::EXTRA);

  } else if (new_y >= 1.0f - m_ball.dimens.halfHeight()) {
    collideHorizontalSurface();
    correctBallPosition(new_x, 1.0f - m_ball.dimens.halfHeight());
  }
  return false;
}

bool GameProcessor::elasticBlockCollision(
    GLfloat top_border,
    GLfloat bottom_border,
    GLfloat left_border,
    GLfloat right_border) {

  if (m_ball.pose.x + 1.0f > left_border &&
      m_ball.pose.x + 1.0f < right_border &&
      (m_ball.pose.y + 1.0f >= 2.0f - top_border ||
       m_ball.pose.y + 1.0f <= 2.0f - bottom_border)) {
    collideHorizontalSurface();
    return true;
  }
  if (m_ball.pose.x + 1.0f <= left_border - m_ball.dimens.halfWidth()) {
    collideRightBorder();
    return true;
  }
  if (m_ball.pose.x + 1.0f >= right_border + m_ball.dimens.halfWidth()) {
    collideLeftBorder();
    return true;
  }
  return false;
}

bool GameProcessor::viscousBlockCollision(
    GLfloat top_border,
    GLfloat bottom_border,
    GLfloat left_border,
    GLfloat right_border,
    int viscosity) {

  if (viscosity == 100) {
    elasticBlockCollision(top_border, bottom_border, left_border, right_border);
    return true;
  } else if (viscosity == 0) {
    return true;
  }

  if (m_ball.pose.x + 1.0f > left_border &&
      m_ball.pose.x + 1.0f < right_border &&
      (m_ball.pose.y + 1.0f >= 2.0f - top_border ||
       m_ball.pose.y + 1.0f <= 2.0f - bottom_border)) {
    collideHorizontalSurface();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  if (m_ball.pose.x + 1.0f <= left_border) {
    collideRightBorder();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  if (m_ball.pose.x + 1.0f >= right_border) {
    collideLeftBorder();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  return false;
}

void GameProcessor::collideBlock(
    GLfloat new_x,
    GLfloat new_y,
    GLfloat top_border,
    GLfloat bottom_border,
    GLfloat left_border,
    GLfloat right_border,
    int viscosity) {

  if (collideCorner(new_x, new_y, top_border, bottom_border, left_border, right_border)) {
    return;
  }
  viscousBlockCollision(top_border, bottom_border, left_border, right_border, viscosity);
}

/* Maths group */
// ----------------------------------------------------------------------------
bool GameProcessor::getImpactedBlock(
    GLfloat ball_x,
    GLfloat ball_y,
    int* row,
    int* col) {

  if (m_ball.pose.x >= ball_x) {  // from right
    *col = static_cast<int>(std::floor((ball_x - m_ball.dimens.halfWidth() + 1.0f) / m_level_dimens.block_width));
  } else {  // from left
    *col = static_cast<int>(std::floor((ball_x + m_ball.dimens.halfWidth() + 1.0f) / m_level_dimens.block_width));
  }

  if (m_ball.pose.y >= ball_y) {  // from top
    *row = static_cast<int>(std::floor((1.0f + m_ball.dimens.halfHeight() - ball_y) / m_level_dimens.block_height));
  } else {  // from bottom
    *row = static_cast<int>(std::floor((1.0f - m_ball.dimens.halfHeight() - ball_y) / m_level_dimens.block_height));
  }

  if (*row < 0 || *row >= m_level->numRows() ||
      *col < 0 || *col >= m_level->numCols()) {
    return false;
  }
  return true;
}

void GameProcessor::correctBallPosition(GLfloat new_x, GLfloat new_y) {
  shiftBall(new_x, new_y);
  m_ball_pose_corrected = true;
}

void GameProcessor::smallAngleAvoid() {
  if (m_ball.angle <= util::PI16) {
    m_ball.angle += util::PI16;
  } else if (m_ball.angle >= util::_2PI - util::PI16) {
    m_ball.angle -= util::PI16;
  } else if (m_ball.angle >= util::PI - util::PI16 && m_ball.angle <= util::PI) {
    m_ball.angle -= util::PI16;
  } else if (m_ball.angle > util::PI && m_ball.angle <= util::PI + util::PI16) {
    m_ball.angle += util::PI16;
  } else if (m_ball.angle <= util::PI2 + util::PI16 && m_ball.angle >= util::PI2) {
    m_ball.angle += util::PI16;
  } else if (m_ball.angle >= util::PI2 - util::PI16 && m_ball.angle < util::PI2) {
    m_ball.angle -= util::PI16;
  } else if (m_ball.angle >= util::_3PI2 && m_ball.angle <= util::_3PI2 + util::PI16) {
    m_ball.angle += util::PI16;
  } else if (m_ball.angle < util::_3PI2 && m_ball.angle >= util::_3PI2 - util::PI16) {
    m_ball.angle -= util::PI16;
  }
}

void GameProcessor::randomAngle() {
  std::normal_distribution<float> init_angle_distribution(util::PI4, util::PI12);
  m_ball.angle = init_angle_distribution(m_generator);
  m_ball.angle += m_direction_distribution(m_generator) ? 0.0f : util::PI2;
  m_ball.angle = std::fmod(m_ball.angle, util::_2PI);
}

void GameProcessor::viscousAngleDisturbance(float viscosity) {
  GLfloat direction = m_direction_distribution(m_generator) ? 1.0f : -1.0f;
  m_ball.angle += direction * m_angle_distribution(m_generator) / 100.0f * viscosity;
  GLfloat sign = m_ball.angle >= 0.0f ? 1.0f : -1.0f;
  m_ball.angle = sign * std::fmod(std::fabs(m_ball.angle), util::_2PI);
}

}

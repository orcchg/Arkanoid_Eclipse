#include <cassert>
#include <chrono>
#include <cmath>

#include "Exceptions.h"
#include "GameProcessor.h"
#include "Params.h"
#include "Prize.h"

namespace game {

/* Public API */
// ----------------------------------------------
GameProcessor::GameProcessor(JavaVM* jvm)
  : m_jvm(jvm), m_jenv(nullptr)
  , master_object(nullptr)
  , fireJavaEvent_lostBall_id(nullptr)
  , fireJavaEvent_levelFinished_id(nullptr)
  , fireJavaEvent_cardinalityChanged_id(nullptr)
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
  , prizeID(0)
  , m_generator(std::chrono::system_clock::now().time_since_epoch().count())
  , m_angle_distribution(util::PI12, util::PI30)
  , m_direction_distribution(0.25f)
  , m_viscosity_distribution(0, 100) {

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
  onCardinalityChanged(m_level->getCardinality());
}

void GameProcessor::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  if (!m_ball_is_flying) {
    m_ball.setAngle(m_throw_angle);
    m_level_finished = false;
    m_ball_is_flying = true;
    m_is_ball_lost = false;
    m_is_ball_death = false;
    m_ball_pose_corrected = false;
    onAngleChanged();
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
    shiftBall(m_bite.getXPose(), m_ball.getPose().getY() /* unchanged */);
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
  GLfloat old_x = m_ball.getPose().getX();
  GLfloat old_y = m_ball.getPose().getY();
  GLfloat new_x = m_ball.getPose().getX() + m_ball.getVelocity() * cos(m_ball.getAngle());
  GLfloat new_y = m_ball.getPose().getY() + m_ball.getVelocity() * sin(m_ball.getAngle());

  if ((m_is_ball_lost && new_y <= -1.0f) || m_is_ball_death) {
    m_ball_is_flying = false;  // stop flying before notify to avoid bugs
    lost_ball_event.notifyListeners(true);
    onLostBall(true);
    onCardinalityChanged(m_level->getCardinality());
    return;
  }

  if (new_x >= 1.0f - m_ball.getDimens().halfWidth()) {  // right border
    collideRightBorder();
    correctBallPosition(1.0f - m_ball.getDimens().halfWidth(), new_y);
  } else if (new_x <= -1.0f + m_ball.getDimens().halfWidth()) {  // left border
    collideLeftBorder();
    correctBallPosition(-1.0f + m_ball.getDimens().halfWidth(), new_y);
  }

  if (new_y <= m_bite_upper_border + m_ball.getDimens().halfHeight()) {
    if (!m_is_ball_lost) {
      m_is_ball_lost = !collideBite(new_x);
      correctBallPosition(new_x, m_bite_upper_border + m_ball.getDimens().halfHeight());
    }
  } else if (collideBlock(new_x, new_y)) {
    m_level_finished = (m_level->blockImpact() == 0);
    onCardinalityChanged(m_level->getCardinality());
  }

  if (!m_ball_pose_corrected) {
    new_x = old_x + m_ball.getVelocity() * cos(m_ball.getAngle());
    new_y = old_y + m_ball.getVelocity() * sin(m_ball.getAngle());
    shiftBall(new_x, new_y);
  }
  std::this_thread::sleep_for (std::chrono::milliseconds(ProcessorParams::milliDelay));
}

void GameProcessor::shiftBall(GLfloat new_x, GLfloat new_y) {
  m_ball.setXPose(new_x);
  m_ball.setYPose(new_y);
  move_ball_event.notifyListeners(m_ball);
}

void GameProcessor::shiftBallIntoBlock(int row, int col) {
  GLfloat new_x = 0.f, new_y = 0.f;
  getCenterOfBlock(row, col, &new_x, &new_y);
  correctBallPosition(new_x, new_y);
}

void GameProcessor::onLostBall(bool /* dummy */) {
  m_is_ball_lost = false;
  m_is_ball_death = false;
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_lostBall_id);
}

void GameProcessor::onLevelFinished(bool /* dummy */) {
  m_level_finished = false;
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_levelFinished_id);
}

void GameProcessor::onScoreUpdated(int score) {
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_scoreUpdated_id, score);
}

void GameProcessor::onAngleChanged() {
//  int angle = static_cast<int>(m_ball.getAngle() / util::PI * 180);
//  m_jenv->CallVoidMethod(master_object, fireJavaEvent_angleChanged_id, angle);
}

void GameProcessor::onCardinalityChanged(int new_cardinality) {
  m_jenv->CallVoidMethod(master_object, fireJavaEvent_cardinalityChanged_id, new_cardinality);
}

void GameProcessor::explode(GLfloat x, GLfloat y, const util::BGRA<GLfloat>& color, Kind kind) {
  ExplosionPackage package(x, y, color, kind);
  explosion_event.notifyListeners(package);
}

void GameProcessor::explodeBlock(int row, int col, Kind kind) {
  GLfloat x = 0.f, y = 0.f;
  getCenterOfBlock(row, col, &x, &y);
  Block block = m_level->getBlock(row, col);
  explode(x, y, BlockUtils::getBlockColor(block), kind);
}

void GameProcessor::explodeBlock(int row, int col, const util::BGRA<GLfloat>& color, Kind kind) {
  GLfloat x = 0.f, y = 0.f;
  getCenterOfBlock(row, col, &x, &y);
  explode(x, y, color, kind);
}

void GameProcessor::spawnPrize(GLfloat x, GLfloat y, Prize prize) {
  if (prize != Prize::NONE) {
    PrizePackage package(prizeID++, x, y, prize);
    prize_event.notifyListeners(package);
  }
}

void GameProcessor::spawnPrizeAtBlock(int row, int col, Prize prize) {
  if (prize != Prize::NONE) {
    GLfloat x = 0.f, y = 0.f;
    getCenterOfBlock(row, col, &x, &y);
    spawnPrize(x, y, prize);
  }
}

/* Collision group */
// ----------------------------------------------------------------------------
void GameProcessor::collideLeftBorder() {
  if (m_ball.getAngle() >= util::PI) {
    m_ball.setAngle(util::_3PI - m_ball.getAngle());
  } else if (m_ball.getAngle() >= util::PI2) {
    m_ball.setAngle(util::PI - m_ball.getAngle());
  }
  GLfloat sign = m_ball.getAngle() >= 0.0f ? 1.0f : -1.0f;
  m_ball.setAngle(sign * std::fmod(std::fabs(m_ball.getAngle()), util::_2PI));
  onAngleChanged();
}

void GameProcessor::collideRightBorder() {
  if (m_ball.getAngle() <= util::PI2) {
    m_ball.setAngle(util::PI - m_ball.getAngle());
  } else if (m_ball.getAngle() >= util::_3PI2) {
    m_ball.setAngle(util::_3PI - m_ball.getAngle());
  }
  GLfloat sign = m_ball.getAngle() >= 0.0f ? 1.0f : -1.0f;
  m_ball.setAngle(sign * std::fmod(std::fabs(m_ball.getAngle()), util::_2PI));
  onAngleChanged();
}

void GameProcessor::collideHorizontalSurface() {
  m_ball.setAngle(util::_2PI - m_ball.getAngle());
  GLfloat sign = m_ball.getAngle() >= 0.0f ? 1.0f : -1.0f;
  m_ball.setAngle(sign * std::fmod(std::fabs(m_ball.getAngle()), util::_2PI));
  onAngleChanged();
}

bool GameProcessor::collideBite(GLfloat new_x) {
  if (new_x >= -(m_bite.getDimens().halfWidth() + m_ball.getDimens().halfWidth()) + m_bite.getXPose() &&
      new_x <= (m_bite.getDimens().halfWidth() + m_ball.getDimens().halfWidth()) + m_bite.getXPose()) {

    GLfloat distance = std::fabs(new_x - m_bite.getXPose());
    GLfloat beta = std::fabs(std::atan(distance / m_bite.getRadius()));

    if (new_x >= m_bite.getXPose() + m_bite.getDimens().quarterWidth()) {
      GLfloat normal = std::fabs(util::PI2 - beta);
      if (m_ball.getAngle() >= util::_3PI2) {
        collideHorizontalSurface();
        if (m_ball.getAngle() >= beta) {
          m_ball.setAngle(m_ball.getAngle() - beta);
        } else {
          m_ball.setAngle(m_ball.getAngle() + beta);
        }
        smallAngleAvoid();
      } else if (m_ball.getAngle() >= util::PI) {
        GLfloat gamma = util::_3PI2 - m_ball.getAngle();
        if (gamma <= beta) {
          GLfloat delta = std::fabs(gamma - 2 * beta + util::PI2);
          m_ball.setAngle(delta);
          smallAngleAvoid();
        } else {
          gamma = m_ball.getAngle() - util::PI;
          GLfloat delta = std::fabs(gamma + 2 * beta - util::PI2);
          m_ball.setAngle(util::PI2 - delta);
          smallAngleAvoid();
        }
      }
    } else if (new_x <= m_bite.getXPose() - m_bite.getDimens().quarterWidth()) {
      GLfloat normal = std::fabs(util::PI2 + beta);
      if (m_ball.getAngle() >= util::_3PI2) {
        GLfloat gamma = m_ball.getAngle() - util::_3PI2;
        if (gamma <= beta) {
          GLfloat delta = std::fabs(gamma - 2 * beta + util::PI2);
          m_ball.setAngle(util::PI - delta);
          smallAngleAvoid();
        } else {
          gamma = util::_2PI - m_ball.getAngle();
          GLfloat delta = std::fabs(gamma + 2 * beta - util::PI2);
          m_ball.setAngle(util::PI2 + delta);
          smallAngleAvoid();
        }
      } else if (m_ball.getAngle() >= util::PI) {
        collideHorizontalSurface();
        if (m_ball.getAngle() >= beta + util::PI2) {
          m_ball.setAngle(m_ball.getAngle() - beta);
        } else {
          m_ball.setAngle(m_ball.getAngle() + beta);
        }
        smallAngleAvoid();
      }
    } else {
      collideHorizontalSurface();
    }
    m_ball.setAngle(std::fmod(std::fabs(m_ball.getAngle()), util::_2PI));
    onAngleChanged();
  } else {
    return false;  // ball missed the bite
  }
  return true;
}

bool GameProcessor::collideBlock(GLfloat new_x, GLfloat new_y) {
  if (new_y >= 1.0f - m_ball.getDimens().halfHeight() - m_level_dimens.getHeight() &&
      new_y < 1.0f - m_ball.getDimens().halfHeight()) {
    int row = 0, col = 0;
    if (!getImpactedBlock(new_x, new_y, &row, &col)) {
      return false;  // ball has left level boundaries
    }

    GLfloat top_border = 0.0f, bottom_border = 0.0f, left_border = 0.0f, right_border = 0.0f;
    m_level_dimens.getBlockDimens(row, col, &top_border, &bottom_border, &left_border, &right_border);

    Direction vertical_direction = Direction::NONE;
    Direction horizontal_direction = Direction::NONE;
    if (m_ball.getPose().getX() + 1.0f > left_border &&
        m_ball.getPose().getX() + 1.0f < right_border) {
      if (m_ball.getPose().getY() >= 1.0f - top_border) {
        vertical_direction = Direction::DOWN;
      } else if (m_ball.getPose().getY() <= 1.0f - bottom_border) {
        vertical_direction = Direction::UP;
      }
    }
    if (m_ball.getPose().getX() + 1.0f <= left_border) {
      horizontal_direction = Direction::RIGHT;
    } else if (m_ball.getPose().getX() + 1.0f >= right_border) {
      horizontal_direction = Direction::LEFT;
    }
    std::vector<RowCol> affected_blocks;
    std::vector<RowCol> network_blocks;
    affected_blocks.reserve(12);
    network_blocks.reserve(12);
    RowCol single_affected;

    bool external_collision = true;
    int viscosity = 0;
    size_t random_index = 0;
    Mode mode = m_direction_distribution(m_generator) ? Mode::DEGRADE : Mode::UPGRADE;
    Block generated_block = m_level->getGenerator().generateBlock();
    Prize spawned_prize = m_level->getPrizeGenerator().generatePrize();

    Block block = m_level->getBlock(row, col);
    m_level->setBlockImpacted(row, col);
    int score = BlockUtils::getBlockScore(block);

    switch (block) {
      case Block::NONE:
        // no impact and disturbance
        return false;
      // --------------------
      case Block::DESTROY:
        explodeBlock(row, col, BlockUtils::getBlockEdgeColor(Block::DESTROY), Kind::DIVERGE);
        m_is_ball_death = true;
        break;
      case Block::ULTRA_1:
        m_level->forceDropCardinality();
        break;
      // --------------------
      case Block::FOG:
      case Block::GLASS:
      case Block::GLASS_1:
        // fly without disturbance
        break;
      // --------------------
      case Block::ELECTRO:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->destroyBlocksAround(row, col, &affected_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(Block::ELECTRO), Kind::DIVERGE);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::KNOCK_VERTICAL:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->destroyBlocksBehind(row, col, vertical_direction, &affected_blocks);
        if (vertical_direction != Direction::NONE) {
          explodeBlock(row, col, BlockUtils::getBlockColor(Block::KNOCK_VERTICAL), Kind::DIVERGE);
        }
        for (auto& item : affected_blocks) {
          explodeBlock(item.row, item.col, BlockUtils::getBlockColor(Block::KNOCK_VERTICAL), Kind::DIVERGE);
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::KNOCK_HORIZONTAL:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->destroyBlocksBehind(row, col, horizontal_direction, &affected_blocks);
        if (horizontal_direction != Direction::NONE) {
          explodeBlock(row, col, BlockUtils::getBlockColor(Block::KNOCK_HORIZONTAL), Kind::DIVERGE);
        }
        for (auto& item : affected_blocks) {
          explodeBlock(item.row, item.col, BlockUtils::getBlockColor(Block::KNOCK_HORIZONTAL), Kind::DIVERGE);
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::MIDAS:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->modifyBlocksAround(row, col, Block::TITAN, &affected_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(Block::MIDAS), Kind::DIVERGE);
        for (auto& item : affected_blocks) {
          explodeBlock(item.row, item.col, BlockUtils::getBlockColor(Block::TITAN), Kind::DIVERGE);
          block_impact_event.notifyListeners(item);
        }
        m_is_ball_death = true;
        break;
      case Block::NETWORK:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->findBlocks(Block::NETWORK, &network_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(Block::NETWORK), Kind::DIVERGE);
        spawnPrizeAtBlock(row, col, spawned_prize);
        if (!network_blocks.empty()) {
          random_index = util::getRandomElement(network_blocks);
          shiftBallIntoBlock(network_blocks[random_index].row, network_blocks[random_index].col);
        }
        break;
      // --------------------
      case Block::HYPER:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->findBlocks(m_level->generatePresentBlock(), &network_blocks);
        if (!network_blocks.empty()) {
          random_index = util::getRandomElement(network_blocks);
          shiftBallIntoBlock(network_blocks[random_index].row, network_blocks[random_index].col);
        }
        break;
      case Block::ORIGIN:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_ball_is_flying = false;
        correctBallPosition(m_bite.getXPose(), m_bite_upper_border + m_ball.getDimens().halfHeight());
        break;
      // --------------------
      case Block::ROLLING:
        viscosity = m_viscosity_distribution(m_generator);
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, viscosity);
        spawnPrizeAtBlock(row, col, spawned_prize);
        break;
      // --------------------
      case Block::ZYGOTE_SPAWN:
        viscosity += 12;
        // intend no break
      case Block::JELLY:
        viscosity += 27;
        // intend no break
      case Block::WATER:
        viscosity += 18;
        // intend no break
      case Block::YOGURT_1:
        viscosity += 12;
        // intend no break
      case Block::CLAY:
        viscosity += 10;
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, viscosity);
        spawnPrizeAtBlock(row, col, spawned_prize);
        break;
      // --------------------
      case Block::MAGIC:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->modifyBlocksAround(row, col, generated_block, &affected_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(generated_block), Kind::DIVERGE);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::QUICK_1:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        score += m_level->changeBlocksAround(row, col, mode, &affected_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(Block::QUICK_1), Kind::DIVERGE);
        spawnPrizeAtBlock(row, col, spawned_prize);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::YOGURT:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 50);
        score += m_level->modifyBlocksAround(row, col, Block::YOGURT_1, &affected_blocks);
        explodeBlock(row, col, BlockUtils::getBlockColor(Block::YOGURT), Kind::DIVERGE);
        spawnPrizeAtBlock(row, col, spawned_prize);
        for (auto& item : affected_blocks) {
          block_impact_event.notifyListeners(item);
        }
        break;
      case Block::ZYGOTE_1:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        m_level->modifyBlockNear(row, col, Block::ZYGOTE_SPAWN, &single_affected);
        explodeBlock(single_affected.row, single_affected.col, BlockUtils::getBlockColor(Block::ZYGOTE_SPAWN), Kind::CONVERGE);
        spawnPrizeAtBlock(row, col, spawned_prize);
        block_impact_event.notifyListeners(single_affected);
        break;
      // --------------------
      default:
        external_collision = blockCollision(top_border, bottom_border, left_border, right_border, 100 /* elastic */);
        spawnPrizeAtBlock(row, col, spawned_prize);
        break;
    }
    block_impact_event.notifyListeners(RowCol(row, col));
    onScoreUpdated(score);
    return (external_collision &&
            block != Block::NONE &&
            block != Block::DESTROY &&
            block != Block::MIDAS &&
            block != Block::TITAN &&
            block != Block::INVUL &&
            block != Block::EXTRA);

  } else if (new_y >= 1.0f - m_ball.getDimens().halfHeight()) {
    collideHorizontalSurface();
    correctBallPosition(new_x, 1.0f - m_ball.getDimens().halfHeight());
  }
  return false;
}

bool GameProcessor::blockCollision(
    GLfloat top_border,
    GLfloat bottom_border,
    GLfloat left_border,
    GLfloat right_border,
    int viscosity) {

//  if (m_ball.getPose().getX() + 1.0f >= left_border + m_ball.getDimens().halfWidth() &&
//      m_ball.getPose().getX() + 1.0f <= right_border - m_ball.getDimens().halfWidth() &&
//      m_ball.getPose().getY() + 1.0f <= 2.0f - top_border + m_ball.getDimens().halfWidth() &&
//      m_ball.getPose().getY() + 1.0f >= 2.0f - bottom_border - m_ball.getDimens().halfWidth()) {
//    // inside block collision is erroneous
//    WRN("Erroneous inside block collision detected! Cardinality not affected twice");
//    return false;
//  }

  if (m_ball.getPose().getX() + 1.0f > left_border - m_ball.getDimens().halfWidth() &&
      m_ball.getPose().getX() + 1.0f < right_border + m_ball.getDimens().halfWidth() &&
      (m_ball.getPose().getY() + 1.0f >= 2.0f - top_border - m_ball.getDimens().halfWidth() ||
       m_ball.getPose().getY() + 1.0f <= 2.0f - bottom_border + m_ball.getDimens().halfWidth())) {
    collideHorizontalSurface();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  if (m_ball.getPose().getX() + 1.0f <= left_border - m_ball.getDimens().halfWidth()) {
    collideRightBorder();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  if (m_ball.getPose().getX() + 1.0f >= right_border + m_ball.getDimens().halfWidth()) {
    collideLeftBorder();
    viscousAngleDisturbance(viscosity);
    return true;
  }
  INF("Corner collision");
  randomAngle();
  return true;
}

/* Maths group */
// ----------------------------------------------------------------------------
bool GameProcessor::getImpactedBlock(
    GLfloat ball_x,
    GLfloat ball_y,
    int* row,
    int* col) {

  if (m_ball.getPose().getX() >= ball_x) {  // from right
    *col = static_cast<int>(std::floor((ball_x - m_ball.getDimens().halfWidth() + 1.0f) / m_level_dimens.getBlockWidth()));
  } else {  // from left
    *col = static_cast<int>(std::floor((ball_x + m_ball.getDimens().halfWidth() + 1.0f) / m_level_dimens.getBlockWidth()));
  }

  if (m_ball.getPose().getY() >= ball_y) {  // from top
    *row = static_cast<int>(std::floor((1.0f + m_ball.getDimens().halfHeight() - ball_y) / m_level_dimens.getBlockHeight()));
  } else {  // from bottom
    *row = static_cast<int>(std::floor((1.0f - m_ball.getDimens().halfHeight() - ball_y) / m_level_dimens.getBlockHeight()));
  }

  if (*row < 0 || *row >= m_level->numRows() ||
      *col < 0 || *col >= m_level->numCols()) {
    return false;
  }
  return true;
}

void GameProcessor::getCenterOfBlock(int row, int col, GLfloat* x, GLfloat* y) {
  GLfloat top_border = 0.0f, bottom_border = 0.0f, left_border = 0.0f, right_border = 0.0f;
  m_level_dimens.getBlockDimens(row, col, &top_border, &bottom_border, &left_border, &right_border);
  *x = 0.5f * (right_border + left_border) - 1.0f;
  *y = -0.5f * (bottom_border + top_border) + 1.0f;
}

void GameProcessor::correctBallPosition(GLfloat new_x, GLfloat new_y) {
  shiftBall(new_x, new_y);
  m_ball_pose_corrected = true;
}

void GameProcessor::smallAngleAvoid() {
  if (m_ball.getAngle() <= util::PI16) {
    m_ball.setAngle(m_ball.getAngle() + util::PI16);
  } else if (m_ball.getAngle() >= util::_2PI - util::PI16) {
    m_ball.setAngle(m_ball.getAngle() - util::PI16);
  } else if (m_ball.getAngle() >= util::PI - util::PI16 && m_ball.getAngle() <= util::PI) {
    m_ball.setAngle(m_ball.getAngle() - util::PI16);
  } else if (m_ball.getAngle() > util::PI && m_ball.getAngle() <= util::PI + util::PI16) {
    m_ball.setAngle(m_ball.getAngle() + util::PI16);
  } else if (m_ball.getAngle() <= util::PI2 + util::PI16 && m_ball.getAngle() >= util::PI2) {
    m_ball.setAngle(m_ball.getAngle() + util::PI16);
  } else if (m_ball.getAngle() >= util::PI2 - util::PI16 && m_ball.getAngle() < util::PI2) {
    m_ball.setAngle(m_ball.getAngle() - util::PI16);
  } else if (m_ball.getAngle() >= util::_3PI2 && m_ball.getAngle() <= util::_3PI2 + util::PI16) {
    m_ball.setAngle(m_ball.getAngle() + util::PI16);
  } else if (m_ball.getAngle() < util::_3PI2 && m_ball.getAngle() >= util::_3PI2 - util::PI16) {
    m_ball.setAngle(m_ball.getAngle() - util::PI16);
  }
  onAngleChanged();
}

void GameProcessor::randomAngle() {
  std::normal_distribution<float> init_angle_distribution(util::PI4, util::PI12);
  m_ball.setAngle(init_angle_distribution(m_generator));
  m_ball.setAngle(m_ball.getAngle() + (m_direction_distribution(m_generator) ? 0.0f : util::PI2));
  m_ball.setAngle(std::fmod(m_ball.getAngle(), util::_2PI));
  onAngleChanged();
}

void GameProcessor::viscousAngleDisturbance(int viscosity) {
  if (viscosity != 0 && viscosity != 100) {
    GLfloat direction = m_direction_distribution(m_generator) ? 1.0f : -1.0f;
    m_ball.setAngle(m_ball.getAngle() + (direction * m_angle_distribution(m_generator) / 100.0f * viscosity));
    GLfloat sign = m_ball.getAngle() >= 0.0f ? 1.0f : -1.0f;
    m_ball.setAngle(sign * std::fmod(std::fabs(m_ball.getAngle()), util::_2PI));
    smallAngleAvoid();
    onAngleChanged();
  }
}

}

#ifndef __ARKANOID_GAMEPROCESSOR__H__
#define __ARKANOID_GAMEPROCESSOR__H__

#include <atomic>
#include <memory>
#include <mutex>
#include <random>
#include <utility>

#include <jni.h>

#include "ActiveObject.h"
#include "Ball.h"
#include "Bite.h"
#include "Level.h"
#include "LevelDimens.h"
#include "utils.h"

namespace game {

/// @class GameProcessor GameProcessor.h "include/GameProcessor.h"
/// @brief Standalone thread performs game logic calculations.
class GameProcessor : public ActiveObject {
public:
  typedef std::shared_ptr<GameProcessor> Ptr;

  GameProcessor(JavaVM* jvm);
  virtual ~GameProcessor() noexcept;

  /** @defgroup Callbacks These methods are responses of incoming events
   *  which AsyncContext subscribed on.
   *  @{
   */
  /// @brief Called when user requests a level to be loaded
  void callback_loadLevel(Level::Ptr level);
  /// @brief Called when user sends a command to throw a ball.
  void callback_throwBall(bool /* dummy */);
  /// @brief Called when ball has been set to it's initial position.
  void callback_initBall(Ball init_ball);
  /// @brief Called when bite's dimensions have been measured.
  void callback_initBite(Bite bite);
  /// @brief Called when newly loaded level's dimensions have been measured.
  void callback_levelDimens(LevelDimens level_dimens);
  /// @brief Called when bite's location has changed.
  void callback_biteMoved(Bite moved_bite);
  /** @} */  // end of Callbacks group

// ----------------------------------------------
/* Private member-functions */
private:
  /** @defgroup JNIEnvironment Native glue between core and GUI.
   * @{
   */
  /// @brief Attaches this thread to an existing JVM.
  void attachToJVM();
  /// @brief Detaches this thread from an existing JVM it had been
  /// previously attached.
  void detachFromJVM();
  /** @} */  // end of JNIEnvironment group

// ----------------------------------------------
/* Public data-members */
public:
  /** @defgroup Event Outcoming events and listeners for incoming events.
   * @{
   */
  /// @brief Listens for event which occurs when user requests a level to be loaded.
  EventListener<Level::Ptr> load_level_listener;
  /// @brief Listens for event which occurs when user sends throw ball command.
  EventListener<bool> throw_ball_listener;
  /// @brief Listens for event which occurs when ball is placed to some initial position.
  EventListener<Ball> init_ball_position_listener;
  /// @brief Listens for bite's measured dimensions.
  EventListener<Bite> init_bite_listener;
  /// @brief Listens for the values of measured dimensions of loaded level.
  EventListener<LevelDimens> level_dimens_listener;
  /// @brief Listens for bite location changes.
  EventListener<Bite> bite_location_listener;

  /// @brief Notifies ball has moved to a new position.
  Event<Ball> move_ball_event;
  /// @brief Notifies whether the ball has been lost.
  Event<bool> lost_ball_event;
  /// @brief Notifies block has been impacted.
  Event<std::pair<size_t, size_t>> block_impact_event;
  /** @} */  // end of Event group

// ----------------------------------------------
/* Private data-members */
private:
  /** @addtogroup JNIEnvironment
   * @{
   */
  JavaVM* m_jvm;  //!< Pointer to Java Virtual Machine in current session.
  JNIEnv* m_jenv;  //!< Pointer to environment local within this thread.
  /** @} */  // end of JNIEnvironment group

  /** @defgroup LogicData Game logic related data members.
   * @{
   */
  Level::Ptr m_level;  //!< Game level at it's current state.
  bool m_ball_is_flying;  //!< Whether the ball is flying now or not.
  bool m_is_ball_lost;  //!< Whether the ball has been lost or not.
  Ball m_ball;  //!< Physical ball's representation.
  Bite m_bite;  //!< Physical bite's representation.
  GLfloat m_bite_upper_border;  //!< Upper border of bite.
  LevelDimens m_level_dimens;  //!< Measured level's dimensions.
  /** @} */  // end of LogicData group

  /** @defgroup Maths Maths auxiliary members.
   * @{
   */
  std::default_random_engine m_generator;
  std::normal_distribution<float> m_angle_distribution;
  std::bernoulli_distribution m_direction_distribution;
  /** @} */  // Maths

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_jnienvironment_mutex;  //!< Sentinel for thread attach to JVM.
  std::mutex m_load_level_mutex;  //!< Sentinel for load level user request.
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::mutex m_init_ball_position_mutex;  //!< Sentinel for init ball position setting.
  std::mutex m_init_bite_mutex;  //!< Sentinel for initial bite dimensions.
  std::mutex m_level_dimens_mutex;  //!< Sentinel for loaded level dimensions.
  std::mutex m_bite_location_mutex;  //!< Sentinel for bite's center location changes.
  std::atomic_bool m_load_level_received;  //!< Load level request has been received.
  std::atomic_bool m_throw_ball_received;  //!< Throw ball command has been received.
  std::atomic_bool m_init_ball_position_received;  //!< Setting ball into init position received.
  std::atomic_bool m_init_bite_received;  //!< Initial bite dimensions have been received.
  std::atomic_bool m_level_dimens_received;  //!< Level has been loaded and it's dimens received.
  std::atomic_bool m_bite_location_received;  //!< New bite's center location has been received.
  /** @} */  // end of Mutex group

// ----------------------------------------------
/* Private member-functions */
private:
  /** @defgroup ActiveObject Basic thread lifecycle and operation functions.
   * @{
   */
  void onStart() override final;  //!< Right after thread has been launched.
  void onStop() override final;   //!< Right before thread has been stopped.
  /// @brief Automatic check whether this thread should continue to operate.
  /// @return Whether this thread should continue sleeping (false)
  /// or working (true).
  bool checkForWakeUp() override final;
  /// @brief Operate the data or do some job as a response of incoming
  /// outer event.
  void eventHandler() override final;
  /** @} */  // end of ActiveObject group

  /** @defgroup Processors Actions being performed by GameProcessor when
   *  corresponding event occurred and has been caught.
   *  @{
   */
  /// @brief Processing when new level has been loaded.
  void process_loadLevel();
  /// @brief Throws the ball, setting it's initial speed and direction.
  void process_throwBall();
  /// @brief Sets the ball's initial position values.
  void process_initBall();
  /// @brief Set's the bite's measured dimensions.
  void process_initBite();
  /// @brief Processing when new level loaded and it's lower border received.
  void process_levelDimens();
  /// @brief Processing when bite's location has changed.
  void process_biteMoved();
  /** @} */  // end of Processors group

  /** @defgroup LogicFunc Game logic related member functions.
   * @{
   */
  /// @brief Calculates new position of ball according to it's velocity
  /// and then notifies listeners that event 'move_ball_event' occurs.
  /// @details Calculated position is the ball's position in the next frame.
  void moveBall();
  /** @} */  // end of LogicFunc group

  /** @addtogroup Maths
   * @{
   */
  /// @brief Recalculates ball's angle when it faces left border of any object.
  void collideLeftBorder();
  /// @brief Recalculates ball's angle when it faces right border of any object.
  void collideRightBorder();
  /// @brief Recalculates ball's angle when it faces the bite.
  /// @param new_x Position of ball's center along X axis in the next frame.
  /// @return TRUE in case ball collides bite, FALSE if ball misses the bite.
  bool collideBite(GLfloat new_x);
  /// @brief Processing of collision between ball and level's blocks.
  /// @param new_x Position of ball's center along X axis in the next frame.
  /// @param new_y Position of ball's center along Y axis in the next frame.
  /// @return TRUE in case ball collides level's lower border,
  /// FALSE if ball misses such border.
  bool collideBlocks(GLfloat new_x, GLfloat new_y);
  /// @brief Calculates row- and column- index of block which is impacted
  /// with the ball at given location.
  /// @param ball_x Position of ball's center along X axis.
  /// @param ball_y Position of ball's center along Y axis.
  /// @param row Output row index of impacted block of current level.
  /// @param col Output column index of impacted block of current level.
  void getImpactedBlock(GLfloat ball_x, GLfloat ball_y, size_t* row, size_t* col);
  /** @} */  // end of Maths group
};

}

#endif /* __ARKANOID_GAMEPROCESSOR__H__ */

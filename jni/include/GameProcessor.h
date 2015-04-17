#ifndef __ARKANOID_GAMEPROCESSOR__H__
#define __ARKANOID_GAMEPROCESSOR__H__

#include <atomic>
#include <memory>
#include <mutex>
#include <random>

#include <jni.h>

#include "ActiveObject.h"
#include "Ball.h"
#include "Bite.h"
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
  /// @brief Called when user sends a command to throw a ball.
  void callback_throwBall(bool /* dummy */);
  /// @brief Called when ball has been set to it's initial position.
  void callback_initBall(Ball init_ball);
  /// @brief Called when bite's dimensions have been measured.
  void callback_initBite(Bite bite);
  /// @brief Called when new level has been loaded and it's lower border passed.
  void callback_loadLevel(float lower_border);
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
  /// @brief Listens for event which occurs when user sends throw ball command.
  EventListener<bool> throw_ball_listener;
  /// @brief Listens for event which occurs when ball is placed to some initial position.
  EventListener<Ball> init_ball_position_listener;
  /// @brief Listens for bite's measured dimensions.
  EventListener<Bite> init_bite_listener;
  /// @brief Listens for the value of lower border of last loaded level.
  EventListener<float> level_lower_border_listener;
  /// @brief Listens for bite location changes.
  EventListener<Bite> bite_location_listener;

  /// @brief Notifies ball has moved to a new position.
  Event<Ball> move_ball_event;
  /// @brief Notifies whether the ball has been lost.
  Event<bool> lost_ball_event;
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
  bool m_ball_is_flying;  //!< Whether the ball is flying now or not.
  bool m_is_ball_lost;  //!< Whether the ball has been lost or not.
  Ball m_ball;  //!< Physical ball's representation.
  Bite m_bite;  //!< Physical bite's representation.
  GLfloat m_bite_upper_border;  //!< Upper border of bite.
  /// @brief Angle between ball's velocity and positive X axis, in radians.
  GLfloat m_ball_angle;
  GLfloat m_ball_speed;  //!< Value of ball's velocity, pixels per frame.
  GLfloat m_level_lower_border;  //!< Lower border of loaded level in it's current state.
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
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::mutex m_init_ball_position_mutex;  //!< Sentinel for init ball position setting.
  std::mutex m_init_bite_mutex;  //!< Sentinel for initial bite dimensions.
  std::mutex m_level_lower_border_mutex;  //!< Sentinel for loaded level lower border.
  std::mutex m_bite_location_mutex;  //!< Sentinel for bite's center location changes.
  std::atomic_bool m_throw_ball_received;  //!< Throw ball command has been received.
  std::atomic_bool m_init_ball_position_received;  //!< Setting ball into init position received.
  std::atomic_bool m_init_bite_received;  //!< Initial bite dimensions have been received.
  std::atomic_bool m_level_lower_border_received;  //!< Level has been loaded and it's lower border received.
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
  /// @brief Throws the ball, setting it's initial speed and direction.
  void process_throwBall();
  /// @brief Sets the ball's initial position values.
  void process_initBall();
  /// @brief Set's the bite's measured dimensions.
  void process_initBite();
  /// @brief Processing when new level loaded and it's lower border received.
  void process_loadLevel();
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
  /// @brief Recalculates ball's angle when it faces left or right border.
  /// @param new_x Position of ball's center along X axis in the next frame.
  void collideLeftRightBorder(GLfloat new_x);
  /// @brief Recalculates ball's angle when it faces the bite.
  /// @param new_x Position of ball's center along X axis in the next frame.
  /// @return TRUE in case ball collides bite, FALSE if ball misses the bite.
  bool collideBite(GLfloat new_x);
  /** @} */  // end of Maths group
};

}

#endif /* __ARKANOID_GAMEPROCESSOR__H__ */

#ifndef __ARKANOID_GAMEPROCESSOR__H__
#define __ARKANOID_GAMEPROCESSOR__H__

#include <atomic>
#include <memory>
#include <mutex>

#include <jni.h>

#include "ActiveObject.h"
#include "BallPosition.h"
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
  void callback_initBall(BallPosition init_position);
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
  EventListener<BallPosition> init_ball_position_listener;

  /// @brief Notifies ball has moved to a new position.
  Event<BallPosition> move_ball_event;
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
  constexpr static float ballAngle = util::PI4;  //!< Initial angle at game start
  constexpr static float ballSpeed = 0.1f;   //!< Initial speed at game start

  bool m_ball_is_flying;  //!< Whether the ball is flying now or not.
  BallPosition m_ball_location;  //!< Last recorded ball's location.
  /// @brief Angle between ball's velocity and positive X axis, in radians.
  GLfloat m_ball_angle;
  GLfloat m_ball_speed;  //!< Value of ball's velocity, pixels per frame.
  /** @} */  // end of LogicData group

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_jnienvironment_mutex;  //!< Sentinel for thread attach to JVM.
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::mutex m_init_ball_position_mutex;  //!< Sentinel for init ball position setting.
  std::atomic_bool m_throw_ball_received;  //!< Throw ball command has been received.
  std::atomic_bool m_init_ball_position_received;  //!< Setting ball into init position received.
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
  /** @} */  // end of Processors group

  /** @defgroup LogicFunc Game logic related member functions.
   * @{
   */
  /// @brief Calculates new position of ball according to it's velocity
  /// and then notifies listeners that event 'move_ball_event' occurs.
  /// @details Calculated position is the ball's position in the next frame.
  void moveBall();
  /** @} */  // end of LogicFunc group
};

}

#endif /* __ARKANOID_GAMEPROCESSOR__H__ */

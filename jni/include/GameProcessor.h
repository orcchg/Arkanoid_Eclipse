#ifndef INCLUDE_GAMEPROCESSOR_H_
#define INCLUDE_GAMEPROCESSOR_H_

#include <atomic>
#include <memory>
#include <mutex>

#include <jni.h>

#include "ActiveObject.h"

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

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_jnienvironment_mutex;  //!< Sentinel for thread attach to JVM.
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::atomic_bool m_throw_ball_received;  //!< Throw ball command has been received.
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
  /** @} */  // end of Processors group
};

}

#endif /* INCLUDE_GAMEPROCESSOR_H_ */

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
#include "RowCol.h"
#include "utils.h"

namespace game {

/// @class GameProcessor GameProcessor.h "include/GameProcessor.h"
/// @brief Standalone thread performs game logic calculations.
class GameProcessor : public ActiveObject {
public:
  typedef std::shared_ptr<GameProcessor> Ptr;

  GameProcessor(JavaVM* jvm, jobject master_object);
  virtual ~GameProcessor() noexcept;

  /** @defgroup Callbacks These methods are responses of incoming events
   *  which AsyncContext subscribed on.
   *  @{
   */
  /// @brief Called when aspect ratio has been measured.
  void callback_aspectMeasured(float aspect);
  /// @brief Called when user requests a level to be loaded
  void callback_loadLevel(Level::Ptr level);
  /// @brief Called when user sends a command to throw a ball.
  void callback_throwBall(float angle);
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

public:
  /** @addtogroup JNIEnvironment
   * @{
   */
  inline void setMasterObject(jobject object) { master_object = object; }
  inline void setOnLostBallMethodID(jmethodID id) { fireJavaEvent_lostBall_id = id; }
  inline void setOnLevelFinishedMethodID(jmethodID id) { fireJavaEvent_levelFinished_id = id; }
  inline void setOnScoreUpdatedMethodID(jmethodID id) { fireJavaEvent_scoreUpdated_id = id; }
  inline void setOnAngleChangedMethodID(jmethodID id) { fireJavaEvent_angleChanged_id = id; }
  inline void setOnCardinalityChangedMethodID(jmethodID id) { fireJavaEvent_cardinalityChanged_id = id; }
  /** @} */  // end of JNIEnvironment group

// ----------------------------------------------
/* Public data-members */
public:
  /** @defgroup Event Outcoming events and listeners for incoming events.
   * @{
   */
  /// @brief Listens for measured aspect ratio.
  EventListener<float> aspect_ratio_listener;
  /// @brief Listens for event which occurs when user requests a level to be loaded.
  EventListener<Level::Ptr> load_level_listener;
  /// @brief Listens for event which occurs when user sends throw ball command.
  EventListener<float> throw_ball_listener;
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
  Event<RowCol> block_impact_event;
  /// @brief Notifies level has been successfully finished.
  Event<bool> level_finished_event;
  /** @} */  // end of Event group

// ----------------------------------------------
/* Private data-members */
private:
  /** @addtogroup JNIEnvironment
   * @{
   */
  JavaVM* m_jvm;  //!< Pointer to Java Virtual Machine in current session.
  JNIEnv* m_jenv;  //!< Pointer to environment local within this thread.
  jobject master_object;
  jmethodID fireJavaEvent_lostBall_id;
  jmethodID fireJavaEvent_levelFinished_id;
  jmethodID fireJavaEvent_scoreUpdated_id;
  jmethodID fireJavaEvent_angleChanged_id;
  jmethodID fireJavaEvent_cardinalityChanged_id;
  /** @} */  // end of JNIEnvironment group

  /** @defgroup LogicData Game logic related data members.
   * @{
   */
  Level::Ptr m_level;  //!< Game level at it's current state.
  GLfloat m_throw_angle;  //!< Initial throw level between ball's trajectory and X axis.
  GLfloat m_aspect;  //!< Measured aspect ratio.
  bool m_level_finished;  //!< Whether level has been successfully finished.
  bool m_ball_is_flying;  //!< Whether the ball is flying now or not.
  bool m_is_ball_lost;  //!< Whether the ball has been lost or not.
  bool m_is_ball_death;  //!< Whether the ball has been lost after DEATH block collision.
  bool m_ball_pose_corrected;  //!< Auxiliary flag for corrected ball's pose.
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
  std::binomial_distribution<int> m_viscosity_distribution;
  /** @} */  // Maths

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_jnienvironment_mutex;  //!< Sentinel for thread attach to JVM.
  std::mutex m_aspect_ratio_mutex;  //!< Sentinel for measured aspect ratio.
  std::mutex m_load_level_mutex;  //!< Sentinel for load level user request.
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::mutex m_init_ball_position_mutex;  //!< Sentinel for init ball position setting.
  std::mutex m_init_bite_mutex;  //!< Sentinel for initial bite dimensions.
  std::mutex m_level_dimens_mutex;  //!< Sentinel for loaded level dimensions.
  std::mutex m_bite_location_mutex;  //!< Sentinel for bite's center location changes.
  std::atomic_bool m_aspect_ratio_received;  //!< Aspect ratio has been measured.
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
  /// @brief Processing when aspect ratio has been measured.
  void process_aspectMeasured();
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
  /// @brief Shift the ball into specified position.
  /// @param new_x New ball's center position along X axis.
  /// @param new_y New ball's center position along Y axis.
  /// @note Forces ball's movement with notification, only internal uses.
  void shiftBall(GLfloat new_x, GLfloat new_y);
  /// @brief Notifies Java layer the ball has been lost.
  void onLostBall(bool /* dummy */);
  /// @brief Notifies Java layer level has been successfully finished.
  void onLevelFinished(bool /* dummy */);
  /// @brief Notifies Java layer level with additive score value.
  void onScoreUpdated(int score);
  /// @brief Notifies Java layer ball's angle has been changed.
  /// @details Angle re-calculated in degrees.
  void onAngleChanged();
  /// @brief Notifies Java layer cardinality value has been updated.
  void onCardinalityChanged(int new_cardinality);
  /** @} */  // end of LogicFunc group

  /** @defgroup Collision Functions to perform various collisions.
   * @{
   */
  /// @brief Recalculates ball's angle when it faces left border of any object.
  void collideLeftBorder();
  /// @brief Recalculates ball's angle when it faces right border of any object.
  void collideRightBorder();
  /// @brief Recalculates ball's angle when it faces any horizontal surface.
  void collideHorizontalSurface();
  /// @brief Recalculates ball's angle when it faces the bite.
  /// @param new_x Position of ball's center along X axis in the next frame.
  /// @return TRUE in case ball collides bite, FALSE if ball misses the bite.
  bool collideBite(GLfloat new_x);
  /// @brief Processing of collision between ball and level's block.
  /// @param new_x Position of ball's center along X axis in the next frame.
  /// @param new_y Position of ball's center along Y axis in the next frame.
  /// @return TRUE in case ball collides level's lower border,
  /// FALSE if ball misses such border.
  bool collideBlock(GLfloat new_x, GLfloat new_y);
  /// @brief Performs viscous block collision from certain side of block
  /// depending on previous ball's position.
  /// @param top_border Top border of collided block.
  /// @param bottom_border Bottom border of collided block.
  /// @param left_border Left border of collided block.
  /// @param right_border Right border of collided block.
  /// @param viscosity Percentage of viscosity (from 0 to 100)
  /// @return TRUE if block has actually been collided, FALSE otherwise.
  /// @details 0 viscosity - no disturbance, 100 - elastic collision
  bool blockCollision(GLfloat top_border, GLfloat bottom_border, GLfloat left_border, GLfloat right_border, int viscosity);
  /** @} */  // end of Collision group

  /** @addtogroup Maths
   * @{
   */
  /// @brief Calculates row- and column- index of block which is impacted
  /// with the ball at given location.
  /// @param ball_x Position of ball's center along X axis.
  /// @param ball_y Position of ball's center along Y axis.
  /// @param row Output row index of impacted block of current level.
  /// @param col Output column index of impacted block of current level.
  /// @return TRUE is some block has been impacted, FALSE if ball has left level's boundaries
  /// in order to avoid index of block out of bounds.
  bool getImpactedBlock(GLfloat ball_x, GLfloat ball_y, int* row, int* col);
  /// @brief Corrects ball's visual position after collision and notifies
  /// rendering thread.
  /// @param new_x Corrected ball's center position along X axis.
  /// @param new_y Corrected ball's center position along Y axis.
  void correctBallPosition(GLfloat new_x, GLfloat new_y);
  /// @brief Slightly disturbs ball's angle to avoid small declines.
  void smallAngleAvoid();
  /// @brief Sets random value for ball's angle.
  void randomAngle();
  /// @brief Slightly disturbs angle according to viscosity.
  /// @param viscosity 0 viscosity - no disturbance, 100 - elastic collision
  void viscousAngleDisturbance(int viscosity);
  /** @} */  // end of Maths group
};

}

#endif /* __ARKANOID_GAMEPROCESSOR__H__ */

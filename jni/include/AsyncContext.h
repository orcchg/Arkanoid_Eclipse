#ifndef __ARKANOID_ASYNC_CONTEXT__H__
#define __ARKANOID_ASYNC_CONTEXT__H__

#include <atomic>
#include <memory>
#include <mutex>
#include <utility>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include "ActiveObject.h"
#include "Ball.h"
#include "Bite.h"
#include "Level.h"
#include "Shader.h"

namespace game {

/**
 * @class AsyncContext AsyncContext.h "include/AsyncContext.h"
 * @brief Represents a render thread and provides an interface to interact
 * with User layer: input commands, gestures, graphic output and events to GUI.
 */
class AsyncContext : public ActiveObject {
public:
  typedef std::shared_ptr<AsyncContext> Ptr;

  AsyncContext(JavaVM* jvm);
  virtual ~AsyncContext() noexcept;

  /** @defgroup Callbacks These methods are responses of incoming events
   *  which AsyncContext subscribed on.
   *  @{
   */
  /// @brief Called when a surface has been prepared for rendering
  /// and hence corresponding event has occurred.
  /// @param window Pointer to a window associated with the rendering surface.
  void callback_setWindow(ANativeWindow* window);
  /// @brief Called when user makes a motion gesture within the surface.
  /// @param distance Distance the user's pointer has passed.
  void callback_shiftGamepad(float distance);
  /// @brief Called when user sends a command to throw a ball.
  void callback_throwBall(bool /* dummy */);
  /// @brief Called when user requests a level to be loaded
  void callback_loadLevel(Level::Ptr level);
  /// @brief Called when ball has moved to a new position.
  void callback_moveBall(Ball moved_ball);
  /// @brief Called when ball has been lost.
  void callback_lostBall(float is_lost);
  /** @} */  // end of Callbacks group

  /** @defgroup GameStat Get game statistics
   * @{
   */
  /// @brief Gets current state of last loaded level.
  Level::Ptr getCurrentLevelState();
  /** @} */  // end of GameStat group

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
  /// @brief Listens for event which occurs when surface will be prepared.
  EventListener<ANativeWindow*> surface_received_listener;
  /// @brief Listens for event which occurs when user performs a motion gesture.
  EventListener<float> shift_gesture_listener;
  /// @brief Listens for event which occurs when user sends throw ball command.
  EventListener<bool> throw_ball_listener;
  /// @brief Listens for event which occurs when user requests a level to be loaded.
  EventListener<Level::Ptr> load_level_listener;
  /// @brief Listens for event which occurs when ball has moved to a new position.
  EventListener<Ball> move_ball_listener;
  /// @brief Listens for event which occurs when ball has been lost.
  EventListener<bool> lost_ball_listener;

  /// @brief Notifies ball has been placed to it's initial position.
  Event<Ball> init_ball_position_event;
  /// @brief Notifies bite's dimensions have been measured.
  Event<Bite> init_bite_event;
  /// @brief Notifies about lower border of loaded level.
  Event<float> level_lower_border_event;
  /// @brief Notifies bite location has changed.
  Event<Bite> bite_location_event;
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

  /** @defgroup WindowSurface Rendering surface stuff.
   * @see https://www.khronos.org/registry/egl/sdk/docs/man/html/eglIntro.xhtml
   * @{
   */
  /// Pointer to a window associated with the rendering surface.
  ANativeWindow* m_window;
  EGLDisplay m_egl_display;  //!< Connection between EGL API and native display.
  EGLSurface m_egl_surface;  //!< Native window extension with auxiliary buffers.
  EGLContext m_egl_context;  //!< Bind client API rendering to an EGL surface.
  EGLint m_width, m_height;  //!< Surface sizes.
  GLfloat m_aspect;  //!< Surface aspect ratio.
  EGLConfig m_config;  //!< Selected frame buffer configuration, best one.
  EGLint m_num_configs;  //!< Number of available frame buffer configurations.
  EGLint m_format;  //!< Frame buffer native visual type (actual configuration).
  /** @} */  // end of WindowSurface group

  /** @defgroup LogicData Game logic related data members.
   * @{
   */
  GLfloat m_position;  //!< Last received position value of user's motion gesture.
  Bite m_bite;  //!< Physical bite's representation.
  Ball m_ball;  //!< Physical ball's representation.

  GLfloat* m_bite_vertex_buffer;  //!< Re-usable buffer for vertices of bite.
  GLfloat* m_bite_color_buffer;   //!< Re-usable buffer for colors of bite.
  GLfloat* m_ball_vertex_buffer;  //!< Re-usable buffer for vertices of ball.
  GLfloat* m_ball_color_buffer;   //!< Re-usable buffer for color of ball.
  GLushort* m_rectangle_index_buffer;  //!< Re-usable buffer for indices of rectangle.

  Level::Ptr m_level;  //!< Last loaded game level.
  GLfloat* m_level_vertex_buffer;  //!< Re-usable buffer for vertices of level.
  GLfloat* m_level_color_buffer;   //!< Re-usable buffer for colors of level.
  GLushort* m_level_index_buffer;  //!< Re-usable buffer for indices of level's blocks.
  /** @} */  // end of LogicData group

  /** @defgroup Shaders Shaders for rendering game components.
   * @{
   */
  shader::ShaderHelper::Ptr m_level_shader;
  shader::ShaderHelper::Ptr m_bite_shader;
  shader::ShaderHelper::Ptr m_ball_shader;
  /** @} */  // end of Shaders group

  /** @defgroup Mutex Thread-safety variables
   * @{
   */
  std::mutex m_jnienvironment_mutex;  //!< Sentinel for thread attach to JVM.
  std::mutex m_surface_mutex;  //!< Sentinel for window setting.
  std::mutex m_shift_gamepad_mutex;  //!< Sentinel for shifting gesture event.
  std::mutex m_throw_ball_mutex;  //!< Sentinel for throw ball user command.
  std::mutex m_load_level_mutex;  //!< Sentinel for load level user request.
  std::mutex m_move_ball_mutex;  //!< Sentinel for move ball to a new position.
  std::mutex m_lost_ball_mutex;  //!< Sentinel for lost ball flag.
  std::atomic_bool m_surface_received;  //!< Window has been set.
  std::atomic_bool m_shift_gamepad_received;  //!< Shift gesture has occurred.
  std::atomic_bool m_throw_ball_received;  //!< Throw ball command has been received.
  std::atomic_bool m_load_level_received;  //!< Load level request has been received.
  std::atomic_bool m_move_ball_received;  //!< Move ball event has been received.
  std::atomic_bool m_lost_ball_received;  //!< Ball has been lost received.
  /** @} */  // end of Mutex group

  /** @defgroup SafetyFlag Logic-safety variables
   * @{
   */
  bool m_window_set;
  /** @} */  // end of SafetyFlag group

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

  /** @defgroup Processors Actions being performed by AsyncContext when
   *  corresponding event occurred and has been caught.
   *  @{
   */
  /// @brief Given a rendering surface in Java, performs setting of native
  /// window to interact with during actual rendering.
  void process_setWindow();
  /// @brief Performs visual translation of the gamepad by given distance.
  void process_shiftGamepad();
  /// @brief Performs visual ball throwing.
  void process_throwBall();
  /// @brief Performs visual refreshing of current level.
  void process_loadLevel();
  /// @brief Performs visual displacement of the ball to a new position.
  void process_moveBall();
  /// @brief Processing when ball has been lost.
  void process_lostBall();
  /** @} */  // end of Processors group

private:
  /** @defgroup LogicFunc Game logic related member functions.
   * @{
   */
  /// @brief Initialize game state.
  void initGame();
  /// @brief Sets the bite into shifted state.
  /// @param position Normalized position the bite should move at.
  /// @note Position should be within [-1, 1] segment.
  void moveBite(float position);
  /// @brief Sets the ball into shifted state.
  /// @param x_position Normalized position along X axis the ball should move at.
  /// @param y_position Normalized position along Y axis the ball should move at.
  /// @note Positions should both be within [-1, 1] segment.
  void moveBall(float x_position, float y_position);
  /** @} */  // end of LogicFunc group

private:
  /** @defgroup GraphicsContext Low-layers functions to setup graphics.
   * @{
   */
  /// @brief Configures rendering surface, context and display.
  /// @return false in case of error, true in case of success.
  bool displayConfig();
  /// @brief Sets gl options for already prepared context.
  /// @note Different contexts could have different gl options.
  void glOptionsConfig();
  /// @brief Releases surface, context and display resources.
  void destroyDisplay();
  /// @brief Render a frame.
  void render();
  /** @} */  // end of GraphicsContext group

  /** @defgroup Drawings Draw routines.
   * @{
   */
  /// @brief Draws current level's state.
  void drawLevel();
  /// @brief Draws bite at it's current position.
  void drawBite();
  /// @brief Draw ball at it's current position.
  void drawBall();
  /** @} */  // end of Drawings group
};

}  // namespace game

#endif  // __ARKANOID_ASYNC_CONTEXT__H__

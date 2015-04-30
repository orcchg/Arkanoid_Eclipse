#include <cmath>

#include <GLES2/gl2.h>

#include "AsyncContext.h"
#include "EGLConfigChooser.h"
#include "Exceptions.h"
#include "logger.h"
#include "Macro.h"
#include "Params.h"
#include "utils.h"

namespace game {

/* Public API */
// ----------------------------------------------------------------------------
AsyncContext::AsyncContext(JavaVM* jvm)
  : m_jvm(jvm), m_jenv(nullptr)
  , m_window(nullptr)
  , m_egl_display(EGL_NO_DISPLAY)
  , m_egl_surface(EGL_NO_SURFACE)
  , m_egl_context(EGL_NO_CONTEXT)
  , m_width(0), m_height(0)
  , m_config(nullptr)
  , m_num_configs(0), m_format(0)
  , m_position(0.0f)
  , m_bite()
  , m_ball()
  , m_impact_queue()
  , m_bite_vertex_buffer(new GLfloat[16])
  , m_bite_color_buffer(new GLfloat[16])
  , m_ball_vertex_buffer(new GLfloat[16])
  , m_ball_color_buffer(new GLfloat[16])
  , m_rectangle_index_buffer(new GLushort[6]{0, 3, 2, 0, 1, 3})
  , m_rectangle_texCoord_buffer(new GLfloat[12]{1.f,1.f,0.f,1.f,0.f,0.f,1.f,0.f,1.f,1.f})
  , m_level(nullptr)
  , m_level_vertex_buffer(nullptr)
  , m_level_color_buffer(nullptr)
  , m_level_index_buffer(nullptr)
  , m_level_shader(nullptr) {

  DBG("enter AsyncContext ctor");
  m_surface_received.store(false);
  m_load_resources_received.store(false);
  m_shift_gamepad_received.store(false);
  m_throw_ball_received.store(false);
  m_load_level_received.store(false);
  m_move_ball_received.store(false);
  m_lost_ball_received.store(false);
  m_block_impact_received.store(false);
  m_level_finished_received.store(false);
  m_window_set = false;
  m_resources = nullptr;

  util::setColor(util::MAGENTA, &m_bite_color_buffer[0], 16);
  util::setColor(util::ORANGE, &m_ball_color_buffer[0], 16);
  DBG("exit AsyncContext ctor");
}

AsyncContext::~AsyncContext() {
  DBG("enter AsyncContext ~dtor");
  m_jvm = nullptr;  m_jenv = nullptr;
  m_window = nullptr;
  destroyDisplay();

  delete [] m_bite_vertex_buffer; m_bite_vertex_buffer = nullptr;
  delete [] m_bite_color_buffer; m_bite_color_buffer = nullptr;
  delete [] m_ball_vertex_buffer; m_ball_vertex_buffer = nullptr;
  delete [] m_ball_color_buffer; m_ball_color_buffer = nullptr;
  delete [] m_rectangle_index_buffer; m_rectangle_index_buffer = nullptr;
  delete [] m_rectangle_texCoord_buffer; m_rectangle_texCoord_buffer = nullptr;

  m_level = nullptr;
  delete [] m_level_vertex_buffer; m_level_vertex_buffer = nullptr;
  delete [] m_level_color_buffer; m_level_color_buffer = nullptr;
  delete [] m_level_index_buffer; m_level_index_buffer = nullptr;

  m_resources = nullptr;
  DBG("exit AsyncContext ~dtor");
}

/* Callbacks group */
// ----------------------------------------------------------------------------
void AsyncContext::callback_setWindow(ANativeWindow* window) {
  std::unique_lock<std::mutex> lock(m_surface_mutex);
  m_surface_received.store(true);
  m_window = window;
  interrupt();
}

void AsyncContext::callback_loadResources(bool /* dummy */) {
  std::unique_lock<std::mutex> lock(m_load_resources_mutex);
  m_load_resources_received.store(true);
  interrupt();
}

void AsyncContext::callback_shiftGamepad(float position) {
  std::unique_lock<std::mutex> lock(m_shift_gamepad_mutex);
  m_shift_gamepad_received.store(true);
  m_position = position;
  interrupt();
}

void AsyncContext::callback_throwBall(float angle /* dummy */) {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  m_throw_ball_received.store(true);
  interrupt();
}

void AsyncContext::callback_loadLevel(Level::Ptr level) {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  m_load_level_received.store(true);
  m_level = level;
  interrupt();
}

void AsyncContext::callback_moveBall(Ball moved_ball) {
  std::unique_lock<std::mutex> lock(m_move_ball_mutex);
  m_move_ball_received.store(true);
  m_ball = moved_ball;
  interrupt();
}

void AsyncContext::callback_lostBall(float is_lost) {
  std::unique_lock<std::mutex> lock(m_lost_ball_mutex);
  m_lost_ball_received.store(true);
  interrupt();
}

void AsyncContext::callback_blockImpact(RowCol block) {
  std::unique_lock<std::mutex> lock(m_block_impact_mutex);
  m_block_impact_received.store(true);
  m_impact_queue.push(block);
  interrupt();
}

void AsyncContext::callback_levelFinished(bool is_finished) {
  std::unique_lock<std::mutex> lock(m_level_finished_mutex);
  m_level_finished_received.store(true);
  interrupt();
}

// ----------------------------------------------
Level::Ptr AsyncContext::getCurrentLevelState() {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  return m_level;
}

void AsyncContext::setResourcesPtr(Resources* resources) {
  m_resources = resources;
}

/* *** Private methods *** */
/* JNIEnvironment group */
// ----------------------------------------------------------------------------
void AsyncContext::attachToJVM() {
  std::unique_lock<std::mutex> lock(m_jnienvironment_mutex);
  auto result = m_jvm->AttachCurrentThread(&m_jenv, nullptr /* thread args */);
  if (result != JNI_OK) {
    ERR("AsyncContext thread was not attached to JVM !");
    throw JNI_exception();
  }
}

void AsyncContext::detachFromJVM() {
  std::unique_lock<std::mutex> lock(m_jnienvironment_mutex);
  m_jvm->DetachCurrentThread();
}

/* ActiveObject group */
// ----------------------------------------------------------------------------
void AsyncContext::onStart() {
  attachToJVM();
}

void AsyncContext::loadResources() {

}
void AsyncContext::onStop() {
  detachFromJVM();
}

bool AsyncContext::checkForWakeUp() {
  return m_surface_received.load() ||
      m_load_resources_received.load() ||
      m_shift_gamepad_received.load() ||
      m_throw_ball_received.load() ||
      m_load_level_received.load() ||
      m_move_ball_received.load() ||
      m_lost_ball_received.load() ||
      m_block_impact_received.load() ||
      m_level_finished_received.load();
}

void AsyncContext::eventHandler() {
  if (m_surface_received.load()) {
    m_surface_received.store(false);
    process_setWindow();
    initGame();
  }
  if (m_window_set) {
    if (m_load_resources_received.load()) {
      m_load_resources_received.store(false);
      process_loadResources();
    }
    if (m_shift_gamepad_received.load()) {
      m_shift_gamepad_received.store(false);
      process_shiftGamepad();
    }
    if (m_block_impact_received.load()) {
      m_block_impact_received.store(false);
      process_blockImpact();
    }  // block impact must be processed before
       // new level loaded to avoid crash
    if (m_load_level_received.load()) {
      m_load_level_received.store(false);
      process_loadLevel();
    }
    if (m_throw_ball_received.load()) {
      m_throw_ball_received.store(false);
      process_throwBall();
    }
    if (m_move_ball_received.load()) {
      m_move_ball_received.store(false);
      process_moveBall();
    }
    if (m_lost_ball_received.load()) {
      m_lost_ball_received.store(false);
      process_lostBall();
    }
    if (m_level_finished_received.load()) {
      m_level_finished_received.store(false);
      process_levelFinished();
    }
    render();  // render frame to reflect changes occurred
  } else {
    // window has not been set, skip any other events
  }
}

/* Processors group */
// ----------------------------------------------------------------------------
void AsyncContext::process_setWindow() {
  std::unique_lock<std::mutex> lock(m_surface_mutex);
  DBG("enter AsyncContext::process_setWindow()");
  if (m_window == nullptr) {
    m_window_set = false;
    ERR("Failed to set window !");
    return;
  }
  if (!displayConfig()) {
    ERR("Failed to configure display, surface and context !");
    throw GraphicsNotConfiguredException();
  }
  glOptionsConfig();
  m_window_set = true;
  DBG("exit AsyncContext::process_setWindow()");
}

void AsyncContext::process_loadResources() {
  std::unique_lock<std::mutex> lock(m_load_resources_mutex);
  if (m_resources != nullptr) {
    for (auto it = m_resources->begin(); it != m_resources->end(); ++it) {
      it->second->load();
    }
  } else {
    ERR("Resources pointer was not set !");
  }
}

void AsyncContext::process_shiftGamepad() {
  std::unique_lock<std::mutex> lock(m_shift_gamepad_mutex);
  moveBite(m_position);
}

void AsyncContext::process_throwBall() {
  std::unique_lock<std::mutex> lock(m_throw_ball_mutex);
  INF("Ball has been thrown");
  // no-op
}

void AsyncContext::process_loadLevel() {
  std::unique_lock<std::mutex> lock(m_load_level_mutex);
  std::queue<RowCol> empty_queue;
  std::swap(m_impact_queue, empty_queue);

  // release memory allocated for previous level if any
  delete [] m_level_vertex_buffer;
  delete [] m_level_color_buffer;
  delete [] m_level_index_buffer;
  m_level_vertex_buffer = new GLfloat[m_level->size() * 16];
  m_level_color_buffer = new GLfloat[m_level->size() * 16];
  m_level_index_buffer = new GLushort[m_level->size() * 6];

  LevelDimens dimens(
      m_level->numRows(),
      m_level->numCols(),
      m_level->numCols() * LevelDimens::blockWidth,
      m_level->numRows() * LevelDimens::blockHeight * m_aspect,
      LevelDimens::blockWidth,
      LevelDimens::blockHeight * m_aspect);

  m_level->toVertexArray(dimens.getBlockWidth(), dimens.getBlockHeight(), -1.0f, 1.0f, &m_level_vertex_buffer[0]);
  m_level->fillColorArray(&m_level_color_buffer[0]);
  util::rectangleIndices(&m_level_index_buffer[0], m_level->size() * 6);

  level_dimens_event.notifyListeners(dimens);
}

void AsyncContext::process_moveBall() {
  std::unique_lock<std::mutex> lock(m_move_ball_mutex);
  moveBall(m_ball.getPose().getX(), m_ball.getPose().getY());
}

void AsyncContext::process_lostBall() {
  std::unique_lock<std::mutex> lock(m_lost_ball_mutex);
  initGame();
}

void AsyncContext::process_blockImpact() {
  std::unique_lock<std::mutex> lock(m_block_impact_mutex);
  while (!m_impact_queue.empty()) {
    auto impact = m_impact_queue.front();
    m_level->fillColorArrayAtBlock(&m_level_color_buffer[0], impact.row, impact.col);
    m_impact_queue.pop();
  }
}

void AsyncContext::process_levelFinished() {
  std::unique_lock<std::mutex> lock(m_level_finished_mutex);
  initGame();
}

/* LogicFunc group */
// ----------------------------------------------------------------------------
void AsyncContext::initGame() {
  aspect_ratio_event.notifyListeners(m_aspect);

  // ensure correct initial location
  m_bite = Bite(BiteParams::biteWidth, BiteParams::biteHeight * m_aspect);
  moveBite(0.0f);

  m_ball = Ball(BallParams::ballSize, BallParams::ballSize * m_aspect);
  m_ball.setXPose(m_bite.getXPose());
  m_ball.setYPose(-BiteParams::neg_biteElevation + m_ball.getDimens().halfHeight());
  moveBall(m_ball.getPose().getX(), m_ball.getPose().getY());

  init_ball_position_event.notifyListeners(m_ball);
  init_bite_event.notifyListeners(m_bite);
}

void AsyncContext::moveBite(float position) {
  if (std::fabs(position - m_bite.getXPose()) >= BiteParams::biteTouchArea) {
    // finger position is out of bite's borders
    return;
  }

  m_bite.setXPose(position);
  if (m_bite.getXPose() > 1.0f - m_bite.getDimens().halfWidth()) {
    m_bite.setXPose(1.0f - m_bite.getDimens().halfWidth());
  } else if (m_bite.getXPose() < m_bite.getDimens().halfWidth() - 1.0f) {
    m_bite.setXPose(m_bite.getDimens().halfWidth() - 1.0f);
  }

  util::setRectangleVertices(
      &m_bite_vertex_buffer[0],
      BiteParams::biteWidth, m_bite.getDimens().height(),
      -m_bite.getDimens().halfWidth() + m_bite.getXPose(),
      -BiteParams::neg_biteElevation,
      1, 1);

  bite_location_event.notifyListeners(m_bite);
}

void AsyncContext::moveBall(float x_position, float y_position) {
  util::setRectangleVertices(
      &m_ball_vertex_buffer[0],
      m_ball.getDimens().width(), m_ball.getDimens().height(),
      -m_ball.getDimens().halfWidth() + x_position,
      m_ball.getDimens().halfHeight() + y_position,
      1, 1);
}

/* GraphicsContext group */
// ----------------------------------------------------------------------------
bool AsyncContext::displayConfig() {
  DBG("enter AsyncContext::displayConfig()");
  eglBindAPI(EGL_OPENGL_ES_API);

  if ((m_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    ERR("eglGetDisplay() returned error %d", eglGetError());
    return false;
  }

  if (!eglInitialize(m_egl_display, 0, 0)) {
    ERR("eglInitialize() returned error %d", eglGetError());
    return false;
  }

  {
    EGLConfigChooser eglConfigChooser(5, 6, 5, 0, 16, 0);
    m_config = eglConfigChooser.chooseConfig(m_egl_display);
    m_num_configs = eglConfigChooser.getNumberConfigs();
    DBG("Number of EGL display configs: %i", m_num_configs);
  }

  if (!eglGetConfigAttrib(m_egl_display, m_config, EGL_NATIVE_VISUAL_ID, &m_format)) {
    ERR("eglGetConfigAttrib() returned error %d", eglGetError());
    destroyDisplay();
    return false;
  }

  if (!(m_egl_surface = eglCreateWindowSurface(m_egl_display, m_config, m_window, 0))) {
    ERR("eglCreateWindowSurface() returned error %d", eglGetError());
    destroyDisplay();
    return false;
  }

  {
    const EGLint attributes[] = {
          EGL_CONTEXT_CLIENT_VERSION, 3,
          EGL_NONE
      };
    if (!(m_egl_context = eglCreateContext(m_egl_display, m_config, 0, attributes))) {
      ERR("eglCreateContext() returned error %d", eglGetError());
      destroyDisplay();
      return false;
    }
  }

  if (!eglMakeCurrent(m_egl_display, m_egl_surface, m_egl_surface, m_egl_context)) {
    ERR("eglMakeCurrent() returned error %d", eglGetError());
    destroyDisplay();
    return false;
  }

  if (!eglQuerySurface(m_egl_display, m_egl_surface, EGL_WIDTH, &m_width) ||
      !eglQuerySurface(m_egl_display, m_egl_surface, EGL_HEIGHT, &m_height)) {
    ERR("eglQuerySurface() returned error %d", eglGetError());
    destroyDisplay();
    return false;
  }
  m_aspect = (GLfloat) m_width / m_height;
  DBG("Surface width = %i, height = %i, aspect = %lf", m_width, m_height, m_aspect);

  /// @see http://android-developers.blogspot.kr/2013_09_01_archive.html
  ANativeWindow_setBuffersGeometry(m_window, 0, 0, m_format);
  DBG("exit AsyncContext::displayConfig()");
  return true;
}

void AsyncContext::glOptionsConfig() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glViewport(-4, -4, m_width + 4, m_height + 4);

#if USE_TEXTURE
  m_level_shader = std::make_shared<shader::ShaderHelper>(shader::SimpleTextureShader());
#else
  m_level_shader = std::make_shared<shader::ShaderHelper>(shader::SimpleShader());
#endif  // USE_TEXTURE
  m_bite_shader = std::make_shared<shader::ShaderHelper>(shader::SimpleShader());
  m_ball_shader = std::make_shared<shader::ShaderHelper>(shader::SimpleShader());

  /* Rendering options */
  // TODO: not need depth (and display config should exclude that) for 2D game
//  glFrontFace(GL_CCW);
//  glCullFace(GL_BACK);
//  glDepthFunc(GL_LESS);
//  glDepthMask(GL_TRUE);
//  glEnable(GL_DEPTH_TEST);
//  glEnable(GL_NORMALIZE);
  glEnable(GL_TEXTURE);
}

void AsyncContext::destroyDisplay() {
  if (m_egl_display != EGL_NO_DISPLAY) {
    eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (m_egl_context != EGL_NO_CONTEXT) {
      eglDestroyContext(m_egl_display, m_egl_context);
      m_egl_context = EGL_NO_CONTEXT;
    }
    if (m_egl_surface != EGL_NO_SURFACE) {
      eglDestroySurface(m_egl_display, m_egl_surface);
      m_egl_surface = EGL_NO_SURFACE;
    }
    eglTerminate (m_egl_display);
    m_egl_display = EGL_NO_DISPLAY;
  }
}

void AsyncContext::render() {
  if (m_egl_display != EGL_NO_DISPLAY) {
    glClear(GL_COLOR_BUFFER_BIT);
//    drawRectangle();  // XXX
#if USE_TEXTURE
    for (int r = 0; r < m_level->numRows(); ++r) {
      for (int c = 0; c < m_level->numCols(); ++c) {
        drawBlock(r, c);
      }
    }
#else
    drawLevel();
#endif
    drawBite();
    drawBall();
    eglSwapInterval(m_egl_display, 0);
    eglSwapBuffers(m_egl_display, m_egl_surface);
  }
}

/* Drawings group */
// ----------------------------------------------------------------------------
void AsyncContext::drawLevel() {
  m_level_shader->useProgram();

  GLuint a_position = m_level_shader->getVertexAttribLocation();
  GLuint a_color = m_level_shader->getColorAttribLocation();

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_level_vertex_buffer[0]);
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_level_color_buffer[0]);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_color);

  glDrawElements(GL_TRIANGLES, m_level->size() * 6, GL_UNSIGNED_SHORT, &m_level_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_color);
}

void AsyncContext::drawBlock(int row, int col) {
  m_level_shader->useProgram();

  GLuint a_position = m_level_shader->getVertexAttribLocation();
#if USE_TEXTURE
  GLuint a_texCoord = m_level_shader->getTexCoordAttribLocation();
#else
  GLuint a_color = m_level_shader->getColorAttribLocation();
#endif

  int rci = col * 16 + row * m_level->numCols() * 16;
  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_level_vertex_buffer[rci]);
#if USE_TEXTURE
  glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, &m_rectangle_texCoord_buffer[0]);
#else
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_level_color_buffer[rci]);
#endif

  glEnableVertexAttribArray(a_position);
#if USE_TEXTURE
  glEnableVertexAttribArray(a_texCoord);
  // TODO: apply texture for specified block
  m_resources->getTexture("brick_tex.png")->apply();
  GLint sampler = m_level_shader->getSampler2DUniformLocation();
  glUniform1i(sampler, 0);
#else
  glEnableVertexAttribArray(a_color);
#endif

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &m_rectangle_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
#if USE_TEXTURE
  glDisableVertexAttribArray(a_texCoord);
#else
  glDisableVertexAttribArray(a_color);
#endif
}

void AsyncContext::drawBite() {
  m_bite_shader->useProgram();

  GLuint a_position = m_bite_shader->getVertexAttribLocation();
  GLuint a_color = m_bite_shader->getColorAttribLocation();

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_bite_vertex_buffer[0]);
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_bite_color_buffer[0]);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_color);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &m_rectangle_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_color);
}

void AsyncContext::drawBall() {
  m_ball_shader->useProgram();

  GLuint a_position = m_ball_shader->getVertexAttribLocation();
  GLuint a_color = m_ball_shader->getColorAttribLocation();

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_ball_vertex_buffer[0]);
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_ball_color_buffer[0]);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_color);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &m_rectangle_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_color);
}

void AsyncContext::drawRectangle() {
//  m_ball_shader->useProgram();

//  GLuint a_position = m_ball_shader->getVertexAttribLocation();
//  GLuint a_color = m_ball_shader->getColorAttribLocation();
//  GLuint a_texCoord = m_ball_shader->getTexCoordAttribLocation();
  GLuint a_position = m_level_shader->getVertexAttribLocation();
  GLuint a_color = m_level_shader->getColorAttribLocation();
  GLuint a_texCoord = m_level_shader->getTexCoordAttribLocation();

  GLfloat* vertex_buffer = new GLfloat[16]{-0.5f, -0.5f, 0.0f, 1.0f,
                                          0.5f, -0.5f, 0.0f, 1.0f,
                                          -0.5f, 0.5f, 0.0f, 1.0f,
                                          0.5f, 0.5f, 0.0f, 1.0f};

  GLfloat* texCoord_buffer = new GLfloat[8]{0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f};

  GLfloat* color_buffer = new GLfloat[16]{1.f, 0.f, 0.f, 1.f,
                                          1.f, 0.f, 0.f, 1.f,
                                          1.f, 0.f, 0.f, 1.f,
                                          1.f, 0.f, 0.f, 1.f};

  m_resources->getTexture("brick_tex.png")->apply();
  GLint sampler = m_level_shader->getSampler2DUniformLocation();
  glUniform1i(sampler, 0);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_texCoord);
//  glEnableVertexAttribArray(a_color);

  m_level_shader->useProgram();

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &vertex_buffer[0]);
  glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, &texCoord_buffer[0]);
//  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &color_buffer[0]);



//  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &m_rectangle_index_buffer[0]);
  glDrawArrays(GL_TRIANGLES, 0, 4);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_texCoord);
//  glDisableVertexAttribArray(a_color);

  delete [] vertex_buffer;
  delete [] texCoord_buffer;
  delete [] color_buffer;
}

}  // namespace game

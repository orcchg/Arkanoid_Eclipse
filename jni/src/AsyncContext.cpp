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
  , m_ball_vertex_buffer(new GLfloat[36])
  , m_ball_color_buffer(new GLfloat[36])
  , m_bg_vertex_buffer(new GLfloat[16]{-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f})
  , m_particle_buffer(nullptr)
  , m_particle_spiral_buffer(nullptr)
  , m_rectangle_index_buffer(new GLushort[6]{0, 3, 2, 0, 1, 3})
  , m_octagon_index_buffer(new GLushort[24]{0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1})
  , m_rectangle_texCoord_buffer(new GLfloat[8]{1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f})
  , m_level(nullptr)
  , m_level_vertex_buffer(nullptr)
  , m_level_color_buffer(nullptr)
  , m_level_index_buffer(nullptr)
  , m_generator(std::chrono::system_clock::now().time_since_epoch().count())
  , m_particle_distribution(0.0f, 1.0f)
  , m_last_time(0)
  , m_particle_time(0.0f)
  , m_render_explosion(false)
  , m_explosion_packages()
  , m_prize_last_time(0)
  , m_prize_time(0.0f)
  , m_prize_packages()
  , m_removed_prizes()
  , m_prize_catch_last_time(0)
  , m_prize_catch_time(0.0f)
  , m_level_shader(nullptr)
  , m_bite_shader(nullptr)
  , m_ball_shader(nullptr)
  , m_explosion_shader(nullptr)
  , m_sample_shader(nullptr)
  , m_prize_shader(nullptr)
  , m_prize_catch_shader(nullptr) {

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
  m_explosion_received.store(false);
  m_prize_received.store(false);
  m_prize_caught_received.store(false);
  m_window_set = false;
  m_resources = nullptr;

  util::setColor(util::SALMON, &m_bite_color_buffer[0], 8);
  util::setColor(util::SIENNA_DARK, &m_bite_color_buffer[8], 8);

  util::setColor(util::ORANGE, &m_ball_color_buffer[0], 4);
  util::setColor(util::SIENNA_LIGHT, &m_ball_color_buffer[4], 16);
  util::setColor(util::SIENNA, &m_ball_color_buffer[20], 4);
  util::setColor(util::SIENNA_DARK, &m_ball_color_buffer[24], 12);
  util::setColor(util::SIENNA, &m_ball_color_buffer[32], 4);

  m_particle_buffer = new GLfloat[particleSize * particleSystemSize];
  m_particle_spiral_buffer = new GLfloat[particleSpiralSize * particleSpiralSystemSize];
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
  delete [] m_bg_vertex_buffer; m_bg_vertex_buffer = nullptr;
  delete [] m_particle_buffer; m_particle_buffer = nullptr;
  delete [] m_particle_spiral_buffer; m_particle_spiral_buffer = nullptr;
  delete [] m_rectangle_index_buffer; m_rectangle_index_buffer = nullptr;
  delete [] m_octagon_index_buffer; m_octagon_index_buffer = nullptr;
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

void AsyncContext::callback_explosion(ExplosionPackage package) {
  std::unique_lock<std::mutex> lock(m_explosion_mutex);
  m_explosion_received.store(true);
  m_explosion_packages.push_back(package);
  interrupt();
}

void AsyncContext::callback_prizeReceived(PrizePackage package) {
  std::unique_lock<std::mutex> lock(m_prize_mutex);
  m_prize_received.store(true);
  m_prize_packages[package.getID()] = package;
  interrupt();
}

void AsyncContext::callback_prizeCaught(int prize_id) {
  std::unique_lock<std::mutex> lock(m_prize_caught_mutex);
  m_prize_caught_received.store(true);
  addPrizeToRemoved(prize_id);
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
      m_level_finished_received.load() ||
      m_explosion_received.load() ||
      m_prize_received.load() ||
      m_prize_caught_received.load();
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
    if (m_explosion_received.load()) {
      m_explosion_received.store(false);
      process_explosion();
    }
    if (m_prize_received.load()) {
      m_prize_received.store(false);
      process_prizeReceived();
    }
    if (m_prize_caught_received.load()) {
      m_prize_caught_received.store(false);
      process_prizeCaught();
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
  initParticleSystem();
  m_window_set = true;
  DBG("exit AsyncContext::process_setWindow()");
}

void AsyncContext::process_loadResources() {
  std::unique_lock<std::mutex> lock(m_load_resources_mutex);
  if (m_resources != nullptr) {
    for (auto it = m_resources->begin(); it != m_resources->end(); ++it) {
      DBG("Loading resources: %s %p", it->first.c_str(), it->second);
      it->second->load();
    }
  } else {
    ERR("Resources pointer was not set !");
  }
  m_bg_texture = m_resources->getRandomTexture("bg");
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
  {
    std::queue<RowCol> empty_queue;
    std::swap(m_impact_queue, empty_queue);
  }

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
  if (m_render_explosion) {
    moveBall(0.0f, 1000.f);
    delay(65);
  }
  initGame();
}

void AsyncContext::process_blockImpact() {
  std::unique_lock<std::mutex> lock(m_block_impact_mutex);
  while (!m_impact_queue.empty()) {
    auto impact = m_impact_queue.front();
    if (!checkBlockPresense(impact.row, impact.col)) {
      WRN("Impacted block is absent in level!");
      std::queue<RowCol> empty_queue;
      std::swap(m_impact_queue, empty_queue);
      return;
    }
    m_level->fillColorArrayAtBlock(&m_level_color_buffer[0], impact.row, impact.col);
    m_impact_queue.pop();
  }
}

void AsyncContext::process_levelFinished() {
  std::unique_lock<std::mutex> lock(m_level_finished_mutex);
  m_bg_texture = m_resources->getRandomTexture("bg");
  if (m_render_explosion) {
    moveBall(0.0f, 1000.f);
    delay(65);
  }
  initGame();
}

void AsyncContext::process_explosion() {
  std::unique_lock<std::mutex> lock(m_explosion_mutex);
  m_last_time = 0;
  m_render_explosion = true;
}

void AsyncContext::process_prizeReceived() {
  std::unique_lock<std::mutex> lock(m_prize_mutex);
  // no-op
}

void AsyncContext::process_prizeCaught() {
  std::unique_lock<std::mutex> lock(m_prize_caught_mutex);
  // no-op
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
  util::setOctagonVertices(
      &m_ball_vertex_buffer[0],
      m_ball.getDimens().width(), m_ball.getDimens().height(),
      -m_ball.getDimens().halfWidth() + x_position,
      m_ball.getDimens().halfHeight() + y_position,
      1, 1);
}

void AsyncContext::addPrizeToRemoved(int prize_id) {
  m_removed_prizes.insert(prize_id);
}

void AsyncContext::clearRemovedPrizes() {
  for (auto& item : m_removed_prizes) {
    m_prize_packages.erase(item);
  }
  m_removed_prizes.clear();
}

bool AsyncContext::checkBlockPresense(int row, int col) {
  return (row >= 0 && row < m_level->numRows()) && (col >= 0 && col < m_level->numCols());
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
  m_explosion_shader = std::make_shared<shader::ShaderHelper>(shader::ParticleSystemShader());
  m_sample_shader = std::make_shared<shader::ShaderHelper>(shader::SimpleTextureShader());
  m_prize_shader = std::make_shared<shader::ShaderHelper>(shader::VerticalFallShader());
  m_prize_catch_shader = std::make_shared<shader::ShaderHelper>(shader::ParticleMoveShader());
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
    drawBackground();

#if USE_TEXTURE
    for (int r = 0; r < m_level->numRows(); ++r) {
      for (int c = 0; c < m_level->numCols(); ++c) {
        drawBlock(r, c);
      }
    }
#else
    for (int r = 0; r < m_level->numRows(); ++r) {
      for (int c = 0; c < m_level->numCols(); ++c) {
        if (m_level->getBlock(r, c) == Block::NONE) {
          glEnable(GL_BLEND);
        } else {
          glDisable(GL_BLEND);
        }
        drawBlock(r, c);
      }
    }
#endif
    drawBite();
    drawBall();

    if (m_render_explosion) {
      for (auto& item : m_explosion_packages) {
        drawExplosion(
            item.getX(),
            item.getY(),
            item.getColor(),
            item.getKind());
      }
    }

    drawPrizeCatch(0.f,0.f,util::MIDAS);

    clearRemovedPrizes();
    for (auto& item : m_prize_packages) {
      drawPrize(item.second);
    }

    eglSwapInterval(m_egl_display, 0);
    eglSwapBuffers(m_egl_display, m_egl_surface);
  }
}

void AsyncContext::delay(int ms) {
  for (int i = 0; i < ms; ++i) {
    render();
    std::this_thread::sleep_for (std::chrono::milliseconds(ProcessorParams::milliDelay));
  }
}

void AsyncContext::initParticleSystem() {
  for (int i = 0; i < particleSystemSize; ++i) {
    int index = i * particleSize;
    // Lifetime of particle
    m_particle_buffer[index + 0] = m_particle_distribution(m_generator);
    // Start position of particle
    m_particle_buffer[index + 4] = m_particle_distribution(m_generator) * 0.25f - 0.125f;
    m_particle_buffer[index + 5] = (m_particle_distribution(m_generator) * 0.25f - 0.125f) * m_aspect;
    m_particle_buffer[index + 6] = 0.0f;
    // End position of particle
    m_particle_buffer[index + 1] = m_particle_distribution(m_generator) * 2.0f - 1.0f;
    m_particle_buffer[index + 2] = (m_particle_distribution(m_generator) * 2.0f - 1.0f) * m_aspect;
    m_particle_buffer[index + 3] = 0.0f;
  }

  // --------------------------------------------
  GLfloat step = 0.001f;
  GLfloat halfStep = 0.5f * step;
  // branch 0
  {
    int bi = 0 * particleSpiralSize * particleSpiralSystemBranchSize;
    int i = 0;
    for (; i < particleSpiralSystemBranchSize >> 1; ++i) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = -halfStep * i;
      m_particle_spiral_buffer[index + 4 + bi] = step * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = step * i;
      m_particle_spiral_buffer[index + 1 + bi] = halfStep * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
    for (int j = -particleSpiralSystemBranchSize >> 1; i < particleSpiralSystemBranchSize; ++i, ++j) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = halfStep * j;
      m_particle_spiral_buffer[index + 4 + bi] = step * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = step * i;
      m_particle_spiral_buffer[index + 1 + bi] = -halfStep * j;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
  }
  // branch 1
  {
    int bi = 1 * particleSpiralSize * particleSpiralSystemBranchSize;
    int i = 0;
    for (; i < particleSpiralSystemBranchSize >> 1; ++i) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = step * i;
      m_particle_spiral_buffer[index + 4 + bi] = halfStep * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = halfStep * i;
      m_particle_spiral_buffer[index + 1 + bi] = -step * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
    for (int j = -particleSpiralSystemBranchSize >> 1; i < particleSpiralSystemBranchSize; ++i, ++j) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = step * i;
      m_particle_spiral_buffer[index + 4 + bi] = -halfStep * j;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = -halfStep * j;
      m_particle_spiral_buffer[index + 1 + bi] = -step * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
  }
  // branch 2
  {
    int bi = 2 * particleSpiralSize * particleSpiralSystemBranchSize;
    int i = 0;
    for (; i < particleSpiralSystemBranchSize >> 1; ++i) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = halfStep * i;
      m_particle_spiral_buffer[index + 4 + bi] = -step * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = -step * i;
      m_particle_spiral_buffer[index + 1 + bi] = -halfStep * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
    for (int j = -particleSpiralSystemBranchSize >> 1; i < particleSpiralSystemBranchSize; ++i, ++j) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = -halfStep * j;
      m_particle_spiral_buffer[index + 4 + bi] = -step * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = -step * i;
      m_particle_spiral_buffer[index + 1 + bi] = halfStep * j;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
  }
  // branch 3
  {
    int bi = 3 * particleSpiralSize * particleSpiralSystemBranchSize;
    int i = 0;
    for (; i < particleSpiralSystemBranchSize >> 1; ++i) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = -step * i;
      m_particle_spiral_buffer[index + 4 + bi] = -halfStep * i;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = -halfStep * i;
      m_particle_spiral_buffer[index + 1 + bi] = step * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
    for (int j = -particleSpiralSystemBranchSize >> 1; i < particleSpiralSystemBranchSize; ++i, ++j) {
      int index = i * particleSpiralSize;
      // Start position of particle
      m_particle_spiral_buffer[index + 3 + bi] = -step * i;
      m_particle_spiral_buffer[index + 4 + bi] = halfStep * j;
      m_particle_spiral_buffer[index + 5 + bi] = 0.0f;
      // End position of particle
      m_particle_spiral_buffer[index + 0 + bi] = halfStep * j;
      m_particle_spiral_buffer[index + 1 + bi] = step * i;
      m_particle_spiral_buffer[index + 2 + bi] = 0.0f;
    }
  }
}

/* Drawings group */
// ----------------------------------------------------------------------------
void AsyncContext::drawLevel() {
  m_level_shader->useProgram();

  GLint a_position = glGetAttribLocation(m_level_shader->getProgram(), "a_position");
  GLint a_color = glGetAttribLocation(m_level_shader->getProgram(), "a_color");

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

  GLint a_position = glGetAttribLocation(m_level_shader->getProgram(), "a_position");
#if USE_TEXTURE
  GLint a_texCoord = glGetAttribLocation(m_level_shader->getProgram(), "a_texCoord");
#else
  GLint a_color = glGetAttribLocation(m_level_shader->getProgram(), "a_color");
#endif

  int rci = col * 16 + row * m_level->numCols() * 16;
  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_level_vertex_buffer[rci]);
#if USE_TEXTUREm_program
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

  GLint a_position = glGetAttribLocation(m_bite_shader->getProgram(), "a_position");
  GLint a_color = glGetAttribLocation(m_bite_shader->getProgram(), "a_color");

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_bite_vertex_buffer[0]);
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_bite_color_buffer[0]);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_color);
  glDisable(GL_BLEND);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &m_rectangle_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_color);
}

void AsyncContext::drawBall() {
  m_ball_shader->useProgram();

  GLint a_position = glGetAttribLocation(m_ball_shader->getProgram(), "a_position");
  GLint a_color = glGetAttribLocation(m_ball_shader->getProgram(), "a_color");

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_ball_vertex_buffer[0]);
  glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, 0, &m_ball_color_buffer[0]);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_color);
  glDisable(GL_BLEND);

  glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, &m_octagon_index_buffer[0]);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_color);
}

void AsyncContext::drawExplosion(GLfloat x, GLfloat y, const util::BGRA<GLfloat>& bgra, Kind kind) {
  m_explosion_shader->useProgram();

  if (m_last_time == 0) {
    m_last_time = clock();
  }
  {
    clock_t currentTime = clock();
    float delta_elapsed = static_cast<float>(currentTime - m_last_time) / CLOCKS_PER_SEC;
    m_last_time = currentTime;
    m_particle_time += delta_elapsed;
    if (m_particle_time >= 1.0f) {
      m_particle_time = 0.0f;
      m_render_explosion = false;
      m_explosion_packages.clear();
      return;
    }
  }

  GLint u_time = glGetUniformLocation(m_explosion_shader->getProgram(), "u_time");
  GLint u_centerPosition = glGetUniformLocation(m_explosion_shader->getProgram(), "u_centerPosition");
  GLint u_color = glGetUniformLocation(m_explosion_shader->getProgram(), "u_color");

  GLfloat* coord = new GLfloat[3]{x, y, 0.0f};
  GLfloat* color = new GLfloat[4]{bgra.b, bgra.g, bgra.r, 0.5f};
  glUniform3fv(u_centerPosition, 1, &coord[0]);
  glUniform4fv(u_color, 1, &color[0]);
  glUniform1f(u_time, m_particle_time);

  GLint a_lifetime = glGetAttribLocation(m_explosion_shader->getProgram(), "a_lifetime");
  GLint a_startPosition = glGetAttribLocation(m_explosion_shader->getProgram(), "a_startPosition");
  GLint a_endPosition = glGetAttribLocation(m_explosion_shader->getProgram(), "a_endPosition");

  glVertexAttribPointer(a_lifetime, 1, GL_FLOAT, GL_FALSE, particleSize * sizeof(GLfloat), &m_particle_buffer[0]);
  glVertexAttribPointer(a_startPosition, 3, GL_FLOAT, GL_FALSE, particleSize * sizeof(GLfloat), &m_particle_buffer[4]);
  glVertexAttribPointer(a_endPosition, 3, GL_FLOAT, GL_FALSE, particleSize * sizeof(GLfloat), &m_particle_buffer[1]);

  m_resources->getTexture("smoke.png")->apply();
  GLint sampler = glGetUniformLocation(m_explosion_shader->getProgram(), "s_texture");
  glUniform1i(sampler, 0);

  glEnableVertexAttribArray(a_lifetime);
  glEnableVertexAttribArray(a_startPosition);
  glEnableVertexAttribArray(a_endPosition);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_POINTS, 0, particleSystemSize);

  delete [] coord;
  delete [] color;

  glDisableVertexAttribArray(a_lifetime);
  glDisableVertexAttribArray(a_startPosition);
  glDisableVertexAttribArray(a_endPosition);
}

void AsyncContext::drawBackground() {
  m_sample_shader->useProgram();

  GLint a_position = glGetAttribLocation(m_sample_shader->getProgram(), "a_position");
  GLint a_texCoord = glGetAttribLocation(m_sample_shader->getProgram(), "a_texCoord");

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &m_bg_vertex_buffer[0]);
  glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, &m_rectangle_texCoord_buffer[0]);

  m_bg_texture->apply();
  GLint sampler = glGetUniformLocation(m_sample_shader->getProgram(), "s_texture");
  glUniform1i(sampler, 0);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_texCoord);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_texCoord);
}

void AsyncContext::drawPrize(const PrizePackage& prize) {
  m_prize_shader->useProgram();

  if (m_prize_last_time == 0) {
    m_prize_last_time = clock();
  }
  {
    clock_t currentTime = clock();
    float delta_elapsed = static_cast<float>(currentTime - m_prize_last_time) / CLOCKS_PER_SEC;
    m_prize_last_time = currentTime;
    m_prize_time += delta_elapsed;
    if (m_prize_time >= 3.0f) {
      m_prize_time = 0.0f;
      return;
    }
  }
  int is_visible = 1;  /* true */
  {
    GLfloat Ypath = prize.getY() - m_prize_time * PrizeParams::prizeSpeed;
    if (Ypath >= -BiteParams::neg_biteElevation) {
      PrizePackage moved_prize = prize;
      moved_prize.setY(Ypath);
      prize_location_event.notifyListeners(moved_prize);
    } else if (Ypath < -BiteParams::neg_biteElevation && Ypath > -1.0f) {
      prize_gone_event.notifyListeners(prize.getID());
    } else {
      is_visible = 0;  /* false */
      addPrizeToRemoved(prize.getID());
    }
  }

  GLint u_time = glGetUniformLocation(m_prize_shader->getProgram(), "u_time");
  GLint u_velocity = glGetUniformLocation(m_prize_shader->getProgram(), "u_velocity");
  GLint u_visible = glGetUniformLocation(m_prize_shader->getProgram(), "u_visible");
  glUniform1f(u_time, m_prize_time);
  glUniform1f(u_velocity, PrizeParams::prizeSpeed);
  glUniform1i(u_visible, is_visible);

  GLint a_position = glGetAttribLocation(m_prize_shader->getProgram(), "a_position");
  GLint a_texCoord = glGetAttribLocation(m_prize_shader->getProgram(), "a_texCoord");

  GLfloat* prize_vertices = new GLfloat[16];
  util::setRectangleVertices(
      prize_vertices,
      PrizeParams::prizeWidth,
      PrizeParams::prizeHeight * m_aspect,
      prize.getX() - PrizeParams::prizeHalfWidth,
      prize.getY() - PrizeParams::prizeHalfHeight,
      1, 1);

  glVertexAttribPointer(a_position, 4, GL_FLOAT, GL_FALSE, 0, &prize_vertices[0]);
  glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, &m_rectangle_texCoord_buffer[0]);

  m_resources->getPrizeTexture(prize.getPrize())->apply();
  GLint sampler = glGetUniformLocation(m_prize_shader->getProgram(), "s_texture");
  glUniform1i(sampler, 0);

  glEnableVertexAttribArray(a_position);
  glEnableVertexAttribArray(a_texCoord);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  delete [] prize_vertices;

  glDisableVertexAttribArray(a_position);
  glDisableVertexAttribArray(a_texCoord);
}

void AsyncContext::drawPrizeCatch(GLfloat x, GLfloat y, const util::BGRA<GLfloat>& bgra) {
  m_prize_catch_shader->useProgram();

  if (m_prize_catch_last_time == 0) {
    m_prize_catch_last_time = clock();
  }
  {
    clock_t currentTime = clock();
    float delta_elapsed = static_cast<float>(currentTime - m_prize_catch_last_time) / CLOCKS_PER_SEC;
    m_prize_catch_last_time = currentTime;
    m_prize_catch_time += delta_elapsed;
    if (m_prize_catch_time >= 3.0f) {
      m_prize_catch_time = 0.0f;
//      return;
    }
  }

  GLint u_time = glGetUniformLocation(m_prize_catch_shader->getProgram(), "u_time");
  GLint u_centerPosition = glGetUniformLocation(m_prize_catch_shader->getProgram(), "u_centerPosition");
  GLint u_color = glGetUniformLocation(m_prize_catch_shader->getProgram(), "u_color");

  GLfloat* coord = new GLfloat[3]{x, y, 0.0f};
  GLfloat* color = new GLfloat[4]{bgra.b, bgra.g, bgra.r, 0.5f};
  glUniform3fv(u_centerPosition, 1, &coord[0]);
  glUniform4fv(u_color, 1, &color[0]);
  glUniform1f(u_time, m_prize_catch_time);

  GLint a_startPosition = glGetAttribLocation(m_prize_catch_shader->getProgram(), "a_startPosition");
  GLint a_endPosition = glGetAttribLocation(m_prize_catch_shader->getProgram(), "a_endPosition");

  glVertexAttribPointer(a_startPosition, 3, GL_FLOAT, GL_FALSE, particleSpiralSize * sizeof(GLfloat), &m_particle_spiral_buffer[3]);
  glVertexAttribPointer(a_endPosition, 3, GL_FLOAT, GL_FALSE, particleSpiralSize * sizeof(GLfloat), &m_particle_spiral_buffer[0]);

  m_resources->getTexture("spark.png")->apply();
  GLint sampler = glGetUniformLocation(m_prize_catch_shader->getProgram(), "s_texture");
  glUniform1i(sampler, 0);

  glEnableVertexAttribArray(a_startPosition);
  glEnableVertexAttribArray(a_endPosition);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_POINTS, 0, particleSpiralSystemSize);

  delete [] coord;
  delete [] color;

  glDisableVertexAttribArray(a_startPosition);
  glDisableVertexAttribArray(a_endPosition);
}

}  // namespace game

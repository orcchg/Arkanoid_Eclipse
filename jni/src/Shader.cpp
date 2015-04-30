#include <GLES2/gl2.h>

#include "Exceptions.h"
#include "logger.h"
#include "Shader.h"

namespace shader {

ShaderHelper::ShaderHelper(const Shader& shader)
  : m_program(0)
  , m_vertex_location(0)
  , m_color_location(1) {

  DBG("enter ShaderHelper::ctor");
  GLuint vertex_shader = loadShader(GL_VERTEX_SHADER, shader.vertex);
  GLuint fragment_shader = loadShader(GL_FRAGMENT_SHADER, shader.fragment);

  m_program = glCreateProgram();
  if (m_program == 0) {
    const char* message = "Failed to create program object";
    ERR("%s", message);
    throw ShaderException(message);
  }

  glAttachShader(m_program, vertex_shader);
  glAttachShader(m_program, fragment_shader);

  // bind a_position to attribute with index m_vertex_location
  glBindAttribLocation(m_program, m_vertex_location, "a_position");
  glBindAttribLocation(m_program, m_color_location, "a_color");
  glBindAttribLocation(m_program, m_texCoord_location, "a_texCoord");
  glLinkProgram(m_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  GLint linked = 0;
  glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLint infoLen = 0;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 1) {
      char* infoLog = new char[infoLen];
      glGetProgramInfoLog(m_program, infoLen, nullptr, infoLog);
      ERR("Error linking program:\n%s\n", infoLog);
      delete [] infoLog;
    }
    glDeleteProgram(m_program);
    throw ShaderException("Error linking program");
  }
  DBG("exit ShaderHelper::ctor");
}

ShaderHelper::~ShaderHelper() {
  glDeleteProgram(m_program);
  m_program = 0;
}

void ShaderHelper::useProgram() const {
  glUseProgram(m_program);
}

GLuint ShaderHelper::loadShader(GLenum type, const char* shader_src) {
  DBG("enter ShaderHelper::loadShader()");
  INF("GL version: %s", glGetString(GL_VERSION));
  GLuint shader = glCreateShader(type);
  if (shader == 0) {
    ERR("Failed to create shader of type %i", type);
    return 0;
  }

  glShaderSource(shader, 1, &shader_src, nullptr);  // load shader source
  glCompileShader(shader);

  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);  // get status
  if (compiled == 0) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 1) {
      char* infoLog = new char[infoLen];
      glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
      ERR("Error compiling shader:\n%s\n", infoLog);
      delete [] infoLog;
    }
    glDeleteShader(shader);
    return 0;
  }
  DBG("exit ShaderHelper::loadShader()");
  return shader;
}

/* Pre-made shaders */
// ----------------------------------------------------------------------------
Shader::Shader(const char* vertex, const char* fragment)
  : vertex(vertex)
  , fragment(fragment) {
}

Shader::~Shader() {
}

SimpleShader::SimpleShader()
  : Shader(
      "  attribute vec4 a_position;   \n"
      "  attribute vec4 a_color;      \n"
      "                               \n"
      "  varying vec4 v_color;        \n"
      "                               \n"
      "  void main() {                \n"
      "    v_color = a_color;         \n"
      "    gl_Position = a_position;  \n"
      "  }                            \n"
      ,
      "  precision mediump float;     \n"
      "                               \n"
      "  varying vec4 v_color;        \n"
      "                               \n"
      "  void main() {                \n"
      "    gl_FragColor = v_color;    \n"
      "  }                            \n") {
}

SimpleTextureShader::SimpleTextureShader()
  : Shader(
      "  attribute vec4 a_position;   \n"
      "  attribute vec2 a_texCoord;   \n"
      "                               \n"
      "  varying vec2 v_texCoord;     \n"
      "                               \n"
      "  void main() {                \n"
      "    v_texCoord = a_texCoord;   \n"
      "    gl_Position = a_position;  \n"
      "  }                            \n"
      ,
      "  precision mediump float;     \n"
      "                               \n"
      "  varying vec2 v_texCoord;     \n"
      "  uniform sampler2D s_texture; \n"
      "                               \n"
      "  void main() {                \n"
      "    gl_FragColor = texture2D(s_texture, v_texCoord); \n"
      "  }                            \n") {
}

}

#ifndef __ARKANOID_SHADER__H__
#define __ARKANOID_SHADER__H__

#include <memory>

#include <GLES/gl.h>
#include <GLES2/gl2.h>

namespace shader {

struct Shader;

/**
 * @class ShaderHelper Shader.h "include/Shader.h"
 * @brief Helper class to load shaders and compile program object.
 */
class ShaderHelper {
public:
  typedef std::shared_ptr<ShaderHelper> Ptr;

  ShaderHelper(const Shader& shader);
  virtual ~ShaderHelper() noexcept;

  void useProgram() const;

  inline GLuint getVertexAttribLocation() const {
    return glGetAttribLocation(m_program, "a_position");  // m_vertex_location;
  }

  inline GLuint getColorAttribLocation() const {
    return glGetAttribLocation(m_program, "a_color");  // m_color_location;
  }

  inline GLuint getTexCoordAttribLocation() const {
    return glGetAttribLocation(m_program, "a_texCoord");  // m_texCoord_location;
  }

  GLint getSampler2DUniformLocation() const;

private:
  GLuint m_program;  //!< Linked program.
  GLuint m_vertex_location;  //!< Location of vertex attribute.
  GLuint m_color_location;  //!< Location of color attribute.
  GLuint m_texCoord_location;  //!< LocatbindColorAttribLocationion of texCoord attribute.

  GLuint loadShader(GLenum type, const char* shader_src);
};

/* Pre-made shaders */
// ----------------------------------------------------------------------------
struct Shader {
  friend class ShaderHelper;
public:
  Shader(const char* vertex, const char* fragment);
  virtual ~Shader();

protected:
  const char* vertex;
  const char* fragment;

protected:
  virtual void bindColorAttribLocation(GLuint program, GLuint color_location) const = 0;
  virtual void bindTexCoordAttribLocation(GLuint program, GLuint texCoord_location) const = 0;
};

struct SimpleShader : public Shader {
  SimpleShader();

  void bindColorAttribLocation(GLuint program, GLuint color_location) const override final;
  void bindTexCoordAttribLocation(GLuint program, GLuint texCoord_location) const override final;
};

struct SimpleTextureShader : public Shader {
  SimpleTextureShader();

  void bindColorAttribLocation(GLuint program, GLuint color_location) const override final;
  void bindTexCoordAttribLocation(GLuint program, GLuint texCoord_location) const override final;
};

}

#endif  // __ARKANOID_SHADER__H__

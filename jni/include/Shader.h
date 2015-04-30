#ifndef __ARKANOID_SHADER__H__
#define __ARKANOID_SHADER__H__

#include <memory>

#include <GLES/gl.h>

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
    return m_vertex_location;
  }

  inline GLuint getColorAttribLocation() const {
    return m_color_location;
  }

  inline GLuint getTexCoordAttribLocation() const {
    return m_texCoord_location;
  }

private:
  GLuint m_program;  //!< Linked program.
  GLuint m_vertex_location;  //!< Location of vertex attribute.
  GLuint m_color_location;  //!< Location of color attribute.
  GLuint m_texCoord_location;  //!< Location of texCoord attribute.

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
};

struct SimpleShader : public Shader {
  SimpleShader();
};

struct SimpleTextureShader : public Shader {
  SimpleTextureShader();
};

}

#endif  // __ARKANOID_SHADER__H__

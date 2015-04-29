#ifndef __ARKANOID_RESOURCES__H__
#define __ARKANOID_RESOURCES__H__

#include <jni.h>
#include <string>
#include <unordered_map>

#include "Level.h"
#include "Texture.h"

namespace game {

class Resources {
public:
  Resources(JNIEnv* jenv, jlong assets);
  ~Resources() noexcept;

  bool read(jstring filename);
  const native::Texture* const getTexture(const std::string& name) const;

private:
  JNIEnv* m_jenv;
  AssetStorage* m_assets;
  std::unordered_map<std::string, native::Texture*> m_textures;
};

}

#endif  // __ARKANOID_RESOURCES__H__

#include "Resources.h"

namespace game {

Resources::Resources(JNIEnv* jenv, jlong assets)
  : m_jenv(jenv)
  , m_assets(reinterpret_cast<AssetStorage*>(assets)) {
}

Resources::~Resources() {
  for (auto& item : m_textures) {
    delete item.second;
    item.second = nullptr;
  }
}

bool Resources::read(jstring filename) {
  const char* raw_name = m_jenv->GetStringUTFChars(filename, nullptr);
  native::Texture* texture = new native::PNGTexture(m_assets, raw_name);
  m_jenv->ReleaseStringUTFChars(filename, raw_name);
  m_textures[raw_name] = texture;
  return true;
}

const native::Texture* const Resources::getTexture(const std::string& name) const {
  return m_textures.at(name);
}

}

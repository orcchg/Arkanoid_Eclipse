#include "logger.h"
#include "Resources.h"

/* Init */
// ----------------------------------------------------------------------------
JNIEXPORT jlong JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_init
  (JNIEnv *jenv, jobject, jobject assets) {
  auto ptr = new game::Resources(jenv, assets);
  jlong descriptor = (jlong)(intptr_t) ptr;
  return descriptor;
}

JNIEXPORT jboolean JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_read
  (JNIEnv *, jobject, jlong descriptor, jstring filename) {
  game::Resources* ptr = reinterpret_cast<game::Resources*>(descriptor);
  return ptr->read(filename);
}

JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_release
  (JNIEnv *, jobject, jlong descriptor) {
  game::Resources* ptr = reinterpret_cast<game::Resources*>(descriptor);
  delete ptr;
  ptr = nullptr;
}

/* Core */
// ----------------------------------------------------------------------------
namespace game {

Resources::Resources(JNIEnv* jenv, jobject assets)
  : m_jenv(jenv)
  , m_assets(new AssetStorage(m_jenv, assets)) {
}

Resources::~Resources() {
  delete m_assets;
  m_assets = nullptr;
  for (auto& item : m_textures) {
    delete item.second;
    item.second = nullptr;
  }
  m_jenv = nullptr;
}

bool Resources::read(jstring filename) {
  const char* raw_name = m_jenv->GetStringUTFChars(filename, nullptr);
  native::Texture* texture = new native::PNGTexture(m_assets, raw_name);
  DBG("Read resource: %s", raw_name);
  m_textures[raw_name] = texture;
  m_jenv->ReleaseStringUTFChars(filename, raw_name);
  return true;
}

const native::Texture* const Resources::getTexture(const std::string& name) const {
  return m_textures.at(name);
}

const native::Texture* const Resources::getRandomTexture(const std::string& prefix) const {
  native::Texture* texture = nullptr;
  bool success = false;
  do {
    size_t random_index = std::rand() % m_textures.size();
    auto shift = m_textures.begin();
    for (int i = 0; i < random_index; ++i) {
      ++shift;
    }
    success = (shift->first.find(prefix) == 0);
    texture = shift->second;
  } while (!success);
  return texture;
}

const native::Texture* const Resources::getPrizeTexture(const Prize& prize) const {
  native::Texture* texture = nullptr;
  switch (prize) {
    // TODO: distinguish textures by prize type
    case Prize::BLOCK:
    case Prize::CLIMB:
    case Prize::DESTROY:
    case Prize::DRAGON:
    case Prize::EASY:
    case Prize::EASY_T:
    case Prize::EVAPORATE:
    case Prize::EXPLODE:
    case Prize::EXTEND:
    case Prize::FAST:
    case Prize::FOG:
    case Prize::GOO:
    case Prize::HYPER:
    case Prize::INIT:
    case Prize::JUMP:
    case Prize::LASER:
    case Prize::MIRROR:
    case Prize::PIERCE:
    case Prize::PROTECT:
    case Prize::RANDOM:
    case Prize::SHORT:
    case Prize::SLOW:
    case Prize::UPGRADE:
    case Prize::DEGRADE:
    case Prize::VITALITY:
    case Prize::WIN:
    case Prize::ZYGOTE:
      texture = const_cast<native::Texture*>(getTexture("pr_star.png"));
      break;
    default:
    case Prize::NONE:  // NONE prize is ignored by GameProcessor
      break;
  }
  return texture;
}

Resources::iterator Resources::begin() {
  return m_textures.begin();
}
Resources::iterator Resources::end() {
  return m_textures.end();
}

Resources::const_iterator Resources::cbegin() const {
  return m_textures.cbegin();
}

Resources::const_iterator Resources::cend() const {
  return m_textures.cend();
}

Resources::Ptr Resources::getSharedPtr() {
  return shared_from_this();
}

}

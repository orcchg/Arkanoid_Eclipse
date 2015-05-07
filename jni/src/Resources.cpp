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
  switch (prize) {
    // TODO: distinguish textures by prize type
    case Prize::BLOCK:
      return getTexture("pr_brick.png");
    case Prize::CLIMB:
      return getTexture("pr_arrow.png");
    case Prize::DESTROY:
      return getTexture("pr_skull.png");
    case Prize::DRAGON:
    case Prize::EASY:
    case Prize::EASY_T:
    case Prize::EVAPORATE:
      return getTexture("pr_waterdrop.png");
    case Prize::EXPLODE:
      return getTexture("pr_explode.png");
    case Prize::EXTEND:
    case Prize::FAST:
    case Prize::FOG:
    case Prize::GOO:
      return getTexture("pr_glue.png");
    case Prize::HYPER:
    case Prize::INIT:
    case Prize::JUMP:
    case Prize::LASER:
      return getTexture("pr_laser.png");
    case Prize::MIRROR:
      return getTexture("pr_mirror.png");
    case Prize::PIERCE:
    case Prize::PROTECT:
    case Prize::RANDOM:
    case Prize::SHORT:
    case Prize::SLOW:
    case Prize::UPGRADE:
    case Prize::DEGRADE:
      return getTexture("pr_star.png");
    case Prize::VITALITY:
      return getTexture("pr_ball.png");
    case Prize::WIN:
    case Prize::ZYGOTE:
      return getTexture("pr_star.png");
    default:
    case Prize::NONE:  // NONE prize is ignored by GameProcessor
      break;
  }
  return nullptr;
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

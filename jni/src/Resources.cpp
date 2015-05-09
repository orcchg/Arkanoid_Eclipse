#include "logger.h"
#include "Resources.h"

/* Init */
// ----------------------------------------------------------------------------
JNIEXPORT jlong JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_init
  (JNIEnv *jenv, jobject, jobject assets, jstring internalFileStorage_Java) {
  auto ptr = new game::Resources(jenv, assets, internalFileStorage_Java);
  jlong descriptor = (jlong)(intptr_t) ptr;
  return descriptor;
}

JNIEXPORT jboolean JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_readTexture
  (JNIEnv *, jobject, jlong descriptor, jstring filename) {
  game::Resources* ptr = reinterpret_cast<game::Resources*>(descriptor);
  return ptr->readTexture(filename);
}

JNIEXPORT jboolean JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_readSound
  (JNIEnv *, jobject, jlong descriptor, jstring filename) {
  game::Resources* ptr = reinterpret_cast<game::Resources*>(descriptor);
  return ptr->readSound(filename);
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

Resources::Resources(JNIEnv* jenv, jobject assets, jstring internalFileStorage_Java)
  : m_jenv(jenv)
  , m_assets(new AssetStorage(m_jenv, assets)) {
  const char* internal_file_storage = jenv->GetStringUTFChars(internalFileStorage_Java, 0);
  m_assets->setInternalFileStorage(internal_file_storage);
  jenv->ReleaseStringUTFChars(internalFileStorage_Java, internal_file_storage);
}

Resources::~Resources() {
  delete m_assets;
  m_assets = nullptr;
  for (auto& item : m_textures) {
    delete item.second;
    item.second = nullptr;
  }
  m_textures.clear();
  for (auto& item: m_sounds) {
    delete item.second;
    item.second = nullptr;
  }
  m_sounds.clear();
  m_jenv = nullptr;
}

/* Texture group */
// ----------------------------------------------------------------------------
bool Resources::readTexture(jstring filename) {
  const char* raw_name = m_jenv->GetStringUTFChars(filename, nullptr);
  native::Texture* texture = nullptr;
  {
    std::string prefix = "texture/" + std::string(raw_name);
    texture = new native::PNGTexture(m_assets, prefix.c_str());
    DBG("Read texture resource: %s", raw_name);
  }
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
      return getTexture("pr_candy.png");
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

Resources::tex_iterator Resources::beginTexture() { return m_textures.begin(); }
Resources::tex_iterator Resources::endTexture() { return m_textures.end(); }
Resources::const_tex_iterator Resources::cbeginTexture() const { return m_textures.cbegin(); }
Resources::const_tex_iterator Resources::cendTexture() const { return m_textures.cend(); }

Resources::Ptr Resources::getSharedPtr() {
  return shared_from_this();
}

/* Sound group */
// ----------------------------------------------------------------------------
bool Resources::readSound(jstring filename) {
  const char* raw_name = m_jenv->GetStringUTFChars(filename, nullptr);
  native::SoundBuffer* sound = nullptr;
  {
    std::string prefix = "sound/" + std::string(raw_name);
    sound = new native::WAVSound(m_assets, prefix.c_str());
    DBG("Read sound resource: %s", raw_name);
  }
  m_sounds[raw_name] = sound;
  m_jenv->ReleaseStringUTFChars(filename, raw_name);
  return true;
}

Resources::sound_iterator Resources::beginSound() { return m_sounds.begin(); }
Resources::sound_iterator Resources::endSound() { return m_sounds.end(); }
Resources::const_sound_iterator Resources::cbeginSound() const { return m_sounds.cbegin(); }
Resources::const_sound_iterator Resources::cendSound() const { return m_sounds.cend(); }

}

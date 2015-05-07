#ifndef __ARKANOID_RESOURCES__H__
#define __ARKANOID_RESOURCES__H__

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_orcchg_arkanoid_surface_NativeResources */

#ifndef _Included_com_orcchg_arkanoid_surface_NativeResources
#define _Included_com_orcchg_arkanoid_surface_NativeResources
#ifdef __cplusplus
extern "C" {
#endif

/* Init */
// ----------------------------------------------------------------------------
/*
 * Class:     com_orcchg_arkanoid_surface_NativeResources
 * Method:    init
 * Signature: (Landroid/content/res/AssetManager;)J
 */
JNIEXPORT jlong JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_init
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_orcchg_arkanoid_surface_NativeResources
 * Method:    read
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_read
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_orcchg_arkanoid_surface_NativeResources
 * Method:    release
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_orcchg_arkanoid_surface_NativeResources_release
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif

/* Core */
// ----------------------------------------------------------------------------
#include <memory>
#include <string>
#include <unordered_map>
#include <cstdlib>

#include "Level.h"
#include "Prize.h"
#include "Texture.h"

namespace game {

class Resources : public std::enable_shared_from_this<Resources> {
public:
  typedef std::shared_ptr<Resources> Ptr;
  typedef std::unordered_map<std::string, native::Texture*>::iterator iterator;
  typedef std::unordered_map<std::string, native::Texture*>::const_iterator const_iterator;

  Resources(JNIEnv* jenv, jobject assets);
  ~Resources() noexcept;

  bool read(jstring filename);
  const native::Texture* const getTexture(const std::string& name) const;
  const native::Texture* const getRandomTexture(const std::string& prefix) const;
  const native::Texture* const getPrizeTexture(const Prize& prize) const;

  iterator begin();
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;

  Ptr getSharedPtr();

private:
  JNIEnv* m_jenv;
  AssetStorage* m_assets;
  std::unordered_map<std::string, native::Texture*> m_textures;
};

}

#endif  // __ARKANOID_RESOURCES__H__

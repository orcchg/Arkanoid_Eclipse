#ifndef __ARKANOID_RESOURCES__H__
#define __ARKANOID_RESOURCES__H__

#include <jni.h>

#include "Level.h"
#include "Texture.h"

namespace game {

class Resources {
public:
  Resources(jlong assets);
  ~Resources() noexcept;

private:
  AssetStorage* m_assets;
};

}

#endif  // __ARKANOID_RESOURCES__H__

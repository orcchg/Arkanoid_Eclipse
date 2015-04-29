#include "Resources.h"

namespace game {

Resources::Resources(jlong assets)
  : m_assets(reinterpret_cast<AssetStorage*>(assets)) {
}

Resources::~Resources() {
}

}

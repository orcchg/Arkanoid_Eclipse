#ifndef __ARKANOID_SOUND_BUFFER__H__
#define __ARKANOID_SOUND_BUFFER__H__

#include "AssetStorage.h"

namespace native {

class SoundBuffer {
public:
  SoundBuffer(AssetStorage* assets, const char* filename);
  SoundBuffer(const char* filepath);
  virtual ~SoundBuffer();

  const char* getFilename() const;
  const char* getName() const;

  virtual bool load();
  virtual void unload();

protected:
  virtual uint8_t* loadSound() = 0;

  enum class ReadMode : int {
    ASSETS = 0, FILESYSTEM = 1
  };

  ReadMode m_read_mode;
  AssetStorage* m_assets;
  char* m_filename;
  off_t m_length;
  uint8_t* m_data;
};

// ----------------------------------------------------------------------------
/// @brief Class allows to operate with WAV and PCM files.
class WAVSound : public SoundBuffer {
public:
  WAVSound(AssetStorage* assets, const char* filename);
  WAVSound(const char* filepath);
  virtual ~WAVSound();

protected:
  uint8_t* loadSound() override final;
};

}

#endif  // __ARKANOID_SOUND_BUFFER__H__

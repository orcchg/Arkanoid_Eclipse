#include <algorithm>
#include <cstdio>
#include <cstring>
#include <libgen.h>

#include "logger.h"
#include "SoundBuffer.h"

namespace native {

SoundBuffer::SoundBuffer(AssetStorage* assets, const char* filename)
  : m_read_mode(ReadMode::ASSETS)
  , m_assets(assets)
  , m_filename(new char[128])
  , m_length(0)
  , m_data(nullptr) {
}

SoundBuffer::SoundBuffer(const char* filepath)
  : m_read_mode(ReadMode::FILESYSTEM)
  , m_assets(nullptr)
  , m_filename(new char[128])
  , m_length(0)
  , m_data(nullptr) {
}

SoundBuffer::~SoundBuffer() {
  m_assets = nullptr;
  delete [] m_filename;  m_filename = nullptr;
  unload();
}

const char* SoundBuffer::getFilename() const { return m_filename; }

const char* SoundBuffer::getName() const {
  if (m_filename != nullptr) {
    std::string lower = m_filename;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return basename(lower.c_str());
  }
  return nullptr;
}

bool SoundBuffer::load() {
  m_data = loadSound();
  if (m_data == nullptr) {
    ERR("Internal error during loading sound!");
    return false;
  }
  return true;
}

void SoundBuffer::unload() {
  delete [] m_data;  m_data = nullptr;
  m_length = 0;
}

uint8_t* SoundBuffer::loadSound() {
  uint8_t* data = nullptr;
  FILE* file_descriptor = nullptr;
  int error_code = 0;

  switch (m_read_mode) {
    case ReadMode::ASSETS:
      if (!m_assets->open(m_filename)) { error_code = 1; goto ERROR_SOUND; }
      m_length = m_assets->length();
      data = new (std::nothrow) uint8_t(m_length);
      if (data == nullptr) { error_code = 2; goto ERROR_SOUND; }
      if (!m_assets->read(data, m_length)) { error_code = 3; goto ERROR_SOUND; }
      m_assets->close();
      break;
    case ReadMode::FILESYSTEM:
      file_descriptor = std::fopen(m_filename, "rb");
      if (file_descriptor == nullptr) { error_code = 4; goto ERROR_SOUND; }
      fseek(file_descriptor, 0, SEEK_END);
      m_length = ftell(file_descriptor);
      rewind(file_descriptor);
      data = new (std::nothrow) uint8_t(m_length);
      if (data == nullptr) { error_code = 5; goto ERROR_SOUND; }
      if (m_length != std::fread(data, 1, m_length, file_descriptor)) { error_code = 6; goto ERROR_SOUND; }
      std::fclose(file_descriptor);
      break;
  }
  return data;

  ERROR_SOUND:
    switch (m_read_mode) {
      case ReadMode::ASSETS:
        m_assets->close();
        break;
      case ReadMode::FILESYSTEM:
        std::fclose(file_descriptor);
        break;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------
WAVSound::WAVSound(AssetStorage* assets, const char* filename)
  : SoundBuffer(assets, filename) {
}

WAVSound::WAVSound(const char* filepath)
  : SoundBuffer(filepath) {
}

WAVSound::~WAVSound() {
}

uint8_t* WAVSound::loadSound() {
  return SoundBuffer::loadSound();
}

}

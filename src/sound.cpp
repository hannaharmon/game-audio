#define NOMINMAX  // Prevent Windows from defining min/max macros
#include "sound.h"
#include "audio_group.h"
#include "audio_manager.h"
#include "logging.h"
#include <iostream>
#include <algorithm>
#include <fstream>

namespace audio {

std::unique_ptr<Sound> Sound::Create(ma_engine* engine, const std::string& filepath, AudioGroup* group) {
  // Validate that file exists
  std::ifstream file(filepath, std::ios::binary);
  if (!file.good()) {
    throw FileLoadException("File not found or cannot be opened: " + filepath);
  }
  file.close();
  
  return std::unique_ptr<Sound>(new Sound(engine, filepath, group));
}

SoundInstance::SoundInstance() : sound(nullptr), finished(false) {}

SoundInstance::~SoundInstance() {
  if (sound) {
    ma_sound_uninit(sound);
    delete sound;
  }
}

Sound::Sound(ma_engine* engine, const std::string& filepath, AudioGroup* group) 
    : engine_(engine), 
      filepath_(filepath), 
      looping_(false), 
      volume_(1.0f),
      pitch_(1.0f),
      group_(group ? group->GetHandle() : nullptr),
      position_(0.0f, 0.0f, 0.0f),
      min_distance_(1.0f),
      max_distance_(1000.0f),
      rolloff_(1.0f),
      spatialization_enabled_(true) {
}

Sound::~Sound() {
  Stop();  // Stops and cleans up all instances
}

void Sound::CleanupFinishedInstances() {
  // Remove any instances that have finished playing
  auto it = sound_instances_.begin();
  while (it != sound_instances_.end()) {
    if ((*it)->sound && ma_sound_is_playing((*it)->sound) == MA_FALSE) {
      it = sound_instances_.erase(it);  // Smart pointer handles cleanup
    } else {
      ++it;
    }
  }
}

void Sound::Play() {
  Play(position_);  // Use default position
}

void Sound::Play(const Vec3& position) {
  // First cleanup any finished instances
  CleanupFinishedInstances();
  
  // If looping and already playing, don't create a new instance
  if (looping_ && !sound_instances_.empty()) {
    // Just ensure the existing instance is playing
    for (auto& instance : sound_instances_) {
      if (instance->sound && ma_sound_is_playing(instance->sound) == MA_FALSE) {
        ma_sound_start(instance->sound);
      }
    }
    return;
  }
  
  // Create new instance
  auto instance = std::make_unique<SoundInstance>();
  instance->sound = new ma_sound;
  
  // Stream large music files, don't stream small SFX for better sync
  // Use streaming for files in music group or looping files (likely music)
  uint32_t flags = (group_ != nullptr || looping_) ? MA_SOUND_FLAG_STREAM : 0;
  AUDIO_LOG(LogLevel::Info, "[Sound::Play] Loading sound file: " << filepath_ << " (streaming: " << (flags ? "yes" : "no") << ")");
  ma_result result = ma_sound_init_from_file(
      engine_,
      filepath_.c_str(),
      flags,
      group_,
      nullptr,
      instance->sound);
      
  if (result != MA_SUCCESS) {
    AUDIO_LOG(LogLevel::Error, "[Sound::Play] FAILED to load sound file: " << filepath_ << " (error code: " << result << ")");
    // Clean up the uninitialized sound before throwing
    // Set to nullptr so destructor doesn't try to uninit an uninitialized sound
    delete instance->sound;
    instance->sound = nullptr;
    // Instance will be cleaned up automatically by unique_ptr<SoundInstance> destructor
    throw FileLoadException("Failed to initialize sound playback for file: " + filepath_ + " (error code: " + std::to_string(result) + ")");
  }
  AUDIO_LOG(LogLevel::Info, "[Sound::Play] Successfully loaded: " << filepath_);
  
  // Configure and play
  ma_sound_set_looping(instance->sound, looping_);
  ma_sound_set_volume(instance->sound, volume_);
  ma_sound_set_pitch(instance->sound, pitch_);
  
  // Configure spatial audio - use the provided position for this instance only
  ma_sound_set_spatialization_enabled(instance->sound, spatialization_enabled_ ? MA_TRUE : MA_FALSE);
  if (spatialization_enabled_) {
    ma_sound_set_position(instance->sound, position.x, position.y, position.z);
    ma_sound_set_min_distance(instance->sound, min_distance_);
    ma_sound_set_max_distance(instance->sound, max_distance_);
    ma_sound_set_rolloff(instance->sound, rolloff_);
  }
  
  ma_sound_start(instance->sound);
  
  // Add to our list of instances
  sound_instances_.push_back(std::move(instance));
}

void Sound::Stop() {
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_stop(instance->sound);
    }
  }
  sound_instances_.clear();  // Smart pointers handle cleanup
}

void Sound::SetLooping(bool should_loop) {
  looping_ = should_loop;
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_looping(instance->sound, looping_);
    }
  }
}

void Sound::SetVolume(float volume) {
  // Clamp volume between 0 and 1
  volume_ = (std::min)(1.0f, (std::max)(0.0f, volume));
  
  AUDIO_LOG(LogLevel::Debug, "[Sound::SetVolume] " << filepath_ << " -> " << volume_ << " (instances: " << sound_instances_.size() << ")");
            
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_volume(instance->sound, volume_);
    }
  }
}

void Sound::SetPitch(float pitch) {
  // Clamp pitch to valid range [0.1, 10.0] to avoid audio issues
  constexpr float MIN_PITCH = 0.1f;
  constexpr float MAX_PITCH = 10.0f;
  pitch_ = (std::min)(MAX_PITCH, (std::max)(MIN_PITCH, pitch));
}

bool Sound::IsPlaying() const {
  // Return true if any instance is playing
  for (const auto& instance : sound_instances_) {
    if (instance->sound && ma_sound_is_playing(instance->sound) == MA_TRUE) {
      return true;
    }
  }
  return false;
}

// Spatial Audio Methods
void Sound::SetPosition(const Vec3& position) {
  position_ = position;
  
  // Update all existing instances
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_position(instance->sound, position_.x, position_.y, position_.z);
    }
  }
}

void Sound::SetMinDistance(float minDistance) {
  if (minDistance <= 0.0f) {
    AUDIO_LOG(LogLevel::Warn, "[Sound::SetMinDistance] Invalid minDistance: " << minDistance << ", clamping to 0.1");
    minDistance = 0.1f;
  }
  min_distance_ = minDistance;
  
  // Update all existing instances
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_min_distance(instance->sound, min_distance_);
    }
  }
}

void Sound::SetMaxDistance(float maxDistance) {
  if (maxDistance <= min_distance_) {
    AUDIO_LOG(LogLevel::Warn, "[Sound::SetMaxDistance] maxDistance must be > minDistance, setting to minDistance + 1.0");
    maxDistance = min_distance_ + 1.0f;
  }
  max_distance_ = maxDistance;
  
  // Update all existing instances
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_max_distance(instance->sound, max_distance_);
    }
  }
}

void Sound::SetRolloff(float rolloff) {
  if (rolloff < 0.0f) {
    AUDIO_LOG(LogLevel::Warn, "[Sound::SetRolloff] Invalid rolloff: " << rolloff << ", clamping to 0.0");
    rolloff = 0.0f;
  }
  rolloff_ = rolloff;
  
  // Update all existing instances
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_rolloff(instance->sound, rolloff_);
    }
  }
}

void Sound::SetSpatializationEnabled(bool enabled) {
  spatialization_enabled_ = enabled;
  
  // Update all existing instances
  for (auto& instance : sound_instances_) {
    if (instance->sound) {
      ma_sound_set_spatialization_enabled(instance->sound, enabled ? MA_TRUE : MA_FALSE);
    }
  }
}

}  // namespace audio
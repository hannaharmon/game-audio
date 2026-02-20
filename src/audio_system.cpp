#include "audio_system.h"
#include "audio_group.h"
#include "sound.h"
#include "audio_manager.h"
#include "logging.h"

namespace audio {

AudioSystem::AudioSystem() : master_volume_(1.0f),
                              cached_listener_position_(0.0f, 0.0f, 0.0f),
                              cached_listener_direction_(0.0f, 0.0f, -1.0f),
                              cached_listener_up_(0.0f, 1.0f, 0.0f),
                              listener_state_dirty_(false) {
  if (ma_engine_init(nullptr, &engine_) != MA_SUCCESS) {
    throw AudioException("Failed to initialize audio engine");
  }
}

AudioSystem::~AudioSystem() {
  // Stop the engine to flush all remaining sounds
  ma_engine_stop(&engine_);
  
  // Then uninit the engine
  ma_engine_uninit(&engine_);
}

std::unique_ptr<Sound> AudioSystem::CreateSound(const std::string& filepath, AudioGroup* group) {
  return Sound::Create(&engine_, filepath, group);
}

std::unique_ptr<AudioGroup> AudioSystem::CreateGroup() {
  return AudioGroup::Create(&engine_);
}

void AudioSystem::SetMasterVolume(float volume) {
  master_volume_ = volume;
  ma_engine_set_volume(&engine_, master_volume_);
}

// Listener Management
void AudioSystem::SetListenerPosition(const Vec3& position, ma_uint32 listenerIndex) {
  // Only update if position actually changed (optimization for frequent updates)
  if (listenerIndex == 0 && position.x == cached_listener_position_.x &&
      position.y == cached_listener_position_.y && position.z == cached_listener_position_.z) {
    return;  // Position unchanged, skip update
  }
  
  ma_engine_listener_set_position(&engine_, listenerIndex, position.x, position.y, position.z);
  
  // Cache the position for listener 0 (most common case)
  if (listenerIndex == 0) {
    cached_listener_position_ = position;
  }
}

Vec3 AudioSystem::GetListenerPosition(ma_uint32 listenerIndex) const {
  ma_vec3f pos = ma_engine_listener_get_position(&engine_, listenerIndex);
  return Vec3(pos.x, pos.y, pos.z);
}

void AudioSystem::SetListenerDirection(const Vec3& direction, ma_uint32 listenerIndex) {
  // Only update if direction actually changed (optimization for frequent updates)
  if (listenerIndex == 0 && direction.x == cached_listener_direction_.x &&
      direction.y == cached_listener_direction_.y && direction.z == cached_listener_direction_.z) {
    return;  // Direction unchanged, skip update
  }
  
  ma_engine_listener_set_direction(&engine_, listenerIndex, direction.x, direction.y, direction.z);
  
  // Cache the direction for listener 0 (most common case)
  if (listenerIndex == 0) {
    cached_listener_direction_ = direction;
  }
}

Vec3 AudioSystem::GetListenerDirection(ma_uint32 listenerIndex) const {
  ma_vec3f dir = ma_engine_listener_get_direction(&engine_, listenerIndex);
  return Vec3(dir.x, dir.y, dir.z);
}

void AudioSystem::SetListenerUp(const Vec3& up, ma_uint32 listenerIndex) {
  // Only update if up vector actually changed (optimization for frequent updates)
  if (listenerIndex == 0 && up.x == cached_listener_up_.x &&
      up.y == cached_listener_up_.y && up.z == cached_listener_up_.z) {
    return;  // Up vector unchanged, skip update
  }
  
  ma_engine_listener_set_world_up(&engine_, listenerIndex, up.x, up.y, up.z);
  
  // Cache the up vector for listener 0 (most common case)
  if (listenerIndex == 0) {
    cached_listener_up_ = up;
  }
}

Vec3 AudioSystem::GetListenerUp(ma_uint32 listenerIndex) const {
  ma_vec3f up = ma_engine_listener_get_world_up(&engine_, listenerIndex);
  return Vec3(up.x, up.y, up.z);
}

}  // namespace audio
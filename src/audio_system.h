#pragma once

#include "miniaudio/miniaudio.h"
#include "vec3.h"
#include <memory>
#include <string>
#include <unordered_map>

/**
 * @file audio_system.h
 * @brief Definition of the AudioSystem class for low-level audio operations
 */

namespace audio {

// Forward declarations
class AudioGroup;
class Sound;

/**
 * @class AudioSystem
 * @brief Low-level audio system that interfaces directly with miniaudio
 * 
 * The AudioSystem class handles the core audio functionality, including
 * initialization of the miniaudio engine and creation of sounds and groups.
 * It manages the underlying audio resources and provides a layer of
 * abstraction over the miniaudio library.
 * 
 * AudioSystem is not meant to be used directly by game code; instead,
 * it's accessed through the AudioManager.
 */
class AudioSystem {
 public:
  /**
   * @brief Constructor
   * 
   * Initializes the miniaudio engine
   */
  AudioSystem();
  
  /**
   * @brief Destructor
   * 
   * Cleans up all audio resources and uninitializes the miniaudio engine
   */
  ~AudioSystem();

  /**
   * @brief Creates a new Sound object
   * 
   * Loads an audio file and creates a Sound object. The caller takes ownership
   * of the returned unique_ptr.
   * 
   * @param filepath Path to the audio file
   * @param group Optional pointer to the group to add the sound to
   * @return std::unique_ptr<Sound> Unique pointer to the newly created Sound (caller owns)
   */
  std::unique_ptr<Sound> CreateSound(const std::string& filepath, AudioGroup* group = nullptr);

  /**
   * @brief Creates a new AudioGroup object
   * 
   * Creates an audio group for collective control of sounds. The caller takes
   * ownership of the returned unique_ptr.
   * 
   * @return std::unique_ptr<AudioGroup> Unique pointer to the newly created AudioGroup (caller owns)
   */
  std::unique_ptr<AudioGroup> CreateGroup();

  /**
   * @brief Sets the master volume for all audio
   * 
   * @param volume Volume level (0.0 to 1.0)
   */
  void SetMasterVolume(float volume);
  
  /**
   * @brief Gets the current master volume
   * 
   * @return float Current master volume (0.0 to 1.0)
   */
  float GetMasterVolume() const { return master_volume_; }

  ///@name Listener Management (Spatial Audio)
  ///@{
  
  /**
   * @brief Set the listener position in 3D space
   * 
   * The listener represents the "ears" of the player/camera.
   * All spatialized sounds are positioned relative to the listener.
   * 
   * @param position 3D position of the listener
   * @param listenerIndex Index of the listener (default 0)
   */
  void SetListenerPosition(const Vec3& position, ma_uint32 listenerIndex = 0);
  
  /**
   * @brief Get the listener position
   * 
   * @param listenerIndex Index of the listener (default 0)
   * @return Vec3 Current listener position
   */
  Vec3 GetListenerPosition(ma_uint32 listenerIndex = 0) const;
  
  /**
   * @brief Set the listener direction (forward vector)
   * 
   * The direction vector represents which way the listener is facing.
   * Should be normalized.
   * 
   * @param direction Forward direction vector (should be normalized)
   * @param listenerIndex Index of the listener (default 0)
   */
  void SetListenerDirection(const Vec3& direction, ma_uint32 listenerIndex = 0);
  
  /**
   * @brief Get the listener direction
   * 
   * @param listenerIndex Index of the listener (default 0)
   * @return Vec3 Current listener direction
   */
  Vec3 GetListenerDirection(ma_uint32 listenerIndex = 0) const;
  
  /**
   * @brief Set the listener up vector
   * 
   * The up vector defines the orientation of the listener.
   * Typically (0, 1, 0) for a standard Y-up coordinate system.
   * 
   * @param up Up vector (should be normalized)
   * @param listenerIndex Index of the listener (default 0)
   */
  void SetListenerUp(const Vec3& up, ma_uint32 listenerIndex = 0);
  
  /**
   * @brief Get the listener up vector
   * 
   * @param listenerIndex Index of the listener (default 0)
   * @return Vec3 Current listener up vector
   */
  Vec3 GetListenerUp(ma_uint32 listenerIndex = 0) const;
  
  /**
   * @brief Get access to the miniaudio engine (for advanced use)
   * 
   * @return ma_engine* Pointer to the miniaudio engine
   */
  ma_engine* GetEngine() { return &engine_; }
  
  ///@}

 private:
  ma_engine engine_;                                      ///< miniaudio engine instance
  float master_volume_;                                   ///< Master volume level
  
  ///@name Cached Listener State (for optimization)
  ///@{
  mutable Vec3 cached_listener_position_;                 ///< Cached listener position to avoid redundant updates
  mutable Vec3 cached_listener_direction_;               ///< Cached listener direction
  mutable Vec3 cached_listener_up_;                       ///< Cached listener up vector
  mutable bool listener_state_dirty_;                     ///< Flag to track if listener state needs update
  ///@}
};

}  // namespace audio
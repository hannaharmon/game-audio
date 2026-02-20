#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <random>
#include <functional>
#include "logging.h"
#include "vec3.h"

#ifdef PlaySound
#undef PlaySound
#endif

/**
 * @file audio_manager.h
 * @brief Main interface for the audio system
 * 
 * This file contains the AudioManager class which serves as the primary
 * interface for all audio functionality in the game engine.
 */

namespace audio {

/**
 * @brief Exception thrown when an audio operation fails
 */
class AudioException : public std::runtime_error {
public:
    explicit AudioException(const std::string& message) 
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when an invalid handle is used
 */
class InvalidHandleException : public AudioException {
public:
    explicit InvalidHandleException(const std::string& message)
        : AudioException(message) {}
};

/**
 * @brief Exception thrown when a file cannot be loaded
 */
class FileLoadException : public AudioException {
public:
    explicit FileLoadException(const std::string& message)
        : AudioException(message) {}
};

/**
 * @brief Exception thrown when the audio system is not initialized
 */
class NotInitializedException : public AudioException {
public:
    explicit NotInitializedException(const std::string& message)
        : AudioException(message) {}
};

} // namespace audio

// Forward declarations
namespace audio {
    class AudioSystem;
    class AudioTrack;
    class AudioGroup;
    class Sound;
}

// Place std type usage in global namespace
using ::std::atomic;
using ::std::mutex;
using ::std::thread;
using ::std::unordered_set;
using ::std::unordered_map;
using ::std::string;
using ::std::unique_ptr;
using ::std::shared_ptr;
using namespace ::std::chrono_literals;

namespace audio {

/**
 * @brief Opaque handle types for audio objects
 * 
 * These typedefs define opaque handles that are used to reference audio objects.
 * The actual implementation details are hidden from the user.
 */
/**
 * @brief Strongly typed handle for audio tracks.
 */
struct TrackHandle {
    uint32_t value{0};

    static constexpr TrackHandle Invalid() { return TrackHandle{0}; }
    constexpr bool IsValid() const { return value != 0; }
    constexpr uint32_t Value() const { return value; }
    explicit constexpr operator bool() const { return IsValid(); }
    friend constexpr bool operator==(const TrackHandle&, const TrackHandle&) = default;
};

/**
 * @brief Strongly typed handle for audio groups.
 */
struct GroupHandle {
    uint32_t value{0};

    static constexpr GroupHandle Invalid() { return GroupHandle{0}; }
    constexpr bool IsValid() const { return value != 0; }
    constexpr uint32_t Value() const { return value; }
    explicit constexpr operator bool() const { return IsValid(); }
    friend constexpr bool operator==(const GroupHandle&, const GroupHandle&) = default;
};

/**
 * @brief Strongly typed handle for individual sounds.
 */
struct SoundHandle {
    uint32_t value{0};

    static constexpr SoundHandle Invalid() { return SoundHandle{0}; }
    constexpr bool IsValid() const { return value != 0; }
    constexpr uint32_t Value() const { return value; }
    explicit constexpr operator bool() const { return IsValid(); }
    friend constexpr bool operator==(const SoundHandle&, const SoundHandle&) = default;
};

} // namespace audio

namespace std {
template <>
struct hash<audio::TrackHandle> {
    size_t operator()(const audio::TrackHandle& handle) const noexcept {
        return std::hash<uint32_t>{}(handle.Value());
    }
};

template <>
struct hash<audio::GroupHandle> {
    size_t operator()(const audio::GroupHandle& handle) const noexcept {
        return std::hash<uint32_t>{}(handle.Value());
    }
};

template <>
struct hash<audio::SoundHandle> {
    size_t operator()(const audio::SoundHandle& handle) const noexcept {
        return std::hash<uint32_t>{}(handle.Value());
    }
};
} // namespace std

namespace audio {

/**
 * @class AudioManager
 * @brief Central manager for all audio functionality
 * 
 * The AudioManager class provides a singleton interface for all audio operations.
 * It manages audio tracks, groups, and individual sounds, providing a high-level
 * API for game code to interact with the audio system.
 * 
 * This class handles resource management, playback control, volume adjustments,
 * and other audio operations. It is the only class that should be directly
 * accessed by game code.
 * 
 * @thread_safety All public methods are thread-safe and can be called from any thread.
 * The internal update thread runs at ~60Hz and updates fading/volume transitions.
 * All resource access is protected by internal mutexes.
 */
class AudioManager {
public:
    /**
     * @brief Get the singleton instance of the AudioManager
     * 
     * @return AudioManager& Reference to the singleton instance
     */
    static AudioManager& GetInstance();

    ///@name System Lifecycle
    ///@{
    
    /**
     * @brief Initialize the audio system
     * 
     * This function initializes the underlying audio system, including
     * the miniaudio engine. It must be called before any other audio
     * operations.
     * 
     * @return bool True if initialization was successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shut down the audio system
     * 
     * This function cleans up all audio resources and shuts down the
     * miniaudio engine. It should be called before application exit.
     */
    void Shutdown();
    ///@}

    ///@name System Control
    ///@{
    
    /**
     * @brief Set the master volume for all audio
     * 
     * @param volume Volume level (0.0 to 1.0)
     */
    void SetMasterVolume(float volume);
    
    /**
     * @brief Get the current master volume level
     * 
     * @return float Current master volume (0.0 to 1.0)
     */
    float GetMasterVolume() const;
    
    /**
     * @brief Set the global audio log level (runtime).
     *
     * Logging defaults to Off. Use this method to enable logging output.
     */
    static void SetLogLevel(LogLevel level);

    /**
     * @brief Get the current audio log level.
     */
    static LogLevel GetLogLevel();
    ///@}

    ///@name Track Management
    ///@{
    
    /**
     * @brief Create a new audio track
     * 
     * Audio tracks can contain multiple layers of audio that play
     * simultaneously, with individual volume control for each layer.
     * 
     * @return TrackHandle Handle to the newly created track
     */
    TrackHandle CreateTrack();
    
    /**
     * @brief Destroy an audio track
     * 
     * This releases all resources associated with the track.
     * 
     * @param track Handle to the track to destroy
     */
    void DestroyTrack(TrackHandle track);
    
    /**
     * @brief Start playing an audio track
     * 
     * All layers in the track will begin playing.
     * 
     * @param track Handle to the track to play
     * @throws InvalidHandleException If the track handle is invalid
     * @throws FileLoadException If any layer's sound file cannot be loaded or initialized for playback
     */
    void PlayTrack(TrackHandle track);
    
    /**
     * @brief Stop playing an audio track
     * 
     * All layers in the track will stop playing.
     * 
     * @param track Handle to the track to stop
     * @throws InvalidHandleException If the track handle is invalid
     */
    void StopTrack(TrackHandle track);
    
    ///@}
    
    ///@name Layer Operations
    ///@{
    
    /**
     * @brief Add an audio layer to a track.
     *
     * Layers are individual sounds that play simultaneously within a track.
     * They can be controlled individually for volume and transitions.
     *
     * @param track Handle to the track
     * @param layerName Name identifier for the layer (unique within the track)
     * @param filepath Path to the audio file
     * @param group Optional handle of the group this layer should belong to (invalid = default/master)
     * @throws InvalidHandleException If the track handle is invalid or the group handle is invalid
     * @throws FileLoadException If the audio file cannot be loaded
     */
    void AddLayer(TrackHandle track, const string& layerName, const string& filepath, GroupHandle group = GroupHandle::Invalid());
    
    /**
     * @brief Remove a layer from a track
     * 
     * @param track Handle to the track
     * @param layerName Name of the layer to remove
     */
    void RemoveLayer(TrackHandle track, const string& layerName);
    
    /**
     * @brief Set the volume of a specific layer
     * 
     * @param track Handle to the track
     * @param layerName Name of the layer
     * @param volume Volume level (0.0 to 1.0)
     */
    void SetLayerVolume(TrackHandle track, const string& layerName, float volume);
    
    /**
     * @brief Fade a layer's volume to a target value over time
     * 
     * @param track Handle to the track
     * @param layerName Name of the layer
     * @param targetVolume Target volume level (0.0 to 1.0)
     * @param duration Duration of the fade in milliseconds
     */
    void FadeLayer(TrackHandle track, const string& layerName, float targetVolume, std::chrono::milliseconds duration);
    ///@}
    
    ///@name Group Operations
    ///@{
    
    /**
     * @brief Create a new audio group.
     *
     * Audio groups allow for collective control of multiple sounds.
     * Groups are referenced by handle; names are not part of the public API.
     *
     * @return GroupHandle Handle to the newly created group
     * @throws AudioException If group creation fails
     */
    GroupHandle CreateGroup();
    
    /**
     * @brief Destroy an audio group
     * 
     * @param group Handle to the group to destroy
     */
    void DestroyGroup(GroupHandle group);
    
    /**
     * @brief Set the volume for an entire audio group
     * 
     * This affects all sounds assigned to the group.
     * 
     * @param group Handle to the group
     * @param volume Volume level (0.0 to 1.0)
     * @throws InvalidHandleException If the group handle is invalid
     */
    void SetGroupVolume(GroupHandle group, float volume);
    
    /**
     * @brief Get the current volume for an audio group
     * 
     * @param group Handle to the group
     * @return float Current volume level (0.0 to 1.0)
     */
    float GetGroupVolume(GroupHandle group) const;
    
    /**
     * @brief Fade a group's volume to a target value over time
     * 
     * @param group Handle to the group
     * @param targetVolume Target volume level (0.0 to 1.0)
     * @param duration Duration of the fade in milliseconds
     */
    void FadeGroup(GroupHandle group, float targetVolume, std::chrono::milliseconds duration);
    ///@}
    
    ///@name Sound Operations
    ///@{
    
    /**
     * @brief Load a sound from a file
     * 
     * @param filepath Path to the audio file
     * @return SoundHandle Handle to the loaded sound
     * @throws FileLoadException If the file cannot be found or loaded
     */
    SoundHandle LoadSound(const string& filepath);
    
    /**
     * @brief Load a sound from a file and assign it to a group
     * 
     * @param filepath Path to the audio file
     * @param group Handle to the audio group to assign the sound to
     * @return SoundHandle Handle to the loaded sound
     * @throws FileLoadException If the file cannot be found or loaded
     */
    SoundHandle LoadSound(const string& filepath, GroupHandle group);
    
    /**
     * @brief Destroy a previously loaded sound
     * 
     * @param sound Handle to the sound to destroy
     */
    void DestroySound(SoundHandle sound);
    
    /**
     * @brief Play a sound
     * 
     * @param sound Handle to the sound to play
     * @throws InvalidHandleException If the sound handle is invalid
     * @throws FileLoadException If the sound file cannot be loaded or initialized for playback
     */
    /**
     * @brief Play a sound
     * 
     * Starts playback of the sound using its current position.
     * 
     * @param sound Handle to the sound
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void PlaySound(SoundHandle sound);
    
    /**
     * @brief Play a sound at a specific position
     * 
     * Starts playback of the sound at the specified position. This allows
     * multiple overlapping spatialized sounds from the same audio file to
     * play at different positions simultaneously (e.g., multiple gunshots).
     * 
     * The position is only applied to this new playback instance. Existing
     * instances keep their positions unchanged.
     * 
     * @param sound Handle to the sound
     * @param position 3D position for this playback instance
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void PlaySound(SoundHandle sound, const Vec3& position);
    
    /**
     * @brief Stop a currently playing sound
     * 
     * @param sound Handle to the sound to stop
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void StopSound(SoundHandle sound);
    
    /**
     * @brief Set the volume of a sound
     * 
     * @param sound Handle to the sound
     * @param volume Volume level (0.0 to 1.0)
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundVolume(SoundHandle sound, float volume);
    
    /**
     * @brief Set the pitch of a sound for its next playback
     * 
     * @param sound Handle to the sound
     * @param pitch Pitch multiplier (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
     */
    void SetSoundPitch(SoundHandle sound, float pitch);
    
    /**
     * @brief Set whether a sound should loop
     * 
     * @param sound Handle to the sound
     * @param should_loop Whether the sound should loop continuously
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundLooping(SoundHandle sound, bool should_loop);
    
    /**
     * @brief Check if a sound is currently playing
     * 
     * @param sound Handle to the sound
     * @return bool True if the sound is playing, false otherwise
     */
    bool IsSoundPlaying(SoundHandle sound) const;
    
    /**
     * @brief Play a random sound from a folder
     * 
     * Loads all .wav files from the specified folder and plays one randomly.
     * Sounds are cached after first load for efficiency.
     * 
     * @param folderPath Path to the folder containing sound files
     * @param group Optional group handle to assign the sounds to
     * @throws AudioException If the folder path is empty
     * @throws FileLoadException If the selected sound file cannot be loaded or initialized for playback
     */
    void PlayRandomSoundFromFolder(const string& folderPath, GroupHandle group = GroupHandle::Invalid());
    ///@}

    ///@name Spatial Audio (3D Positioning)
    ///@{
    
    /**
     * @brief Set the listener position in 3D space
     * 
     * The listener represents the "ears" of the player/camera.
     * All spatialized sounds are positioned relative to the listener.
     * 
     * Coordinate System: Uses OpenGL/miniaudio convention:
     * - +X = Right, +Y = Up, -Z = Forward
     * 
     * For 2D games (Node2D): Simply use z=0:
     * ```cpp
     * Vec3 audio_pos(node2d_pos.x, node2d_pos.y, 0.0f);
     * SetListenerPosition(audio_pos);
     * ```
     * 
     * To use with game engines (e.g., Basilisk Engine nodes):
     * Simply convert your engine's position vector to Vec3:
     * ```cpp
     * auto node_pos = camera_node.get_position();
     * Vec3 audio_pos(node_pos.x, node_pos.y, node_pos.z);
     * SetListenerPosition(audio_pos);
     * ```
     * 
     * Performance: This method is optimized to skip updates when the position
     * hasn't changed, so it's safe to call every frame. The mutex overhead
     * is minimal, and spatial audio requires frequent updates for accurate
     * positioning.
     * 
     * @param position 3D position of the listener
     * @param listenerIndex Index of the listener (default 0)
     */
    void SetListenerPosition(const Vec3& position, uint32_t listenerIndex = 0);
    
    /**
     * @brief Get the listener position
     * 
     * @param listenerIndex Index of the listener (default 0)
     * @return Vec3 Current listener position
     */
    Vec3 GetListenerPosition(uint32_t listenerIndex = 0) const;
    
    /**
     * @brief Set the listener direction (forward vector)
     * 
     * The direction vector represents which way the listener is facing.
     * Should be normalized.
     * 
     * @param direction Forward direction vector (should be normalized)
     * @param listenerIndex Index of the listener (default 0)
     */
    void SetListenerDirection(const Vec3& direction, uint32_t listenerIndex = 0);
    
    /**
     * @brief Get the listener direction
     * 
     * @param listenerIndex Index of the listener (default 0)
     * @return Vec3 Current listener direction
     */
    Vec3 GetListenerDirection(uint32_t listenerIndex = 0) const;
    
    /**
     * @brief Set the listener up vector
     * 
     * The up vector defines the orientation of the listener.
     * Typically (0, 1, 0) for a standard Y-up coordinate system.
     * 
     * @param up Up vector (should be normalized)
     * @param listenerIndex Index of the listener (default 0)
     */
    void SetListenerUp(const Vec3& up, uint32_t listenerIndex = 0);
    
    /**
     * @brief Get the listener up vector
     * 
     * @param listenerIndex Index of the listener (default 0)
     * @return Vec3 Current listener up vector
     */
    Vec3 GetListenerUp(uint32_t listenerIndex = 0) const;
    
    /**
     * @brief Set the 3D position of a sound
     * 
     * Sets the position for the sound. The sound will be spatialized
     * relative to the listener position.
     * 
     * Coordinate System: Uses OpenGL/miniaudio convention:
     * - +X = Right, +Y = Up, -Z = Forward
     * 
     * For 2D games (Node2D): Simply use z=0:
     * ```cpp
     * Vec3 audio_pos(node2d_pos.x, node2d_pos.y, 0.0f);
     * SetSoundPosition(sound, audio_pos);
     * ```
     * 
     * To use with game engines (e.g., Basilisk Engine nodes):
     * Simply convert your engine's node position to Vec3:
     * ```cpp
     * auto node_pos = game_object.get_position();
     * Vec3 audio_pos(node_pos.x, node_pos.y, node_pos.z);
     * SetSoundPosition(sound, audio_pos);
     * ```
     * 
     * @param sound Handle to the sound
     * @param position 3D position of the sound
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundPosition(SoundHandle sound, const Vec3& position);
    
    /**
     * @brief Get the 3D position of a sound
     * 
     * @param sound Handle to the sound
     * @return Vec3 Current 3D position
     * @throws InvalidHandleException If the sound handle is invalid
     */
    Vec3 GetSoundPosition(SoundHandle sound) const;
    
    /**
     * @brief Set the minimum distance for distance attenuation
     * 
     * At distances less than minDistance, the sound will be at full volume.
     * Beyond minDistance, volume will attenuate based on the attenuation model.
     * 
     * @param sound Handle to the sound
     * @param minDistance Minimum distance (must be > 0)
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundMinDistance(SoundHandle sound, float minDistance);
    
    /**
     * @brief Get the minimum distance of a sound
     * 
     * @param sound Handle to the sound
     * @return float Current minimum distance
     * @throws InvalidHandleException If the sound handle is invalid
     */
    float GetSoundMinDistance(SoundHandle sound) const;
    
    /**
     * @brief Set the maximum distance for distance attenuation
     * 
     * At distances beyond maxDistance, the sound will be at minimum gain.
     * 
     * @param sound Handle to the sound
     * @param maxDistance Maximum distance (must be > minDistance)
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundMaxDistance(SoundHandle sound, float maxDistance);
    
    /**
     * @brief Get the maximum distance of a sound
     * 
     * @param sound Handle to the sound
     * @return float Current maximum distance
     * @throws InvalidHandleException If the sound handle is invalid
     */
    float GetSoundMaxDistance(SoundHandle sound) const;
    
    /**
     * @brief Set the rolloff factor for distance attenuation
     * 
     * Higher values mean faster volume dropoff with distance.
     * Typical values: 1.0 (linear), 2.0 (inverse square)
     * 
     * @param sound Handle to the sound
     * @param rolloff Rolloff factor (typically 1.0 to 2.0)
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundRolloff(SoundHandle sound, float rolloff);
    
    /**
     * @brief Get the rolloff factor of a sound
     * 
     * @param sound Handle to the sound
     * @return float Current rolloff factor
     * @throws InvalidHandleException If the sound handle is invalid
     */
    float GetSoundRolloff(SoundHandle sound) const;
    
    /**
     * @brief Enable or disable spatialization for a sound
     * 
     * @param sound Handle to the sound
     * @param enabled Whether to enable spatialization
     * @throws InvalidHandleException If the sound handle is invalid
     */
    void SetSoundSpatializationEnabled(SoundHandle sound, bool enabled);
    
    /**
     * @brief Check if spatialization is enabled for a sound
     * 
     * @param sound Handle to the sound
     * @return bool True if spatialization is enabled
     * @throws InvalidHandleException If the sound handle is invalid
     */
    bool IsSoundSpatializationEnabled(SoundHandle sound) const;
    ///@}

    /**
     * @brief Check if the audio system is initialized and running
     * 
     * @return bool True if the system is initialized, false otherwise
     */
    bool IsInitialized() const { return running_.load(); }

private:
    /**
     * @brief Constructor - private due to singleton pattern
     */
    AudioManager();
    
    /**
     * @brief Destructor
     */
    ~AudioManager();
    
    // Disable copy and assignment
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * @brief Ensures the audio system is initialized before use
     * 
     * @throws NotInitializedException If Initialize() has not been called
     */
    void EnsureInitialized() const;

    ///@name Internal Handle Management
    ///@{
    
    /**
     * @brief Generate a new track handle
     * @return TrackHandle New unique handle
     */
    TrackHandle NextTrackHandle() { return TrackHandle{next_track_handle_++}; }
    
    /**
     * @brief Generate a new group handle
     * @return GroupHandle New unique handle
     */
    GroupHandle NextGroupHandle() { return GroupHandle{next_group_handle_++}; }
    
    /**
     * @brief Generate a new sound handle
     * @return SoundHandle New unique handle
     */
    SoundHandle NextSoundHandle() { return SoundHandle{next_sound_handle_++}; }
    ///@}

    ///@name Resource Storage
    ///@{
    unique_ptr<AudioSystem> audio_system_;   ///< Core audio system
    unordered_map<TrackHandle, unique_ptr<AudioTrack>> tracks_;  ///< Track storage
    unordered_map<GroupHandle, unique_ptr<AudioGroup>> groups_;  ///< Group storage
    unordered_map<SoundHandle, unique_ptr<Sound>> sounds_;       ///< Sound storage
    unordered_map<string, std::vector<SoundHandle>> folder_sounds_; ///< Cached sounds per folder
    ///@}

    ///@name Threading
    ///@{
    atomic<bool> running_{false};     ///< Flag indicating if audio system is running
    thread update_thread_;            ///< Thread for audio updates
    mutable mutex resource_mutex_;    ///< Mutex for thread-safe resource access (mutable for const methods)
    std::mt19937 rng_{std::random_device{}()}; ///< RNG for random playback
    ///@}

    ///@name Handle Counters
    ///@{
    atomic<uint32_t> next_track_handle_{1};    ///< Counter for generating unique track handles
    atomic<uint32_t> next_group_handle_{1};    ///< Counter for generating unique group handles
    atomic<uint32_t> next_sound_handle_{1};    ///< Counter for generating unique sound handles
    ///@}
};

} // namespace audio
#define NOMINMAX  // Prevent Windows from defining min/max macros
#include "audio_manager.h"
#include "audio_system.h"
#include "audio_track.h"
#include "audio_group.h"
#include "sound.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

// Place all std usage in global namespace
using ::std::lock_guard;
using ::std::mutex;
using ::std::thread;
using ::std::unique_ptr;
using ::std::shared_ptr;
using ::std::make_unique;
using ::std::make_shared;
using namespace ::std::chrono_literals;

namespace audio {

AudioManager::AudioManager() {
    audio_system_ = make_unique<AudioSystem>();
}

AudioManager::~AudioManager() {
    if (running_) {
        Shutdown();
    }
}

AudioManager& AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::EnsureInitialized() const {
    if (!running_) {
        throw NotInitializedException("Audio system not initialized. Call Initialize() first.");
    }
}

bool AudioManager::Initialize() {
    if (running_) {
        std::cerr << "AudioManager already running" << std::endl;
        return false;
    }

    // Re-create audio system if it was destroyed during shutdown
    if (!audio_system_) {
        audio_system_ = make_unique<AudioSystem>();
    }

    running_ = true;

    update_thread_ = thread([this]() {
        while (running_) {
            std::this_thread::sleep_for(16ms); // ~60Hz update rate

            lock_guard<mutex> lock(resource_mutex_);

            // Update all tracks
            for (auto& [handle, track] : tracks_) {
                if (track) {
                    track->Update();
                }
            }
            
            // Update all groups for fading
            auto now = std::chrono::steady_clock::now();
            for (auto& [handle, group] : groups_) {
                if (!group) continue;
                
                // Check if the group is fading
                auto fade_end_time = group->GetFadeEndTime();
                if (now < fade_end_time) {
                    // Calculate progress as a percentage (0 to 1)
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - (fade_end_time - group->GetFadeDuration()));
                    float progress = static_cast<float>(elapsed.count()) / group->GetFadeDuration().count();
                    progress = std::min(1.0f, std::max(0.0f, progress));
                    
                    // Linear interpolation between start and target volume
                    float new_volume = group->GetStartVolume() + 
                        (group->GetTargetVolume() - group->GetStartVolume()) * progress;
                    
                    group->SetVolume(new_volume);
                }
                else if (group->IsFading()) {
                    // Fade is complete, set to target volume
                    group->SetVolume(group->GetTargetVolume());
                    group->SetFadingState(false);
                }
            }
        }
    });

    return true;
}

void AudioManager::Shutdown() {
    if (!running_) return;

    running_ = false;
    if (update_thread_.joinable()) {
        update_thread_.join();
    }

    // Clean up resources in proper order
    lock_guard<mutex> lock(resource_mutex_);
    
    // First stop all tracks to ensure sounds stop before being destroyed
    for (auto& [handle, track] : tracks_) {
        if (track) {
            track->Stop();
        }
    }
    
    // Then clear all resources
    tracks_.clear();
    sounds_.clear();  // Clear sounds before groups
    groups_.clear();  // Clear groups before audio system
    group_names_.clear();

    // Explicitly destroy the audio system to ensure miniaudio threads are stopped
    audio_system_.reset();
}

// System control
void AudioManager::SetMasterVolume(float volume) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    if (audio_system_) {
        audio_system_->SetMasterVolume(volume);
    }
}

float AudioManager::GetMasterVolume() const {
    EnsureInitialized();
    if (!audio_system_) return 0.0f;
    return audio_system_->GetMasterVolume();
}

// Track management
TrackHandle AudioManager::CreateTrack() {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    
    // Create a new track handle
    TrackHandle handle = NextTrackHandle();
    
    // Create the track and store it
    tracks_[handle] = AudioTrack::Create(audio_system_.get());
    
    return handle;
}

void AudioManager::DestroyTrack(TrackHandle track) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = tracks_.find(track);
    if (it != tracks_.end()) {
        tracks_.erase(it);
    }
}

void AudioManager::PlayTrack(TrackHandle track) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = tracks_.find(track);
    if (it == tracks_.end() || !it->second) {
        throw InvalidHandleException("Invalid track handle: " + std::to_string(track));
    }
    it->second->Play();
}

void AudioManager::StopTrack(TrackHandle track) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = tracks_.find(track);
    if (it == tracks_.end() || !it->second) {
        throw InvalidHandleException("Invalid track handle: " + std::to_string(track));
    }
    it->second->Stop();
}

// Layer operations
void AudioManager::AddLayer(TrackHandle track, const string& layerName, 
                           const string& filepath, const string& group) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto track_it = tracks_.find(track);
    if (track_it == tracks_.end() || !track_it->second) {
        throw InvalidHandleException("Invalid track handle: " + std::to_string(track));
    }
    
    // Look up the group by name if provided
    AudioGroup* group_ptr = nullptr;
    if (!group.empty()) {
        auto group_it = group_names_.find(group);
        if (group_it != group_names_.end()) {
            auto groups_it = groups_.find(group_it->second);
            if (groups_it != groups_.end() && groups_it->second) {
                group_ptr = groups_it->second.get();
            }
        }
    }
    
    track_it->second->AddLayer(layerName, filepath, group_ptr, true);
}

void AudioManager::RemoveLayer(TrackHandle track, const string& layerName) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto track_it = tracks_.find(track);
    if (track_it == tracks_.end() || !track_it->second) {
        throw InvalidHandleException("Invalid track handle: " + std::to_string(track));
    }
    
    track_it->second->RemoveLayer(layerName);
}

void AudioManager::SetLayerVolume(TrackHandle track, const string& layerName, float volume) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto track_it = tracks_.find(track);
    if (track_it == tracks_.end() || !track_it->second) {
        throw InvalidHandleException("Invalid track handle: " + std::to_string(track));
    }
    
    track_it->second->SetLayerVolume(layerName, volume);
}

void AudioManager::FadeLayer(TrackHandle track, const string& layerName, 
                            float targetVolume, std::chrono::milliseconds duration) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto track_it = tracks_.find(track);
    if (track_it == tracks_.end() || !track_it->second) return;
    
    track_it->second->FadeLayer(layerName, targetVolume, duration);
}

// Group operations
GroupHandle AudioManager::CreateGroup(const string& name) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    
    // Create a new group handle
    GroupHandle handle = NextGroupHandle();
    
    // Create the group in the audio system
    auto group = audio_system_->CreateGroup(name);
    if (!group) {
        throw AudioException("Failed to create audio group");
    }
    
    // Store the group
    groups_[handle] = std::move(group);
    
    // Store name mapping if provided
    if (!name.empty()) {
        group_names_[name] = handle;
    }
    
    return handle;
}

void AudioManager::DestroyGroup(GroupHandle group) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = groups_.find(group);
    if (it != groups_.end()) {
        // Remove from name mapping if present
        for (auto name_it = group_names_.begin(); name_it != group_names_.end(); ) {
            if (name_it->second == group) {
                name_it = group_names_.erase(name_it);
            } else {
                ++name_it;
            }
        }
        groups_.erase(it);
    }
}

void AudioManager::SetGroupVolume(GroupHandle group, float volume) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = groups_.find(group);
    if (it == groups_.end() || !it->second) {
        throw InvalidHandleException("Invalid group handle: " + std::to_string(group));
    }
    it->second->SetVolume(volume);
}

float AudioManager::GetGroupVolume(GroupHandle group) const {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = groups_.find(group);
    if (it != groups_.end() && it->second) {
        return it->second->GetVolume();
    }
    return 0.0f;
}

void AudioManager::FadeGroup(GroupHandle group, float targetVolume, 
                            std::chrono::milliseconds duration) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = groups_.find(group);
    if (it != groups_.end() && it->second) {
        it->second->FadeVolume(targetVolume, duration);
    }
}

// Sound operations
SoundHandle AudioManager::LoadSound(const string& filepath) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    
    // Create a new sound handle
    SoundHandle handle = NextSoundHandle();
    
    // Create the sound
    auto sound_ptr = audio_system_->CreateSound(filepath);
    if (!sound_ptr) {
        throw FileLoadException("Failed to load sound file: " + filepath);
    }
    
    // Store the sound
    sounds_[handle] = std::move(sound_ptr);
    
    return handle;
}

SoundHandle AudioManager::LoadSound(const string& filepath, GroupHandle group) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    
    // Create a new sound handle
    SoundHandle handle = NextSoundHandle();
    
    // Get the AudioGroup pointer
    AudioGroup* group_ptr = nullptr;
    if (group != 0) {
        auto group_it = groups_.find(group);
        if (group_it != groups_.end()) {
            group_ptr = group_it->second.get();
        }
    }
    
    // Create the sound with the group
    auto sound_ptr = audio_system_->CreateSound(filepath, group_ptr);
    if (!sound_ptr) {
        throw FileLoadException("Failed to load sound file: " + filepath);
    }
    
    // Store the sound
    sounds_[handle] = std::move(sound_ptr);
    
    return handle;
}

void AudioManager::DestroySound(SoundHandle sound) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it != sounds_.end()) {
        sounds_.erase(it);
    }
}

void AudioManager::StartSound(SoundHandle sound) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it == sounds_.end() || !it->second) {
        throw InvalidHandleException("Invalid sound handle: " + std::to_string(sound));
    }
    it->second->Play();
}

void AudioManager::StopSound(SoundHandle sound) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it == sounds_.end() || !it->second) {
        throw InvalidHandleException("Invalid sound handle: " + std::to_string(sound));
    }
    it->second->Stop();
}

void AudioManager::SetSoundVolume(SoundHandle sound, float volume) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it == sounds_.end() || !it->second) {
        throw InvalidHandleException("Invalid sound handle: " + std::to_string(sound));
    }
    it->second->SetVolume(volume);
}

void AudioManager::SetSoundPitch(SoundHandle sound, float pitch) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it != sounds_.end() && it->second) {
        it->second->SetPitch(pitch);
    }
}

bool AudioManager::IsSoundPlaying(SoundHandle sound) const {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    auto it = sounds_.find(sound);
    if (it != sounds_.end() && it->second) {
        return it->second->IsPlaying();
    }
    return false;
}

void AudioManager::PlayRandomSoundFromFolder(const string& folderPath, GroupHandle group) {
    EnsureInitialized();
    lock_guard<mutex> lock(resource_mutex_);
    
    // Get the AudioGroup pointer for this handle
    AudioGroup* group_ptr = nullptr;
    if (group != 0) {
        auto group_it = groups_.find(group);
        if (group_it != groups_.end()) {
            group_ptr = group_it->second.get();
        }
    }
    
    // Check if we've already loaded sounds from this folder
    auto folder_it = folder_sounds_.find(folderPath);
    if (folder_it == folder_sounds_.end()) {
        // First time accessing this folder - load all .wav files
        std::vector<SoundHandle> loaded_sounds;
        
        // Get all .wav files in the folder
        #ifdef _WIN32
        string searchPath = folderPath + "/*.wav";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            std::cout << "Found sounds in folder: " << folderPath << std::endl;
            do {
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    string filepath = folderPath + "/" + findData.cFileName;
                    
                    // Load the sound
                    SoundHandle handle = NextSoundHandle();
                    auto sound = audio_system_->CreateSound(filepath, group_ptr);
                    if (sound) {
                        sounds_[handle] = std::move(sound);
                        loaded_sounds.push_back(handle);
                    }
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
        #else
        // Unix/Linux/macOS implementation using opendir/readdir
        DIR* dir = opendir(folderPath.c_str());
        if (dir != nullptr) {
            std::cout << "Found sounds in folder: " << folderPath << std::endl;
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                string filename = entry->d_name;
                
                // Skip . and .. entries
                if (filename == "." || filename == "..") {
                    continue;
                }
                
                // Check if file ends with .wav (case-insensitive)
                if (filename.length() > 4) {
                    string extension = filename.substr(filename.length() - 4);
                    // Convert to lowercase for comparison
                    std::transform(extension.begin(), extension.end(), extension.begin(),
                                   [](unsigned char c) { return std::tolower(c); });
                    
                    if (extension == ".wav") {
                        string filepath = folderPath + "/" + filename;
                        
                        // Verify it's a regular file (not a directory or symlink to directory)
                        struct stat statbuf;
                        if (stat(filepath.c_str(), &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
                            // Load the sound
                            SoundHandle handle = NextSoundHandle();
                            auto sound = audio_system_->CreateSound(filepath, group_ptr);
                            if (sound) {
                                sounds_[handle] = std::move(sound);
                                loaded_sounds.push_back(handle);
                            }
                        }
                    }
                }
            }
            closedir(dir);
        }
        #endif
        
        if (loaded_sounds.empty()) {
            std::cerr << "No .wav files found in folder: " << folderPath << std::endl;
            return;
        }
        
        folder_sounds_[folderPath] = loaded_sounds;
        folder_it = folder_sounds_.find(folderPath);
    }
    
    // Play a random sound from the folder
    if (!folder_it->second.empty()) {
        size_t randomIndex = rand() % folder_it->second.size();
        SoundHandle randomSound = folder_it->second[randomIndex];
        
        auto sound_it = sounds_.find(randomSound);
        if (sound_it != sounds_.end() && sound_it->second) {
            sound_it->second->Play();
        }
    }
}

} // namespace audio
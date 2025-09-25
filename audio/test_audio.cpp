/**
 * @file test_audio.cpp
 * @brief Test application for the audio system
 * 
 * This file contains a simple test application that demonstrates the
 * functionality of the audio system. It creates a layered music track
 * with multiple instrument layers and allows interactive control of
 * volume and playback through a command-line interface.
 */

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <stdexcept>
#include <string>

#include "audio_manager.h"
using namespace std::chrono_literals;

/**
 * @brief Process user input commands for audio control
 * 
 * This function processes the following commands:
 * - v [0.0-1.0] - Set master volume
 * - m [0.0-1.0] - Set music volume
 * - s [0.0-1.0] - Set SFX volume
 * - x           - Play sound effect
 * - b           - Toggle battle mode (fade in/out)
 * - q           - Quit
 *
 * @param input User input string
 * @param music_track Handle to the music track
 * @param music_group Handle to the music group
 * @param sfx_group Handle to the SFX group
 * @param sfx Handle to the sound effect
 */
void ProcessInput(const std::string& input,
                 audio::TrackHandle music_track,
                 audio::GroupHandle music_group,
                 audio::GroupHandle sfx_group,
                 audio::SoundHandle sfx);

int main() {
    try {
        std::cout << "Starting audio test program...\n";
        
        // Get the audio manager instance and initialize it
        auto& audio = audio::AudioManager::GetInstance();
        std::cout << "Initializing audio system...\n";
        if (!audio.Initialize()) {
            std::cerr << "Failed to initialize audio system\n";
            return 1;
        }
        std::cout << "Audio system initialized successfully\n";

        std::cout << "Creating audio groups...\n";
        auto music_group = audio.CreateGroup("music");
        auto sfx_group = audio.CreateGroup("sfx");
        
        std::cout << "Setting initial group volumes...\n";
        audio.SetGroupVolume(music_group, 0.7f);  // Music quieter than SFX
        audio.SetGroupVolume(sfx_group, 1.0f);
        std::cout << "Groups created and configured\n";

        std::cout << "Creating layered music track...\n";
        auto music_track = audio.CreateTrack();
        
        std::cout << "Adding layers to music track...\n";
        // Add all our layers to the music track using absolute paths
        std::string sound_dir = "C:\\Users\\Administrator\\GameJams\\miniaudio\\sound_files\\";
        audio.AddLayer(music_track, "kick", sound_dir + "kick.wav", "music");
        audio.AddLayer(music_track, "clap", sound_dir + "clap.wav", "music");
        audio.AddLayer(music_track, "bass", sound_dir + "double_bass.wav", "music");
        audio.AddLayer(music_track, "flute", sound_dir + "alto_flute.wav", "music");
        audio.AddLayer(music_track, "clarinet", sound_dir + "clarinet.wav", "music");
        audio.AddLayer(music_track, "horns", sound_dir + "horns.wav", "music");
        std::cout << "All layers added successfully\n";
        
        std::cout << "Setting initial layer volumes...\n";
        // Start with just rhythm section
        audio.SetLayerVolume(music_track, "kick", 1.0f);
        audio.SetLayerVolume(music_track, "clap", 1.0f);
        audio.SetLayerVolume(music_track, "bass", 0.8f);
        audio.SetLayerVolume(music_track, "flute", 0.0f);
        std::cout << "Initial volumes set\n";
        audio.SetLayerVolume(music_track, "clarinet", 0.0f);
        audio.SetLayerVolume(music_track, "horns", 0.0f);

        // Start playing
        audio.PlayTrack(music_track);

        // Load our sound effect
        auto sfx = audio.LoadSound(sound_dir + "hit.wav");

        std::cout << "Interactive Audio Test" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  v [0.0-1.0] - Set master volume" << std::endl;
        std::cout << "  m [0.0-1.0] - Set music volume" << std::endl;
        std::cout << "  s [0.0-1.0] - Set SFX volume" << std::endl;
        std::cout << "  x           - Play sound effect" << std::endl;
        std::cout << "  b           - Toggle battle mode (fade in/out)" << std::endl;
        std::cout << "  q           - Quit" << std::endl;

        std::string input;
        std::atomic<bool> running{true};
        
        // Start input thread
        std::thread input_thread([&]() {
            std::string line;
            while (running && std::getline(std::cin, line)) {
                if (line == "q") {
                    running = false;
                    break;
                }
                input = line;  // Update the shared input string
            }
            running = false;  // Ensure we exit if stdin closes
        });

        // Main update loop
        while (running) {
            // Process any pending input
            if (!input.empty()) {
                ProcessInput(input, music_track, music_group, sfx_group, sfx);
                input.clear();
            }

            // Sleep to prevent CPU spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        // Cleanup
        if (input_thread.joinable()) {
            input_thread.join();
        }
        audio.Shutdown();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}

// Define the input processing function
void ProcessInput(const std::string& input,
                 audio::TrackHandle music_track,
                 audio::GroupHandle music_group,
                 audio::GroupHandle sfx_group,
                 audio::SoundHandle sfx) {
    if (input.empty()) return;

    auto& audio = audio::AudioManager::GetInstance();
    char command = input[0];
    try {
        float volume = 0.0f;
        if (input.length() > 2) {
            volume = std::stof(input.substr(2));
        }

        switch (command) {
            case 'v':
                if (volume >= 0.0f && volume <= 1.0f) {
                    audio.SetMasterVolume(volume);
                    std::cout << "Master volume set to: " << volume << std::endl;
                } else {
                    std::cout << "Volume must be between 0.0 and 1.0" << std::endl;
                }
                break;

            case 'm':
                if (volume >= 0.0f && volume <= 1.0f) {
                    audio.SetGroupVolume(music_group, volume);
                    std::cout << "Music volume set to: " << volume << std::endl;
                } else {
                    std::cout << "Volume must be between 0.0 and 1.0" << std::endl;
                }
                break;

            case 's':
                if (volume >= 0.0f && volume <= 1.0f) {
                    audio.SetGroupVolume(sfx_group, volume);
                    std::cout << "SFX volume set to: " << volume << std::endl;
                } else {
                    std::cout << "Volume must be between 0.0 and 1.0" << std::endl;
                }
                break;

            case 'b': {
                static bool battle_mode = false;
                battle_mode = !battle_mode;
                if (battle_mode) {
                    std::cout << "Transitioning to battle mode..." << std::endl;
                    // Fade in all instruments over 2 seconds
                    audio.FadeLayer(music_track, "flute", 0.7f, 2000ms);
                    audio.FadeLayer(music_track, "clarinet", 0.7f, 2000ms);
                    audio.FadeLayer(music_track, "horns", 0.8f, 2000ms);
                } else {
                    std::cout << "Transitioning to normal mode..." << std::endl;
                    // Fade out battle instruments over 3 seconds
                    audio.FadeLayer(music_track, "flute", 0.0f, 3000ms);
                    audio.FadeLayer(music_track, "clarinet", 0.0f, 3000ms);
                    audio.FadeLayer(music_track, "horns", 0.0f, 3000ms);
                }
                break;
            }
            
            case 'x':
                std::cout << "Playing sound effect..." << std::endl;
                audio.StartSound(sfx);  // Using the renamed function
                break;

            default:
                std::cout << "Unknown command. Use v/m/s for volumes, b for battle mode, q to quit." << std::endl;
                break;
        }
    } catch (...) {
        std::cout << "Invalid input. Format: command [value]" << std::endl;
    }
}
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
#include <cstdlib>
#include <ctime>

#include "../src/audio_manager.h"
#include "../src/audio_session.h"
using namespace std::chrono_literals;

bool digital_mode = true;
bool battle = false;

// Define default sound directory (can be overridden via compile definition)
#ifndef SOUND_FILES_DIR
#define SOUND_FILES_DIR "../../sound_files/"
#endif

/**
 * @brief Process user input commands for audio control
 * 
 * This function processes the following commands:
 * - v [0.0-1.0] - Set master volume
 * - m [0.0-1.0] - Set music volume
 * - s [0.0-1.0] - Set SFX volume
 * - x           - Play sound effect
 * - o           - Toggle music (fade in/out)
 * - b           - Toggle music mode (fade in/out)
 * - q           - Quit
 *
 * @param input User input string
 * @param music_track Handle to the music track
 * @param music_group Handle to the music group
 * @param sfx_group Handle to the SFX group
 * @param sfx Handle to the sound effect
 * @param sound_dir Directory containing sound files
 */
void ProcessInput(const std::string& input,
                 audio::TrackHandle music_track,
                 audio::GroupHandle music_group,
                 audio::GroupHandle sfx_group,
                 audio::SoundHandle sfx,
                 const std::string& sound_dir);

int main(int argc, char* argv[]) {
    try {
        // Seed the random number generator
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        // Get sound directory from command line or use compile-time default
        std::string sound_dir = SOUND_FILES_DIR;
        if (argc > 1) {
            sound_dir = argv[1];
            // Ensure trailing slash
            if (!sound_dir.empty() && sound_dir.back() != '/' && sound_dir.back() != '\\') {
                sound_dir += '/';
            }
        }
        
        std::cout << "Sound directory: " << sound_dir << "\n";
        std::cout << "Starting audio test program...\n";
        
        // Initialize the audio system (recommended RAII session)
        std::cout << "Initializing audio system...\n";
        audio::AudioSession session;
        auto& audio = audio::AudioManager::GetInstance();
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
        // Add all our layers to the music track
        audio.AddLayer(music_track, "digital_base", sound_dir + "digital_base.wav", "music");
        audio.AddLayer(music_track, "digital_battle", sound_dir + "digital_battle.wav", "music");
        audio.AddLayer(music_track, "strings_base", sound_dir + "strings_base.wav", "music");
        audio.AddLayer(music_track, "strings_battle", sound_dir + "strings_battle.wav", "music");
        std::cout << "All layers added successfully\n";
        
        std::cout << "Setting initial layer volumes...\n";
        // Start with digital base
        audio.SetLayerVolume(music_track, "digital_base", 1.0f);
        audio.SetLayerVolume(music_track, "digital_battle", 0.0f);
        std::cout << "Initial volumes set\n";
        audio.SetLayerVolume(music_track, "strings_base", 0.0f);
        audio.SetLayerVolume(music_track, "strings_battle", 0.0f);

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
        std::cout << "  o           - Toggle music (fade in/out)" << std::endl;
        std::cout << "  t           - Toggle music type (fade in/out)" << std::endl;
        std::cout << "  b           - Toggle music mode (fade in/out)" << std::endl;
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
                ProcessInput(input, music_track, music_group, sfx_group, sfx, sound_dir);
                input.clear();
            }

            // Sleep to prevent CPU spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        // Cleanup
        if (input_thread.joinable()) {
            input_thread.join();
        }
        session.Close();
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
                 audio::SoundHandle sfx,
                 const std::string& sound_dir) {
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

            case 'b': 
                if (battle) {
                    std::cout << "Transitioning to normal mode..." << std::endl;
                    if (digital_mode)
                    {
                        audio.FadeLayer(music_track, "digital_battle", 0.0f, 3000ms);
                    } else {
                        audio.FadeLayer(music_track, "strings_battle", 0.0f, 3000ms);
                    }
                } else {
                    std::cout << "Transitioning to battle mode..." << std::endl;
                    if (digital_mode)
                    {
                        audio.FadeLayer(music_track, "digital_battle", 1.0f, 3000ms);
                    } else {
                        audio.FadeLayer(music_track, "strings_battle", 1.0f, 3000ms);
                    }
                }
                battle = !battle;
                break;

            case 't':
                if (digital_mode) {
                    std::cout << "Transitioning to strings..." << std::endl;
                    audio.FadeLayer(music_track, "strings_base", 1.0f, 2000ms);
                    audio.FadeLayer(music_track, "digital_base", 0.0f, 3000ms);
                    if (battle) {
                        audio.FadeLayer(music_track, "digital_battle", 0.0f, 3000ms);
                        audio.FadeLayer(music_track, "strings_battle", 1.0f, 2000ms);
                    }
                } else {
                    std::cout << "Transitioning to digital..." << std::endl;
                    audio.FadeLayer(music_track, "digital_base", 0.8f, 2000ms);
                    audio.FadeLayer(music_track, "strings_base", 0.0f, 3000ms);
                    if (battle) {
                        audio.FadeLayer(music_track, "strings_battle", 0.0f, 3000ms);
                        audio.FadeLayer(music_track, "digital_battle", 0.8f, 2000ms);
                    }
                }
                digital_mode = !digital_mode;
                break;
            
            case 'x':
                std::cout << "Playing sound effect..." << std::endl;
                {
                    // Generate random number between 1 and 8
                    int random_num = (rand() % 8) + 1;
                    std::string filename = "touch_" + std::to_string(random_num) + ".wav";
                    audio.PlaySound(audio.LoadSound(sound_dir + filename));
                }
                break;

            default:
                std::cout << "Unknown command. Use v/m/s for volumes, b for battle mode, q to quit." << std::endl;
                break;
        }
    } catch (...) {
        std::cout << "Invalid input. Format: command [value]" << std::endl;
    }
}
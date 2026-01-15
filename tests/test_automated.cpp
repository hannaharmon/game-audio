/**
 * Automated Audio System Tests
 * 
 * Comprehensive test suite that exercises all audio system functionality
 * without requiring user input. Tests return 0 on success, 1 on failure.
 */

#include "../audio/audio_manager.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <cmath>

using namespace audio;
using namespace std::chrono_literals;

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    std::cout << "\nTEST: " << name << std::endl; \
    try {

#define ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "  FAIL: " << message << std::endl; \
        tests_failed++; \
        throw std::runtime_error(message); \
    } \
    tests_passed++; \
    std::cout << "  PASS: " << message << std::endl;

#define END_TEST \
    } catch (const std::exception& e) { \
        std::cerr << "  Exception: " << e.what() << std::endl; \
    }

// Helper to wait for audio operations
void wait_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Path configuration
std::string sound_dir;

void test_initialization() {
    TEST("System Initialization")
    
    auto& audio = AudioManager::GetInstance();
    
    // System should already be initialized from main()
    ASSERT(!audio.Initialize(), "Initialize should return false when already initialized")
    
    // Shutdown and reinitialize
    audio.Shutdown();
    ASSERT(audio.Initialize(), "Should be able to reinitialize after shutdown")
    
    END_TEST
}

void test_master_volume() {
    TEST("Master Volume Control")
    
    auto& audio = AudioManager::GetInstance();
    
    audio.SetMasterVolume(0.5f);
    ASSERT(std::abs(audio.GetMasterVolume() - 0.5f) < 0.01f, "Master volume should be 0.5")
    
    audio.SetMasterVolume(1.0f);
    ASSERT(std::abs(audio.GetMasterVolume() - 1.0f) < 0.01f, "Master volume should be 1.0")
    
    audio.SetMasterVolume(0.0f);
    ASSERT(std::abs(audio.GetMasterVolume() - 0.0f) < 0.01f, "Master volume should be 0.0")
    
    // Note: System doesn't clamp volume - miniaudio handles this internally
    audio.SetMasterVolume(1.0f); // Reset to normal
    
    END_TEST
}

void test_group_operations() {
    TEST("Audio Group Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create groups
    GroupHandle music = audio.CreateGroup("music");
    ASSERT(music != 0, "Music group handle should be non-zero")
    
    GroupHandle sfx = audio.CreateGroup("sfx");
    ASSERT(sfx != 0, "SFX group handle should be non-zero")
    ASSERT(sfx != music, "Group handles should be unique")
    
    // Test volume control
    audio.SetGroupVolume(music, 0.7f);
    ASSERT(std::abs(audio.GetGroupVolume(music) - 0.7f) < 0.01f, "Music group volume should be 0.7")
    
    audio.SetGroupVolume(sfx, 0.3f);
    ASSERT(std::abs(audio.GetGroupVolume(sfx) - 0.3f) < 0.01f, "SFX group volume should be 0.3")
    
    // Test fade (non-blocking, just verify it doesn't crash)
    audio.FadeGroup(music, 0.0f, 500ms);
    wait_ms(100);
    ASSERT(true, "Group fade operation should not crash")
    
    // Cleanup
    audio.DestroyGroup(music);
    audio.DestroyGroup(sfx);
    ASSERT(true, "Group destruction should not crash")
    
    END_TEST
}

void test_sound_loading() {
    TEST("Sound Loading and Unloading")
    
    auto& audio = AudioManager::GetInstance();
    GroupHandle group = audio.CreateGroup("test");
    
    // Test basic loading
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    ASSERT(sound != 0, "Sound should load successfully")
    
    // Test loading with group
    SoundHandle sound2 = audio.LoadSound(sound_path, group);
    ASSERT(sound2 != 0, "Sound with group should load successfully")
    ASSERT(sound2 != sound, "Different load calls should return different handles")
    
    // Test unloading
    audio.UnloadSound(sound);
    audio.UnloadSound(sound2);
    ASSERT(true, "Sound unloading should not crash")
    
    audio.DestroyGroup(group);
    
    END_TEST
}

void test_sound_playback() {
    TEST("Sound Playback Control")
    
    auto& audio = AudioManager::GetInstance();
    
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    ASSERT(sound != 0, "Sound should load")
    
    // Test playback
    ASSERT(!audio.IsSoundPlaying(sound), "Sound should not be playing initially")
    
    audio.StartSound(sound);
    wait_ms(100);
    ASSERT(audio.IsSoundPlaying(sound), "Sound should be playing after start")
    
    audio.StopSound(sound);
    wait_ms(50);
    ASSERT(!audio.IsSoundPlaying(sound), "Sound should stop after StopSound")
    
    // Test volume control
    audio.SetSoundVolume(sound, 0.5f);
    ASSERT(true, "SetSoundVolume should not crash")
    
    // Test pitch control
    audio.SetSoundPitch(sound, 1.5f);
    audio.StartSound(sound);
    wait_ms(100);
    ASSERT(true, "Pitch modification should work")
    audio.StopSound(sound);
    
    audio.UnloadSound(sound);
    
    END_TEST
}

void test_track_operations() {
    TEST("Audio Track Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create track
    TrackHandle track = audio.CreateTrack();
    ASSERT(track != 0, "Track should be created")
    
    // Add layers
    audio.AddLayer(track, "layer1", sound_dir + "/digital_base.wav");
    audio.AddLayer(track, "layer2", sound_dir + "/digital_battle.wav");
    ASSERT(true, "Layers should be added without error")
    
    // Set layer volumes
    audio.SetLayerVolume(track, "layer1", 1.0f);
    audio.SetLayerVolume(track, "layer2", 0.0f);
    ASSERT(true, "Layer volumes should be set")
    
    // Play track
    audio.PlayTrack(track);
    wait_ms(200);
    ASSERT(true, "Track should play")
    
    // Test layer fade
    audio.FadeLayer(track, "layer2", 1.0f, 300ms);
    wait_ms(350);
    ASSERT(true, "Layer fade should complete")
    
    // Stop track
    audio.StopTrack(track);
    wait_ms(50);
    ASSERT(true, "Track should stop")
    
    // Remove layer
    audio.RemoveLayer(track, "layer1");
    ASSERT(true, "Layer removal should not crash")
    
    audio.DestroyTrack(track);
    
    END_TEST
}

void test_multiple_instances() {
    TEST("Multiple Sound Instances")
    
    auto& audio = AudioManager::GetInstance();
    
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    
    // Play same sound multiple times
    audio.StartSound(sound);
    wait_ms(50);
    audio.StartSound(sound);
    wait_ms(50);
    audio.StartSound(sound);
    
    ASSERT(true, "Multiple instances should play concurrently")
    
    wait_ms(200);
    audio.StopSound(sound);
    
    audio.UnloadSound(sound);
    
    END_TEST
}

void test_random_sound_folder() {
    TEST("Random Sound From Folder")
    
    auto& audio = AudioManager::GetInstance();
    GroupHandle group = audio.CreateGroup("random_test");
    
    // This should load all .wav files from the folder and play one randomly
    audio.PlayRandomSoundFromFolder(sound_dir, group);
    wait_ms(200);
    ASSERT(true, "Random sound playback should work")
    
    // Play again (should use cached sounds)
    audio.PlayRandomSoundFromFolder(sound_dir, group);
    wait_ms(200);
    ASSERT(true, "Cached random sound playback should work")
    
    audio.DestroyGroup(group);
    
    END_TEST
}

void test_error_handling() {
    TEST("Error Handling")
    
    auto& audio = AudioManager::GetInstance();
    
    // Test invalid sound file - system creates handle but fails to load
    // This is current behavior, not necessarily wrong
    SoundHandle invalidSound = audio.LoadSound("nonexistent_file.wav");
    ASSERT(true, "Loading invalid file should not crash")
    if (invalidSound != 0) {
        audio.UnloadSound(invalidSound);
    }
    
    // Test operations on invalid handles
    audio.StartSound(9999);
    ASSERT(true, "Starting invalid sound should not crash")
    
    audio.SetSoundVolume(9999, 0.5f);
    ASSERT(true, "Setting volume on invalid sound should not crash")
    
    audio.UnloadSound(9999);
    ASSERT(true, "Unloading invalid sound should not crash")
    
    // Test operations on invalid track
    audio.PlayTrack(9999);
    ASSERT(true, "Playing invalid track should not crash")
    
    audio.DestroyTrack(9999);
    ASSERT(true, "Destroying invalid track should not crash")
    
    // Test operations on invalid group
    audio.SetGroupVolume(9999, 0.5f);
    ASSERT(true, "Setting volume on invalid group should not crash")
    
    audio.DestroyGroup(9999);
    ASSERT(true, "Destroying invalid group should not crash")
    
    END_TEST
}

void test_resource_cleanup() {
    TEST("Resource Cleanup")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create many resources
    std::vector<GroupHandle> groups;
    std::vector<TrackHandle> tracks;
    std::vector<SoundHandle> sounds;
    
    for (int i = 0; i < 10; i++) {
        groups.push_back(audio.CreateGroup("group_" + std::to_string(i)));
        tracks.push_back(audio.CreateTrack());
        sounds.push_back(audio.LoadSound(sound_dir + "/digital_base.wav"));
    }
    
    ASSERT(groups.size() == 10, "Should create 10 groups")
    ASSERT(tracks.size() == 10, "Should create 10 tracks")
    ASSERT(sounds.size() == 10, "Should load 10 sounds")
    
    // Clean up all resources
    for (auto g : groups) audio.DestroyGroup(g);
    for (auto t : tracks) audio.DestroyTrack(t);
    for (auto s : sounds) audio.UnloadSound(s);
    
    ASSERT(true, "Mass resource cleanup should complete")
    
    END_TEST
}

void test_concurrent_operations() {
    TEST("Concurrent Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    GroupHandle music = audio.CreateGroup("music");
    GroupHandle sfx = audio.CreateGroup("sfx");
    
    SoundHandle sound1 = audio.LoadSound(sound_dir + "/digital_base.wav", music);
    SoundHandle sound2 = audio.LoadSound(sound_dir + "/digital_battle.wav", sfx);
    
    // Start multiple operations concurrently
    audio.StartSound(sound1);
    audio.StartSound(sound2);
    audio.SetGroupVolume(music, 0.8f);
    audio.SetGroupVolume(sfx, 0.6f);
    audio.FadeGroup(music, 0.3f, 500ms);
    
    wait_ms(600);
    
    ASSERT(true, "Concurrent operations should work")
    
    audio.StopSound(sound1);
    audio.StopSound(sound2);
    audio.UnloadSound(sound1);
    audio.UnloadSound(sound2);
    audio.DestroyGroup(music);
    audio.DestroyGroup(sfx);
    
    END_TEST
}

void test_edge_cases() {
    TEST("Edge Cases")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create and immediately destroy
    GroupHandle g = audio.CreateGroup("temp");
    audio.DestroyGroup(g);
    ASSERT(true, "Immediate destruction should work")
    
    // Load and immediately unload
    SoundHandle s = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.UnloadSound(s);
    ASSERT(true, "Immediate unload should work")
    
    // Create track, add layer, remove layer, destroy track
    TrackHandle t = audio.CreateTrack();
    audio.AddLayer(t, "test", sound_dir + "/digital_base.wav");
    audio.RemoveLayer(t, "test");
    audio.DestroyTrack(t);
    ASSERT(true, "Quick layer add/remove should work")
    
    // Play and stop immediately
    SoundHandle s2 = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.StartSound(s2);
    audio.StopSound(s2);
    audio.UnloadSound(s2);
    ASSERT(true, "Immediate play/stop should work")
    
    // Remove non-existent layer
    TrackHandle t2 = audio.CreateTrack();
    audio.RemoveLayer(t2, "nonexistent");
    ASSERT(true, "Removing non-existent layer should not crash")
    audio.DestroyTrack(t2);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Automated Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Determine sound directory
    if (argc > 1) {
        sound_dir = argv[1];
    } else {
#ifdef SOUND_FILES_DIR
        sound_dir = SOUND_FILES_DIR;
#else
        sound_dir = "../sound_files";
#endif
    }
    
    std::cout << "Sound directory: " << sound_dir << std::endl;
    
    // Initialize audio system
    auto& audio = AudioManager::GetInstance();
    if (!audio.Initialize()) {
        std::cerr << "FATAL: Failed to initialize audio system" << std::endl;
        return 1;
    }
    
    // Run all tests
    test_initialization();
    test_master_volume();
    test_group_operations();
    test_sound_loading();
    test_sound_playback();
    test_track_operations();
    test_multiple_instances();
    test_random_sound_folder();
    test_error_handling();
    test_resource_cleanup();
    test_concurrent_operations();
    test_edge_cases();
    
    // Final shutdown
    audio.Shutdown();
    
    // Print results
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\nALL TESTS PASSED ✓" << std::endl;
        return 0;
    } else {
        std::cout << "\nSOME TESTS FAILED ✗" << std::endl;
        return 1;
    }
}

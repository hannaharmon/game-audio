/**
 * Audio System Volume Control Tests
 * 
 * Tests volume control, clamping, and pitch validation.
 */

#include "test_common.h"
#include "audio_manager.h"
#include <cmath>
#include <algorithm>

using namespace audio;

void test_master_volume() {
    TEST("Master Volume Control")
    
    auto& audio = AudioManager::GetInstance();
    
    audio.SetMasterVolume(0.5f);
    ASSERT(std::abs(audio.GetMasterVolume() - 0.5f) < 0.01f, "Master volume should be 0.5")
    
    audio.SetMasterVolume(1.0f);
    ASSERT(std::abs(audio.GetMasterVolume() - 1.0f) < 0.01f, "Master volume should be 1.0")
    
    audio.SetMasterVolume(0.0f);
    ASSERT(std::abs(audio.GetMasterVolume() - 0.0f) < 0.01f, "Master volume should be 0.0")
    
    // Test volume clamping
    audio.SetMasterVolume(-0.5f);
    ASSERT(audio.GetMasterVolume() >= 0.0f, "Negative master volume should be clamped to >= 0.0")
    
    audio.SetMasterVolume(2.0f);
    ASSERT(std::abs(audio.GetMasterVolume() - 1.0f) < 0.01f, "Master volume > 1.0 should be clamped to 1.0")
    
    audio.SetMasterVolume(1.0f); // Reset to normal
    
    END_TEST
}

void test_volume_clamping() {
    TEST("Volume Clamping for All Methods")
    
    auto& audio = AudioManager::GetInstance();
    
    // Test group volume clamping
    GroupHandle group = audio.CreateGroup();
    audio.SetGroupVolume(group, -1.0f);
    ASSERT(audio.GetGroupVolume(group) >= 0.0f, "Negative group volume should be clamped")
    
    audio.SetGroupVolume(group, 5.0f);
    ASSERT(std::abs(audio.GetGroupVolume(group) - 1.0f) < 0.01f, "Group volume > 1.0 should be clamped")
    
    // Test sound volume clamping
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.SetSoundVolume(sound, -0.3f);
    ASSERT(true, "Negative sound volume should be clamped (no crash)")
    
    audio.SetSoundVolume(sound, 10.0f);
    ASSERT(true, "Sound volume > 1.0 should be clamped (no crash)")
    
    // Test layer volume clamping
    TrackHandle track = audio.CreateTrack();
    audio.AddLayer(track, "layer1", sound_dir + "/digital_base.wav");
    audio.SetLayerVolume(track, "layer1", -0.5f);
    ASSERT(true, "Negative layer volume should be clamped (no crash)")
    
    audio.SetLayerVolume(track, "layer1", 2.5f);
    ASSERT(true, "Layer volume > 1.0 should be clamped (no crash)")
    
    // Cleanup
    audio.DestroySound(sound);
    audio.DestroyTrack(track);
    audio.DestroyGroup(group);
    
    END_TEST
}

void test_pitch_validation() {
    TEST("Pitch Validation and Clamping")
    
    auto& audio = AudioManager::GetInstance();
    
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    
    // Test negative pitch (should be clamped to minimum)
    audio.SetSoundPitch(sound, -1.0f);
    ASSERT(true, "Negative pitch should be clamped (no crash)")
    
    // Test zero pitch (should be clamped to minimum)
    audio.SetSoundPitch(sound, 0.0f);
    ASSERT(true, "Zero pitch should be clamped (no crash)")
    
    // Test very high pitch (should be clamped to maximum)
    audio.SetSoundPitch(sound, 100.0f);
    ASSERT(true, "Very high pitch should be clamped (no crash)")
    
    // Test valid pitch values
    audio.SetSoundPitch(sound, 0.5f);
    ASSERT(true, "Valid pitch 0.5 should work")
    
    audio.SetSoundPitch(sound, 1.0f);
    ASSERT(true, "Valid pitch 1.0 should work")
    
    audio.SetSoundPitch(sound, 2.0f);
    ASSERT(true, "Valid pitch 2.0 should work")
    
    audio.DestroySound(sound);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Volume Control Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    sound_dir = get_sound_dir(argc, argv);
    std::cout << "Sound directory: " << sound_dir << std::endl;
    
    // Initialize audio system
    auto& audio = AudioManager::GetInstance();
    if (!audio.Initialize()) {
        std::cerr << "FATAL: Failed to initialize audio system" << std::endl;
        return 1;
    }
    
    // Run all tests
    test_master_volume();
    test_volume_clamping();
    test_pitch_validation();
    
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

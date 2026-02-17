/**
 * Audio System Group Tests
 * 
 * Tests audio group operations including creation, volume control, and fading.
 */

#include "test_common.h"
#include "audio_manager.h"
#include <cmath>

using namespace audio;
using namespace std::chrono_literals;

void test_group_operations() {
    TEST("Audio Group Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create groups
    GroupHandle music = audio.CreateGroup();
    ASSERT(music.IsValid(), "Music group handle should be non-zero")
    
    GroupHandle sfx = audio.CreateGroup();
    ASSERT(sfx.IsValid(), "SFX group handle should be non-zero")
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

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Group Tests" << std::endl;
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
    test_group_operations();
    
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

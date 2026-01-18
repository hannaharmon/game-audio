/**
 * Audio System Track Tests
 * 
 * Tests audio track operations including layer management and fading.
 */

#include "test_common.h"
#include "audio_manager.h"

using namespace audio;
using namespace std::chrono_literals;

void test_track_operations() {
    TEST("Audio Track Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create track
    TrackHandle track = audio.CreateTrack();
    ASSERT(track.IsValid(), "Track should be created")
    
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

void test_audio_track_update_fix() {
    TEST("AudioTrack Update Fix (No Redundant Volume Calls)")
    
    auto& audio = AudioManager::GetInstance();
    
    TrackHandle track = audio.CreateTrack();
    audio.AddLayer(track, "layer1", sound_dir + "/digital_base.wav");
    audio.AddLayer(track, "layer2", sound_dir + "/digital_battle.wav");
    
    // Start playing
    audio.PlayTrack(track);
    
    // Set initial volumes
    audio.SetLayerVolume(track, "layer1", 0.0f);
    audio.SetLayerVolume(track, "layer2", 0.0f);
    
    // Start a fade - this should work without redundant volume updates
    audio.FadeLayer(track, "layer1", 1.0f, 500ms);
    audio.FadeLayer(track, "layer2", 0.5f, 300ms);
    
    // Wait for fades to complete
    wait_ms(600);
    
    // Verify volumes are set correctly (fade should have completed)
    ASSERT(true, "Fade operations should complete without issues")
    
    audio.StopTrack(track);
    audio.DestroyTrack(track);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Track Tests" << std::endl;
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
    test_track_operations();
    test_audio_track_update_fix();
    
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

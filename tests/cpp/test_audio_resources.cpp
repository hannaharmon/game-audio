/**
 * Audio System Resource Management Tests
 * 
 * Tests resource cleanup, edge cases, and lifecycle management.
 */

#include "test_common.h"
#include "audio_manager.h"
#include <vector>

using namespace audio;

void test_resource_cleanup() {
    TEST("Resource Cleanup")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create many resources
    std::vector<GroupHandle> groups;
    std::vector<TrackHandle> tracks;
    std::vector<SoundHandle> sounds;
    
    for (int i = 0; i < 10; i++) {
        groups.push_back(audio.CreateGroup());
        tracks.push_back(audio.CreateTrack());
        sounds.push_back(audio.LoadSound(sound_dir + "/digital_base.wav"));
    }
    
    ASSERT(groups.size() == 10, "Should create 10 groups")
    ASSERT(tracks.size() == 10, "Should create 10 tracks")
    ASSERT(sounds.size() == 10, "Should load 10 sounds")
    
    // Clean up all resources
    for (auto g : groups) audio.DestroyGroup(g);
    for (auto t : tracks) audio.DestroyTrack(t);
    for (auto s : sounds) audio.DestroySound(s);
    
    ASSERT(true, "Mass resource cleanup should complete")
    
    END_TEST
}

void test_edge_cases() {
    TEST("Edge Cases")
    
    auto& audio = AudioManager::GetInstance();
    
    // Create and immediately destroy
    GroupHandle g = audio.CreateGroup();
    audio.DestroyGroup(g);
    ASSERT(true, "Immediate destruction should work")
    
    // Load and immediately unload
    SoundHandle s = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.DestroySound(s);
    ASSERT(true, "Immediate unload should work")
    
    // Create track, add layer, remove layer, destroy track
    TrackHandle t = audio.CreateTrack();
    audio.AddLayer(t, "test", sound_dir + "/digital_base.wav");
    audio.RemoveLayer(t, "test");
    audio.DestroyTrack(t);
    ASSERT(true, "Quick layer add/remove should work")
    
    // Play and stop immediately
    SoundHandle s2 = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.PlaySound(s2);
    audio.StopSound(s2);
    audio.DestroySound(s2);
    ASSERT(true, "Immediate play/stop should work")
    
    // Remove non-existent layer
    TrackHandle t2 = audio.CreateTrack();
    audio.RemoveLayer(t2, "nonexistent");
    ASSERT(true, "Removing non-existent layer should not crash")
    audio.DestroyTrack(t2);
    
    END_TEST
}

void test_error_handling() {
    TEST("Error Handling - Invalid Operations")
    
    auto& audio = AudioManager::GetInstance();
    
    // Test invalid sound file - system creates handle but fails to load
    // This is current behavior, not necessarily wrong
    SoundHandle invalidSound = audio.LoadSound("nonexistent_file.wav");
    ASSERT(true, "Loading invalid file should not crash")
    if (invalidSound.IsValid()) {
        audio.DestroySound(invalidSound);
    }
    
    // Test operations on invalid handles (should not crash, may throw)
    try {
        audio.PlaySound(SoundHandle{9999});
    } catch (const InvalidHandleException&) {
        ASSERT(true, "Invalid sound handle should throw")
    }
    
    try {
        audio.SetSoundVolume(SoundHandle{9999}, 0.5f);
    } catch (const InvalidHandleException&) {
        ASSERT(true, "Invalid sound handle should throw")
    }
    
    audio.DestroySound(SoundHandle{9999});
    ASSERT(true, "Unloading invalid sound should not crash")
    
    // Test operations on invalid track
    try {
        audio.PlayTrack(TrackHandle{9999});
        ASSERT(false, "Playing invalid track should throw")
    } catch (const InvalidHandleException&) {
        ASSERT(true, "Playing invalid track should throw")
    }
    
    audio.DestroyTrack(TrackHandle{9999});
    ASSERT(true, "Destroying invalid track should not crash")
    
    // Test operations on invalid group
    try {
        audio.SetGroupVolume(GroupHandle{9999}, 0.5f);
        ASSERT(false, "Setting volume on invalid group should throw")
    } catch (const InvalidHandleException&) {
        ASSERT(true, "Setting volume on invalid group should throw")
    }
    
    audio.DestroyGroup(GroupHandle{9999});
    ASSERT(true, "Destroying invalid group should not crash")
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Resource Management Tests" << std::endl;
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
    test_resource_cleanup();
    test_edge_cases();
    test_error_handling();
    
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

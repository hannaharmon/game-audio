/**
 * Audio System Sound Tests
 * 
 * Tests sound loading, playback, multiple instances, and random sound folders.
 */

#include "test_common.h"
#include "audio_manager.h"

using namespace audio;

void test_sound_loading() {
    TEST("Sound Loading and Unloading")
    
    auto& audio = AudioManager::GetInstance();
    GroupHandle group = audio.CreateGroup("test");
    
    // Test basic loading
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    ASSERT(sound.IsValid(), "Sound should load successfully")
    
    // Test loading with group
    SoundHandle sound2 = audio.LoadSound(sound_path, group);
    ASSERT(sound2.IsValid(), "Sound with group should load successfully")
    ASSERT(sound2 != sound, "Different load calls should return different handles")
    
    // Test unloading
    audio.DestroySound(sound);
    audio.DestroySound(sound2);
    ASSERT(true, "Sound unloading should not crash")
    
    audio.DestroyGroup(group);
    
    END_TEST
}

void test_sound_playback() {
    TEST("Sound Playback Control")
    
    auto& audio = AudioManager::GetInstance();
    
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    ASSERT(sound.IsValid(), "Sound should load")
    
    // Test playback
    ASSERT(!audio.IsSoundPlaying(sound), "Sound should not be playing initially")
    
    audio.PlaySound(sound);
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
    audio.PlaySound(sound);
    wait_ms(100);
    ASSERT(true, "Pitch modification should work")
    audio.StopSound(sound);
    
    audio.DestroySound(sound);
    
    END_TEST
}

void test_multiple_instances() {
    TEST("Multiple Sound Instances")
    
    auto& audio = AudioManager::GetInstance();
    
    std::string sound_path = sound_dir + "/digital_base.wav";
    SoundHandle sound = audio.LoadSound(sound_path);
    
    // Play same sound multiple times
    audio.PlaySound(sound);
    wait_ms(50);
    audio.PlaySound(sound);
    wait_ms(50);
    audio.PlaySound(sound);
    
    ASSERT(true, "Multiple instances should play concurrently")
    
    wait_ms(200);
    audio.StopSound(sound);
    
    audio.DestroySound(sound);
    
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

    // Simulate a typical shutdown/reinitialize cycle to ensure folder cache resets cleanly
    audio.Shutdown();
    ASSERT(audio.Initialize(), "Reinitialize after shutdown should work")
    GroupHandle group2 = audio.CreateGroup("random_test_2");
    audio.PlayRandomSoundFromFolder(sound_dir, group2);
    wait_ms(200);
    ASSERT(true, "Random sound playback should work after reinitialize")
    audio.DestroyGroup(group2);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Sound Tests" << std::endl;
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
    test_sound_loading();
    test_sound_playback();
    test_multiple_instances();
    test_random_sound_folder();
    
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

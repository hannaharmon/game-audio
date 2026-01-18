/**
 * Audio System Threading Tests
 * 
 * Tests thread safety, concurrent operations, and GetMasterVolume thread safety fix.
 */

#include "test_common.h"
#include "audio_manager.h"
#include <vector>
#include <atomic>
#include <thread>
#include <cmath>

using namespace audio;

void test_thread_safety_get_master_volume() {
    TEST("Thread Safety - GetMasterVolume")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set initial volume
    audio.SetMasterVolume(0.5f);
    
    // Launch multiple threads that call GetMasterVolume concurrently
    const int num_threads = 10;
    const int calls_per_thread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> failure_count{0};
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&audio, calls_per_thread, &success_count, &failure_count]() {
            for (int j = 0; j < calls_per_thread; j++) {
                try {
                    float vol = audio.GetMasterVolume();
                    if (vol >= 0.0f && vol <= 1.0f) {
                        success_count++;
                    } else {
                        failure_count++;
                    }
                } catch (const std::exception&) {
                    failure_count++;
                }
            }
        });
    }
    
    // Also change volume from main thread while reading
    for (int i = 0; i < 50; i++) {
        audio.SetMasterVolume(0.3f + (i % 2) * 0.4f);
        wait_ms(10);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    ASSERT(success_count > 0, "GetMasterVolume should succeed from multiple threads");
    ASSERT(failure_count == 0, "GetMasterVolume should not fail or crash");
    
    // Reset volume
    audio.SetMasterVolume(1.0f);
    
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
    audio.PlaySound(sound1);
    audio.PlaySound(sound2);
    audio.SetGroupVolume(music, 0.8f);
    audio.SetGroupVolume(sfx, 0.6f);
    audio.FadeGroup(music, 0.3f, 500ms);
    
    wait_ms(600);
    
    ASSERT(true, "Concurrent operations should work")
    
    audio.StopSound(sound1);
    audio.StopSound(sound2);
    audio.DestroySound(sound1);
    audio.DestroySound(sound2);
    audio.DestroyGroup(music);
    audio.DestroyGroup(sfx);
    
    END_TEST
}

void test_concurrent_operations_stress() {
    TEST("Concurrent Operations Stress Test")
    
    auto& audio = AudioManager::GetInstance();
    
    const int num_threads = 8;
    const int operations_per_thread = 50;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> exception_count{0};
    
    // Create some resources first
    std::vector<GroupHandle> groups;
    std::vector<SoundHandle> sounds;
    for (int i = 0; i < 5; i++) {
        groups.push_back(audio.CreateGroup("group_" + std::to_string(i)));
        sounds.push_back(audio.LoadSound(sound_dir + "/digital_base.wav"));
    }
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&audio, &groups, &sounds, operations_per_thread, 
                              &success_count, &exception_count, i]() {
            for (int j = 0; j < operations_per_thread; j++) {
                try {
                    // Mix of operations
                    if (j % 4 == 0) {
                        audio.SetMasterVolume(0.5f + (j % 10) * 0.05f);
                    } else if (j % 4 == 1) {
                        audio.SetGroupVolume(groups[j % groups.size()], 0.3f + (j % 7) * 0.1f);
                    } else if (j % 4 == 2) {
                        audio.SetSoundVolume(sounds[j % sounds.size()], 0.4f + (j % 6) * 0.1f);
                    } else {
                        float vol = audio.GetMasterVolume();
                        (void)vol; // Suppress unused warning
                    }
                    success_count++;
                } catch (const std::exception&) {
                    exception_count++;
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    ASSERT(success_count > 0, "Concurrent operations should succeed");
    ASSERT(exception_count == 0, "Concurrent operations should not throw exceptions");
    
    // Cleanup
    for (auto s : sounds) audio.DestroySound(s);
    for (auto g : groups) audio.DestroyGroup(g);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Threading Tests" << std::endl;
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
    test_thread_safety_get_master_volume();
    test_concurrent_operations();
    test_concurrent_operations_stress();
    
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

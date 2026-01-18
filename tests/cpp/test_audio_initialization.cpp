/**
 * Audio System Initialization Tests
 * 
 * Tests system initialization, shutdown, AudioSession, and logging controls.
 */

#include "test_common.h"
#include "audio_manager.h"
#include "audio_session.h"
#include "logging.h"
#include <iostream>

using namespace audio;

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

void test_audio_session_usage() {
    TEST("AudioSession Usage")
    
    auto& audio = AudioManager::GetInstance();
    audio.Shutdown();
    
    {
        AudioSession session;
        GroupHandle group = audio.CreateGroup("session_group");
        ASSERT(group.IsValid(), "AudioSession should allow group creation")
        audio.SetGroupVolume(group, 0.5f);
        audio.DestroyGroup(group);
    }
    
    ASSERT(audio.Initialize(), "AudioManager should reinitialize after AudioSession scope")
    
    END_TEST
}

void test_logging_controls() {
    TEST("Logging Controls")
    
    auto original = AudioManager::GetLogLevel();
    AudioManager::SetLogLevel(LogLevel::Debug);
    ASSERT(AudioManager::GetLogLevel() == LogLevel::Debug, "SetLogLevel should update global level")
    ASSERT(Logger::IsEnabled(LogLevel::Info), "Info should be enabled at Debug level")
    
    AudioManager::SetLogLevel(LogLevel::Warn);
    ASSERT(!Logger::IsEnabled(LogLevel::Info), "Info should be disabled at Warn level")
    ASSERT(Logger::IsEnabled(LogLevel::Error), "Error should be enabled at Warn level")
    
    AudioManager::SetLogLevel(original);
    END_TEST
}

void test_rapid_shutdown_reinitialize() {
    TEST("Rapid Shutdown/Reinitialize Cycles")
    
    auto& audio = AudioManager::GetInstance();
    
    // Perform multiple shutdown/reinitialize cycles
    for (int i = 0; i < 5; i++) {
        audio.Shutdown();
        wait_ms(50);
        ASSERT(audio.Initialize(), "Should be able to reinitialize after shutdown")
        wait_ms(50);
        
        // Verify system works after reinitialize
        GroupHandle group = audio.CreateGroup("test");
        audio.SetGroupVolume(group, 0.5f);
        ASSERT(std::abs(audio.GetGroupVolume(group) - 0.5f) < 0.01f, 
               "System should work after reinitialize")
        audio.DestroyGroup(group);
    }
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Initialization Tests" << std::endl;
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
    test_initialization();
    test_audio_session_usage();
    test_logging_controls();
    test_rapid_shutdown_reinitialize();
    
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

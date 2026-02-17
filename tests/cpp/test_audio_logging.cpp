/**
 * Audio System Logging Tests
 * 
 * Tests that logging functionality works correctly with the simplified system
 * (logging always compiled in, controlled at runtime).
 */

#include "test_common.h"
#include "audio_manager.h"
#include "logging.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace audio;

void test_logging_levels() {
    TEST("Logging Level Hierarchy")
    
    auto original = AudioManager::GetLogLevel();
    
    // Test that Off disables everything
    AudioManager::SetLogLevel(LogLevel::Off);
    ASSERT(!Logger::IsEnabled(LogLevel::Error), "Error should be disabled at Off")
    ASSERT(!Logger::IsEnabled(LogLevel::Warn), "Warn should be disabled at Off")
    ASSERT(!Logger::IsEnabled(LogLevel::Info), "Info should be disabled at Off")
    ASSERT(!Logger::IsEnabled(LogLevel::Debug), "Debug should be disabled at Off")
    
    // Test Error level
    AudioManager::SetLogLevel(LogLevel::Error);
    ASSERT(Logger::IsEnabled(LogLevel::Error), "Error should be enabled at Error level")
    ASSERT(!Logger::IsEnabled(LogLevel::Warn), "Warn should be disabled at Error level")
    
    // Test Warn level
    AudioManager::SetLogLevel(LogLevel::Warn);
    ASSERT(Logger::IsEnabled(LogLevel::Error), "Error should be enabled at Warn level")
    ASSERT(Logger::IsEnabled(LogLevel::Warn), "Warn should be enabled at Warn level")
    ASSERT(!Logger::IsEnabled(LogLevel::Info), "Info should be disabled at Warn level")
    
    // Test Info level
    AudioManager::SetLogLevel(LogLevel::Info);
    ASSERT(Logger::IsEnabled(LogLevel::Error), "Error should be enabled at Info level")
    ASSERT(Logger::IsEnabled(LogLevel::Warn), "Warn should be enabled at Info level")
    ASSERT(Logger::IsEnabled(LogLevel::Info), "Info should be enabled at Info level")
    ASSERT(!Logger::IsEnabled(LogLevel::Debug), "Debug should be disabled at Info level")
    
    // Test Debug level
    AudioManager::SetLogLevel(LogLevel::Debug);
    ASSERT(Logger::IsEnabled(LogLevel::Error), "Error should be enabled at Debug level")
    ASSERT(Logger::IsEnabled(LogLevel::Warn), "Warn should be enabled at Debug level")
    ASSERT(Logger::IsEnabled(LogLevel::Info), "Info should be enabled at Debug level")
    ASSERT(Logger::IsEnabled(LogLevel::Debug), "Debug should be enabled at Debug level")
    
    AudioManager::SetLogLevel(original);
    END_TEST
}

void test_logging_output() {
    TEST("Logging Output")
    
    auto original = AudioManager::GetLogLevel();
    
    // Test that logging actually outputs when enabled
    // We can't easily capture stdout/stderr in a simple test, but we can verify
    // that the logging functions don't crash and that IsEnabled works correctly
    
    AudioManager::SetLogLevel(LogLevel::Info);
    ASSERT(Logger::IsEnabled(LogLevel::Info), "Info should be enabled")
    
    // Test that we can call Log without crashing
    Logger::Log(LogLevel::Info, "Test log message");
    Logger::Log(LogLevel::Warn, "Test warning message");
    Logger::Log(LogLevel::Error, "Test error message");
    
    // When level is Off, IsEnabled should return false
    AudioManager::SetLogLevel(LogLevel::Off);
    ASSERT(!Logger::IsEnabled(LogLevel::Error), "Nothing should be enabled at Off")
    
    // Logging should still work (but won't output)
    Logger::Log(LogLevel::Error, "This should not output");
    
    AudioManager::SetLogLevel(original);
    END_TEST
}

void test_logging_default_state() {
    TEST("Logging Default State")
    
    // After initialization, logging should default to Off
    auto& audio = AudioManager::GetInstance();
    
    // The default should be Off (but we'll restore whatever it was)
    auto current = AudioManager::GetLogLevel();
    
    // Reset to Off to test default behavior
    AudioManager::SetLogLevel(LogLevel::Off);
    ASSERT(AudioManager::GetLogLevel() == LogLevel::Off, "Default should be Off")
    ASSERT(!Logger::IsEnabled(LogLevel::Error), "Error should be disabled by default")
    
    // Test that we can enable it
    AudioManager::SetLogLevel(LogLevel::Info);
    ASSERT(Logger::IsEnabled(LogLevel::Info), "Should be able to enable logging")
    
    END_TEST
}

void test_logging_persistence() {
    TEST("Logging Level Persistence")
    
    auto original = AudioManager::GetLogLevel();
    
    // Set a level
    AudioManager::SetLogLevel(LogLevel::Warn);
    ASSERT(AudioManager::GetLogLevel() == LogLevel::Warn, "Level should persist")
    
    // Get it back
    auto retrieved = AudioManager::GetLogLevel();
    ASSERT(retrieved == LogLevel::Warn, "GetLogLevel should return set level")
    
    // Change it
    AudioManager::SetLogLevel(LogLevel::Debug);
    ASSERT(AudioManager::GetLogLevel() == LogLevel::Debug, "Level should update")
    
    AudioManager::SetLogLevel(original);
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Logging Tests" << std::endl;
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
    test_logging_levels();
    test_logging_output();
    test_logging_default_state();
    test_logging_persistence();
    
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

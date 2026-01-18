/**
 * Audio System Error Handling Tests
 * 
 * Tests that verify all exception throwing paths in the audio system.
 * Ensures that C++ exceptions are properly thrown with correct types and messages.
 */

#include "../src/audio_manager.h"
#include "../src/audio_session.h"
#include <iostream>
#include <cassert>
#include <string>
#include <stdexcept>

using namespace audio;

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    std::cout << "\nTEST: " << name << std::endl; \
    try {

#define ASSERT_THROWS(exception_type, code, message) \
    do { \
        bool threw_correct_exception = false; \
        try { \
            code; \
        } catch (const exception_type& e) { \
            threw_correct_exception = true; \
            std::cout << "  PASS: " << message << " (caught: " << e.what() << ")" << std::endl; \
            tests_passed++; \
        } catch (const std::exception& e) { \
            std::cerr << "  FAIL: " << message << " - Wrong exception type: " << e.what() << std::endl; \
            tests_failed++; \
        } \
        if (!threw_correct_exception) { \
            std::cerr << "  FAIL: " << message << " - No exception thrown" << std::endl; \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_NO_THROW(code, message) \
    do { \
        try { \
            code; \
            std::cout << "  PASS: " << message << std::endl; \
            tests_passed++; \
        } catch (const std::exception& e) { \
            std::cerr << "  FAIL: " << message << " - Unexpected exception: " << e.what() << std::endl; \
            tests_failed++; \
        } \
    } while(0)

#define END_TEST \
    } catch (const std::exception& e) { \
        std::cerr << "  Uncaught exception in test: " << e.what() << std::endl; \
        tests_failed++; \
    }

// Path configuration
std::string sound_dir;

void test_not_initialized() {
    TEST("Not Initialized Errors")
    
    AudioManager& manager = AudioManager::GetInstance();
    manager.Shutdown();  // Ensure system is not running
    
    ASSERT_THROWS(NotInitializedException,
        manager.SetMasterVolume(0.5f),
        "SetMasterVolume without Initialize throws NotInitializedException");
    
    ASSERT_THROWS(NotInitializedException,
        manager.CreateGroup("test_group"),
        "CreateGroup without Initialize throws NotInitializedException");
    
    ASSERT_THROWS(NotInitializedException,
        manager.CreateTrack(),
        "CreateTrack without Initialize throws NotInitializedException");
    
    ASSERT_THROWS(NotInitializedException,
        manager.LoadSound("some_file.wav"),
        "LoadSound without Initialize throws NotInitializedException");
    
    ASSERT_NO_THROW(
        manager.Initialize(),
        "Reinitialize after not initialized does not throw");
    
    END_TEST
}

void test_audio_session_lifecycle() {
    TEST("AudioSession Lifecycle")

    AudioManager& manager = AudioManager::GetInstance();
    manager.Shutdown();  // Ensure clean state

    AudioSession session;

    ASSERT_NO_THROW(
        manager.SetMasterVolume(0.5f),
        "AudioSession initializes audio system");

    session.Close();

    ASSERT_THROWS(NotInitializedException,
        manager.SetMasterVolume(0.5f),
        "AudioSession.Close shuts down when it owns initialization");

    END_TEST
}

void test_invalid_track_handle() {
    TEST("Invalid Track Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with completely invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlayTrack(0),
        "PlayTrack with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopTrack(0),
        "StopTrack with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.AddLayer(0, "layer1", sound_dir + "background_music.mp3"),
        "AddLayer with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.RemoveLayer(0, "layer1"),
        "RemoveLayer with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.SetLayerVolume(0, "layer1", 0.5f),
        "SetLayerVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlayTrack(999999),
        "PlayTrack with handle 999999 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopTrack(999999),
        "StopTrack with handle 999999 throws InvalidHandleException");
    
    END_TEST
}

void test_invalid_sound_handle() {
    TEST("Invalid Sound Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.StartSound(0),
        "StartSound with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopSound(0),
        "StopSound with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.SetSoundVolume(0, 0.5f),
        "SetSoundVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.StartSound(999999),
        "StartSound with handle 999999 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopSound(999999),
        "StopSound with handle 999999 throws InvalidHandleException");
    
    END_TEST
}

void test_invalid_group_handle() {
    TEST("Invalid Group Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.SetGroupVolume(0, 0.5f),
        "SetGroupVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.SetGroupVolume(999999, 0.5f),
        "SetGroupVolume with handle 999999 throws InvalidHandleException");
    
    END_TEST
}

void test_file_not_found() {
    TEST("File Not Found Errors")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test loading non-existent sound file
    ASSERT_THROWS(FileLoadException,
        manager.LoadSound("nonexistent_file.mp3"),
        "LoadSound with non-existent file throws FileLoadException");
    
    ASSERT_THROWS(FileLoadException,
        manager.LoadSound("does_not_exist.wav"),
        "LoadSound with invalid path throws FileLoadException");
    
    // Test loading with empty filename
    ASSERT_THROWS(FileLoadException,
        manager.LoadSound(""),
        "LoadSound with empty filename throws FileLoadException");
    
    // Create a track first, then try to add invalid layer
    uint32_t track = manager.CreateTrack();
    
    ASSERT_THROWS(FileLoadException,
        manager.AddLayer(track, "layer1", "nonexistent_layer.mp3"),
        "AddLayer with non-existent file throws FileLoadException");
    
    END_TEST
}

void test_valid_operations_no_exceptions() {
    TEST("Valid Operations Should Not Throw")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Load a valid sound
    uint32_t sound_handle = 0;
    ASSERT_NO_THROW(
        sound_handle = manager.LoadSound(sound_dir + "clap.wav"),
        "LoadSound with valid file does not throw");
    
    // Start and stop the sound
    ASSERT_NO_THROW(
        manager.StartSound(sound_handle),
        "StartSound with valid handle does not throw");
    
    ASSERT_NO_THROW(
        manager.StopSound(sound_handle),
        "StopSound with valid handle does not throw");
    
    // Create and manipulate track
    uint32_t track = 0;
    ASSERT_NO_THROW(
        track = manager.CreateTrack(),
        "CreateTrack does not throw");
    
    ASSERT_NO_THROW(
        manager.AddLayer(track, "layer1", sound_dir + "clap.wav"),
        "AddLayer with valid file does not throw");
    
    ASSERT_NO_THROW(
        manager.PlayTrack(track),
        "PlayTrack with valid handle does not throw");
    
    ASSERT_NO_THROW(
        manager.StopTrack(track),
        "StopTrack with valid handle does not throw");
    
    // Create and manipulate group
    uint32_t group = 0;
    ASSERT_NO_THROW(
        group = manager.CreateGroup("test_group"),
        "CreateGroup does not throw");
    
    ASSERT_NO_THROW(
        manager.SetGroupVolume(group, 0.5f),
        "SetGroupVolume with valid handle does not throw");
    
    END_TEST
}

void test_exception_messages() {
    TEST("Exception Messages Are Descriptive")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Check that exception messages contain useful information
    try {
        manager.PlayTrack(0);
        std::cerr << "  FAIL: Expected InvalidHandleException" << std::endl;
        tests_failed++;
    } catch (const InvalidHandleException& e) {
        std::string msg = e.what();
        if (msg.find("Invalid") != std::string::npos || 
            msg.find("handle") != std::string::npos) {
            std::cout << "  PASS: InvalidHandleException message is descriptive: " << msg << std::endl;
            tests_passed++;
        } else {
            std::cerr << "  FAIL: Exception message not descriptive: " << msg << std::endl;
            tests_failed++;
        }
    }
    
    try {
        manager.LoadSound("nonexistent.mp3");
        std::cerr << "  FAIL: Expected FileLoadException" << std::endl;
        tests_failed++;
    } catch (const FileLoadException& e) {
        std::string msg = e.what();
        if (msg.find("file") != std::string::npos || 
            msg.find("load") != std::string::npos ||
            msg.find("not found") != std::string::npos) {
            std::cout << "  PASS: FileLoadException message is descriptive: " << msg << std::endl;
            tests_passed++;
        } else {
            std::cerr << "  FAIL: Exception message not descriptive: " << msg << std::endl;
            tests_failed++;
        }
    }
    
    END_TEST
}

void test_multiple_invalid_operations() {
    TEST("Multiple Invalid Operations In Sequence")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Multiple invalid track operations should all throw
    for (int i = 0; i < 3; i++) {
        ASSERT_THROWS(InvalidHandleException,
            manager.PlayTrack(0),
            "Multiple PlayTrack calls with invalid handle all throw");
    }
    
    // Multiple invalid file loads should all throw
    for (int i = 0; i < 3; i++) {
        ASSERT_THROWS(FileLoadException,
            manager.LoadSound("nonexistent.mp3"),
            "Multiple LoadSound calls with invalid file all throw");
    }
    
    END_TEST
}

void test_exception_types_hierarchy() {
    TEST("Exception Type Hierarchy")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test that specific exceptions can be caught as AudioException
    try {
        manager.PlayTrack(0);
        std::cerr << "  FAIL: Expected exception to be thrown" << std::endl;
        tests_failed++;
    } catch (const AudioException& e) {
        std::cout << "  PASS: InvalidHandleException caught as AudioException base class" << std::endl;
        tests_passed++;
    }
    
    try {
        manager.LoadSound("nonexistent.mp3");
        std::cerr << "  FAIL: Expected exception to be thrown" << std::endl;
        tests_failed++;
    } catch (const AudioException& e) {
        std::cout << "  PASS: FileLoadException caught as AudioException base class" << std::endl;
        tests_passed++;
    }
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Error Handling Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Determine sound file directory
    #ifdef SOUND_FILES_DIR
        sound_dir = SOUND_FILES_DIR;
    #else
        sound_dir = "../sound_files/";
    #endif
    
    std::cout << "Using sound files from: " << sound_dir << std::endl;
    
    // Initialize audio system
    try {
        AudioManager& manager = AudioManager::GetInstance();
        manager.Initialize();
        std::cout << "Audio system initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize audio system: " << e.what() << std::endl;
        return 1;
    }
    
    // Run all error handling tests
    test_not_initialized();
    test_audio_session_lifecycle();
    test_invalid_track_handle();
    test_invalid_sound_handle();
    test_invalid_group_handle();
    test_file_not_found();
    test_valid_operations_no_exceptions();
    test_exception_messages();
    test_multiple_invalid_operations();
    test_exception_types_hierarchy();
    
    // Shutdown
    AudioManager& manager = AudioManager::GetInstance();
    manager.Shutdown();
    
    // Print results
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\nAll error handling tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome tests failed!" << std::endl;
        return 1;
    }
}

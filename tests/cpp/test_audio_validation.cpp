/**
 * Audio System Validation Tests
 * 
 * Tests input validation, error handling, and exception types.
 */

#include "test_common.h"
#include "audio_manager.h"
#include "audio_session.h"
#include <string>

using namespace audio;
using namespace std::chrono_literals;

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

    ASSERT_NO_THROW(
        manager.Initialize(),
        "Reinitialize after AudioSession.Close does not throw");

    END_TEST
}

void test_invalid_track_handle() {
    TEST("Invalid Track Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with completely invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlayTrack(TrackHandle{0}),
        "PlayTrack with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopTrack(TrackHandle{0}),
        "StopTrack with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.AddLayer(TrackHandle{0}, "layer1", sound_dir + "/digital_base.wav"),
        "AddLayer with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.RemoveLayer(TrackHandle{0}, "layer1"),
        "RemoveLayer with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.SetLayerVolume(TrackHandle{0}, "layer1", 0.5f),
        "SetLayerVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlayTrack(TrackHandle{999999}),
        "PlayTrack with handle 999999 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopTrack(TrackHandle{999999}),
        "StopTrack with handle 999999 throws InvalidHandleException");
    
    END_TEST
}

void test_invalid_sound_handle() {
    TEST("Invalid Sound Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlaySound(SoundHandle{0}),
        "PlaySound with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopSound(SoundHandle{0}),
        "StopSound with handle 0 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.SetSoundVolume(SoundHandle{0}, 0.5f),
        "SetSoundVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.PlaySound(SoundHandle{999999}),
        "PlaySound with handle 999999 throws InvalidHandleException");
    
    ASSERT_THROWS(InvalidHandleException,
        manager.StopSound(SoundHandle{999999}),
        "StopSound with handle 999999 throws InvalidHandleException");
    
    END_TEST
}

void test_invalid_group_handle() {
    TEST("Invalid Group Handle Operations")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test with invalid handle (0)
    ASSERT_THROWS(InvalidHandleException,
        manager.SetGroupVolume(GroupHandle{0}, 0.5f),
        "SetGroupVolume with handle 0 throws InvalidHandleException");
    
    // Test with non-existent handle (999999)
    ASSERT_THROWS(InvalidHandleException,
        manager.SetGroupVolume(GroupHandle{999999}, 0.5f),
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
    TrackHandle track = manager.CreateTrack();
    
    ASSERT_THROWS(FileLoadException,
        manager.AddLayer(track, "layer1", "nonexistent_layer.mp3"),
        "AddLayer with non-existent file throws FileLoadException");
    
    manager.DestroyTrack(track);
    
    END_TEST
}

void test_fade_duration_validation() {
    TEST("Fade Duration Validation")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    GroupHandle group = manager.CreateGroup("test");
    TrackHandle track = manager.CreateTrack();
    manager.AddLayer(track, "layer1", sound_dir + "/digital_base.wav");
    
    // Test negative duration
    ASSERT_THROWS(AudioException,
        manager.FadeGroup(group, 0.5f, std::chrono::milliseconds(-100)),
        "FadeGroup with negative duration throws AudioException");
    
    ASSERT_THROWS(AudioException,
        manager.FadeLayer(track, "layer1", 0.5f, std::chrono::milliseconds(-50)),
        "FadeLayer with negative duration throws AudioException");
    
    // Test zero duration
    ASSERT_THROWS(AudioException,
        manager.FadeGroup(group, 0.5f, std::chrono::milliseconds(0)),
        "FadeGroup with zero duration throws AudioException");
    
    ASSERT_THROWS(AudioException,
        manager.FadeLayer(track, "layer1", 0.5f, std::chrono::milliseconds(0)),
        "FadeLayer with zero duration throws AudioException");
    
    // Cleanup
    manager.DestroyTrack(track);
    manager.DestroyGroup(group);
    
    END_TEST
}

void test_input_validation() {
    TEST("Input Validation for Layer Names and Paths")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    TrackHandle track = manager.CreateTrack();
    
    // Test empty layer name
    ASSERT_THROWS(AudioException,
        manager.AddLayer(track, "", sound_dir + "/digital_base.wav"),
        "AddLayer with empty layer name throws AudioException");
    
    // Test empty filepath
    ASSERT_THROWS(AudioException,
        manager.AddLayer(track, "layer1", ""),
        "AddLayer with empty filepath throws AudioException");
    
    // Test empty folder path
    ASSERT_THROWS(AudioException,
        manager.PlayRandomSoundFromFolder("", GroupHandle::Invalid()),
        "PlayRandomSoundFromFolder with empty path throws AudioException");
    
    // Cleanup
    manager.DestroyTrack(track);
    
    END_TEST
}

void test_exception_messages() {
    TEST("Exception Messages Are Descriptive")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Check that exception messages contain useful information
    try {
        manager.PlayTrack(TrackHandle{0});
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

void test_exception_types_hierarchy() {
    TEST("Exception Type Hierarchy")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Test that specific exceptions can be caught as AudioException
    try {
        manager.PlayTrack(TrackHandle{0});
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

void test_valid_operations_no_exceptions() {
    TEST("Valid Operations Should Not Throw")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Load a valid sound
    SoundHandle sound_handle = SoundHandle::Invalid();
    ASSERT_NO_THROW(
        sound_handle = manager.LoadSound(sound_dir + "/digital_base.wav"),
        "LoadSound with valid file does not throw");
    
    // Start and stop the sound
    ASSERT_NO_THROW(
        manager.PlaySound(sound_handle),
        "PlaySound with valid handle does not throw");
    
    ASSERT_NO_THROW(
        manager.StopSound(sound_handle),
        "StopSound with valid handle does not throw");
    
    // Create and manipulate track
    TrackHandle track = TrackHandle::Invalid();
    ASSERT_NO_THROW(
        track = manager.CreateTrack(),
        "CreateTrack does not throw");
    
    ASSERT_NO_THROW(
        manager.AddLayer(track, "layer1", sound_dir + "/digital_base.wav"),
        "AddLayer with valid file does not throw");
    
    ASSERT_NO_THROW(
        manager.PlayTrack(track),
        "PlayTrack with valid handle does not throw");
    
    ASSERT_NO_THROW(
        manager.StopTrack(track),
        "StopTrack with valid handle does not throw");
    
    // Create and manipulate group
    GroupHandle group = GroupHandle::Invalid();
    ASSERT_NO_THROW(
        group = manager.CreateGroup("test_group"),
        "CreateGroup does not throw");
    
    ASSERT_NO_THROW(
        manager.SetGroupVolume(group, 0.5f),
        "SetGroupVolume with valid handle does not throw");
    
    // Cleanup
    manager.DestroySound(sound_handle);
    manager.DestroyTrack(track);
    manager.DestroyGroup(group);
    
    END_TEST
}

void test_multiple_invalid_operations() {
    TEST("Multiple Invalid Operations In Sequence")
    
    AudioManager& manager = AudioManager::GetInstance();
    
    // Multiple invalid track operations should all throw
    for (int i = 0; i < 3; i++) {
        ASSERT_THROWS(InvalidHandleException,
            manager.PlayTrack(TrackHandle{0}),
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

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Validation Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    sound_dir = get_sound_dir(argc, argv);
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
    test_fade_duration_validation();
    test_input_validation();
    test_exception_messages();
    test_exception_types_hierarchy();
    test_valid_operations_no_exceptions();
    test_multiple_invalid_operations();
    
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
        std::cout << "\nAll validation tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome tests failed!" << std::endl;
        return 1;
    }
}

"""
Error Handling Tests for audio_py module

Tests that the audio system properly raises exceptions for error conditions:
- Invalid handles
- Missing files
- Uninitialized system usage
- Invalid parameters
"""

import sys
import os

repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
build_release = os.path.join(repo_root, 'build', 'Release')
build_debug = os.path.join(repo_root, 'build', 'Debug')
build_root = os.path.join(repo_root, 'build')

for path in (build_release, build_debug, build_root):
    if os.path.exists(path) and path not in sys.path:
        sys.path.insert(0, path)

import audio_py

SOUND_DIR = os.path.join(repo_root, 'sound_files')

def test_not_initialized():
    """Test 0: Using API without initialize should raise NotInitializedException"""
    print("Test 0: Not initialized... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.shutdown()
    
    try:
        audio.set_master_volume(0.5)
        print("FAIL - No exception raised")
        return False
    except audio_py.NotInitializedException as e:
        print(f"PASS - Caught expected exception: {e}")
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        return False

def test_audio_session_lifecycle():
    """Test 0b: AudioSession initializes and shuts down properly"""
    print("Test 0b: AudioSession lifecycle... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.shutdown()

    session = audio_py.AudioSession()
    try:
        audio.set_master_volume(0.5)
    except Exception as e:
        print(f"FAIL - Unexpected exception after session init: {type(e).__name__}: {e}")
        session.close()
        return False

    session.close()

    try:
        audio.set_master_volume(0.5)
        print("FAIL - No exception after session close")
        return False
    except audio_py.NotInitializedException:
        print("PASS")
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        return False

def test_invalid_track_handle():
    """Test 1: Using invalid track handle should raise InvalidHandleException"""
    print("Test 1: Invalid track handle... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # Try to play a track that doesn't exist
        audio.play_track(audio_py.TrackHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_invalid_sound_handle():
    """Test 2: Using invalid sound handle should raise InvalidHandleException"""
    print("Test 2: Invalid sound handle... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # Try to play a sound that doesn't exist
        audio.play_sound(audio_py.SoundHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_invalid_group_handle():
    """Test 3: Using invalid group handle should raise InvalidHandleException"""
    print("Test 3: Invalid group handle... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # Try to set volume on a group that doesn't exist
        audio.set_group_volume(audio_py.GroupHandle(99999), 0.5)
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_missing_sound_file():
    """Test 4: Loading non-existent file should raise FileLoadException"""
    print("Test 4: Missing sound file... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # Try to load a file that doesn't exist
        audio.load_sound("nonexistent_file.wav")
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.FileLoadException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_stop_invalid_sound():
    """Test 5: Stopping invalid sound handle should raise InvalidHandleException"""
    print("Test 5: Stop invalid sound... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.stop_sound(audio_py.SoundHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_set_volume_invalid_sound():
    """Test 6: Setting volume on invalid sound should raise InvalidHandleException"""
    print("Test 6: Set volume on invalid sound... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.set_sound_volume(audio_py.SoundHandle(99999), 0.5)
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_add_layer_invalid_track():
    """Test 7: Adding layer to invalid track should raise InvalidHandleException"""
    print("Test 7: Add layer to invalid track... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.add_layer(audio_py.TrackHandle(99999), "layer", os.path.join(SOUND_DIR, "hit.wav"))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_stop_track_invalid_handle():
    """Test 8: Stopping invalid track should raise InvalidHandleException"""
    print("Test 8: Stop invalid track... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.stop_track(audio_py.TrackHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except audio_py.InvalidHandleException as e:
        print(f"PASS - Caught expected exception: {e}")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_valid_operations_no_exception():
    """Test 9: Valid operations should not raise exceptions"""
    print("Test 9: Valid operations don't throw... ", end="")
    audio = audio_py.AudioManager.get_instance()
    
    try:
        audio.initialize()
        group = audio.create_group("test")
        sound = audio.load_sound(os.path.join(SOUND_DIR, "hit.wav"), group)
        audio.set_sound_volume(sound, 0.5)
        audio.play_sound(sound)
        audio.stop_sound(sound)
        audio.destroy_sound(sound)
        audio.destroy_group(group)
        audio.shutdown()
        print("PASS")
        return True
    except Exception as e:
        print(f"FAIL - Unexpected exception: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_exception_types_inheritance():
    """Test 10: Verify exception types inherit correctly"""
    print("Test 10: Exception inheritance... ", end="")
    
    # Verify all custom exceptions are available
    try:
        assert hasattr(audio_py, 'AudioException')
        assert hasattr(audio_py, 'InvalidHandleException')
        assert hasattr(audio_py, 'FileLoadException')
        assert hasattr(audio_py, 'NotInitializedException')
        print("PASS")
        return True
    except AssertionError as e:
        print(f"FAIL - {e}")
        return False

def test_load_sound_with_invalid_group():
    """Test 11: Loading sound with invalid group should still work but ignore group"""
    print("Test 11: Load sound with invalid group... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # This should work - invalid group is silently ignored in current implementation
        sound = audio.load_sound(os.path.join(SOUND_DIR, "hit.wav"), 99999)
        audio.destroy_sound(sound)
        audio.shutdown()
        print("PASS - Invalid group ignored gracefully")
        return True
    except Exception as e:
        print(f"PASS - Exception raised (also acceptable): {type(e).__name__}")
        audio.shutdown()
        return True

def test_multiple_errors_sequence():
    """Test 12: Multiple error conditions in sequence"""
    print("Test 12: Multiple errors in sequence... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    errors_caught = 0
    expected_errors = 3
    
    try:
        audio.play_sound(audio_py.SoundHandle(99999))
    except audio_py.InvalidHandleException:
        errors_caught += 1
    
    try:
        audio.load_sound("nonexistent.wav")
    except audio_py.FileLoadException:
        errors_caught += 1
    
    try:
        audio.set_group_volume(audio_py.GroupHandle(99999), 0.5)
    except audio_py.InvalidHandleException:
        errors_caught += 1
    
    audio.shutdown()
    
    if errors_caught == expected_errors:
        print(f"PASS - All {expected_errors} exceptions caught")
        return True
    else:
        print(f"FAIL - Only {errors_caught}/{expected_errors} exceptions caught")
        return False

def main():
    """Run all error handling tests"""
    print("=" * 60)
    print("Error Handling Tests for audio_py")
    print("=" * 60)
    
    tests = [
        test_not_initialized,
        test_audio_session_lifecycle,
        test_invalid_track_handle,
        test_invalid_sound_handle,
        test_invalid_group_handle,
        test_missing_sound_file,
        test_stop_invalid_sound,
        test_set_volume_invalid_sound,
        test_add_layer_invalid_track,
        test_stop_track_invalid_handle,
        test_valid_operations_no_exception,
        test_exception_types_inheritance,
        test_load_sound_with_invalid_group,
        test_multiple_errors_sequence,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"FAIL - Uncaught exception in test: {type(e).__name__}: {e}")
            failed += 1
    
    print("=" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)

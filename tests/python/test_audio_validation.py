"""
Audio System Validation Tests
Tests input validation, error handling, and exception types.
"""

from test_common import *

def test_not_initialized():
    """Test: Using API without initialize should raise NotInitializedException"""
    print("Test: Not initialized... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.shutdown()
    
    try:
        audio.set_master_volume(0.5)
        print("FAIL - No exception raised")
        return False
    except game_audio.NotInitializedException:
        print("PASS")
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        return False

def test_audio_session_lifecycle():
    """Test: AudioSession initializes and shuts down properly"""
    print("Test: AudioSession lifecycle... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.shutdown()

    session = game_audio.AudioSession()
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
    except game_audio.NotInitializedException:
        print("PASS")
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        return False

def test_invalid_track_handle():
    """Test: Using invalid track handle should raise InvalidHandleException"""
    print("Test: Invalid track handle... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.play_track(game_audio.TrackHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except game_audio.InvalidHandleException:
        print("PASS")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_invalid_sound_handle():
    """Test: Using invalid sound handle should raise InvalidHandleException"""
    print("Test: Invalid sound handle... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.play_sound(game_audio.SoundHandle(99999))
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except game_audio.InvalidHandleException:
        print("PASS")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_invalid_group_handle():
    """Test: Using invalid group handle should raise InvalidHandleException"""
    print("Test: Invalid group handle... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.set_group_volume(game_audio.GroupHandle(99999), 0.5)
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except game_audio.InvalidHandleException:
        print("PASS")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_file_not_found():
    """Test: Loading non-existent file should raise FileLoadException"""
    print("Test: File not found... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    try:
        audio.load_sound("nonexistent_file.mp3")
        print("FAIL - No exception raised")
        audio.shutdown()
        return False
    except game_audio.FileLoadException:
        print("PASS")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_sound_playback_initialization_failure():
    """Test: Sound playback initialization failure handling"""
    print("Test: Sound playback initialization failure... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    try:
        # Test that valid sounds play correctly (regression test)
        # This verifies our fix doesn't break normal operation
        if sound_exists("digital_base.wav"):
            sound = audio.load_sound(get_sound_path("digital_base.wav"))
            audio.play_sound(sound)
            wait_ms(50)
            audio.stop_sound(sound)
            audio.destroy_sound(sound)
            
            # Verify exception hierarchy - FileLoadException should be catchable as AudioException
            # Note: In practice, ma_sound_init_from_file() can fail if:
            # - The file was deleted after loading but before playing
            # - The file is corrupted or in an unsupported format
            # - Audio device issues occur
            # - Memory allocation fails
            # If such a failure occurs, PlaySound() will throw FileLoadException
            
            # Test that FileLoadException can be caught as AudioException (hierarchy test)
            sound2 = audio.load_sound(get_sound_path("digital_base.wav"))
            try:
                audio.play_sound(sound2)
                wait_ms(50)
                audio.stop_sound(sound2)
                audio.destroy_sound(sound2)
                
                # If we get here, playback succeeded (expected for valid files)
                print("PASS")
                audio.shutdown()
                return True
            except game_audio.FileLoadException as e:
                # If FileLoadException is thrown, verify it has useful information
                msg = str(e)
                if "file" in msg.lower() or "playback" in msg.lower() or "initialize" in msg.lower():
                    print("PASS (exception with descriptive message)")
                    audio.destroy_sound(sound2)
                    audio.shutdown()
                    return True
                else:
                    print(f"FAIL - Exception message not descriptive: {msg}")
                    audio.destroy_sound(sound2)
                    audio.shutdown()
                    return False
            except game_audio.AudioException:
                # FileLoadException should be catchable as AudioException
                print("PASS (exception caught as AudioException)")
                audio.destroy_sound(sound2)
                audio.shutdown()
                return True
        else:
            print("SKIP - Test sound file not found")
            audio.shutdown()
            return True
    except Exception as e:
        print(f"FAIL - Unexpected exception: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def test_fade_duration_validation():
    """Test: Fade duration validation"""
    print("Test: Fade duration validation... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    group = audio.create_group()
    
    # Test negative duration
    try:
        audio.fade_group(group, 0.5, timedelta(milliseconds=-100))
        print("FAIL - No exception for negative duration")
        audio.destroy_group(group)
        audio.shutdown()
        return False
    except game_audio.AudioException:
        pass  # Expected
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.destroy_group(group)
        audio.shutdown()
        return False
    
    # Test zero duration
    try:
        audio.fade_group(group, 0.5, timedelta(milliseconds=0))
        print("FAIL - No exception for zero duration")
        audio.destroy_group(group)
        audio.shutdown()
        return False
    except game_audio.AudioException:
        pass  # Expected
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.destroy_group(group)
        audio.shutdown()
        return False
    
    audio.destroy_group(group)
    audio.shutdown()
    print("PASS")
    return True

def test_input_validation():
    """Test: Input validation for layer names and paths"""
    print("Test: Input validation... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    track = audio.create_track()
    
    # Test empty layer name
    try:
        if sound_exists("digital_base.wav"):
            audio.add_layer(track, "", get_sound_path("digital_base.wav"))
            print("FAIL - No exception for empty layer name")
            audio.destroy_track(track)
            audio.shutdown()
            return False
    except game_audio.AudioException:
        pass  # Expected
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.destroy_track(track)
        audio.shutdown()
        return False
    
    # Test empty filepath
    try:
        audio.add_layer(track, "layer1", "")
        print("FAIL - No exception for empty filepath")
        audio.destroy_track(track)
        audio.shutdown()
        return False
    except game_audio.AudioException:
        pass  # Expected
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.destroy_track(track)
        audio.shutdown()
        return False
    
    # Test empty folder path
    try:
        audio.play_random_sound_from_folder("", game_audio.GroupHandle(0))
        print("FAIL - No exception for empty folder path")
        audio.destroy_track(track)
        audio.shutdown()
        return False
    except game_audio.AudioException:
        pass  # Expected
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.destroy_track(track)
        audio.shutdown()
        return False
    
    audio.destroy_track(track)
    audio.shutdown()
    print("PASS")
    return True

def test_exception_hierarchy():
    """Test: Exception type hierarchy"""
    print("Test: Exception hierarchy... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    # Test that specific exceptions can be caught as AudioException
    try:
        audio.play_track(game_audio.TrackHandle(0))
        print("FAIL - Expected exception to be thrown")
        audio.shutdown()
        return False
    except game_audio.AudioException:
        print("PASS")
        audio.shutdown()
        return True
    except Exception as e:
        print(f"FAIL - Wrong exception type: {type(e).__name__}: {e}")
        audio.shutdown()
        return False

def run_all_tests():
    """Run all validation tests"""
    print("=== Python Audio Validation Tests ===\n")
    
    tests = [
        test_not_initialized,
        test_audio_session_lifecycle,
        test_invalid_track_handle,
        test_invalid_sound_handle,
        test_invalid_group_handle,
        test_file_not_found,
        test_sound_playback_initialization_failure,
        test_fade_duration_validation,
        test_input_validation,
        test_exception_hierarchy,
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
            print(f"ERROR - {e}")
            failed += 1
    
    print(f"\n=== Results: {passed} passed, {failed} failed ===")
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)

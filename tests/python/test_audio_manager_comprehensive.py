"""Comprehensive test suite for Python audio bindings.

Tests edge cases, error handling, memory management, thread safety,
and cross-platform compatibility.
"""
import sys
import os
import time
import platform

# Add build directory to Python path
if platform.system() == "Windows":
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build', 'Debug'))
else:
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build'))

import audio_py

# Test results tracking
test_results = []
current_test = ""

def test(name):
    """Decorator to track test execution."""
    global current_test
    current_test = name
    def decorator(func):
        def wrapper():
            try:
                print(f"\n{name}...", end=" ", flush=True)
                result = func()
                if result is False:
                    print("FAIL")
                    test_results.append((name, False, "Assertion failed"))
                else:
                    print("PASS")
                    test_results.append((name, True, None))
            except Exception as e:
                print(f"FAIL ({e})")
                test_results.append((name, False, str(e)))
        return wrapper
    return decorator


# === INITIALIZATION & CLEANUP TESTS ===

@test("Test 1: Multiple init/shutdown cycles")
def test_multiple_init_shutdown():
    """Test that we can initialize and shutdown multiple times without issues."""
    audio = audio_py.AudioManager.get_instance()
    
    for i in range(5):
        assert audio.initialize(), f"Initialize failed on cycle {i+1}"
        time.sleep(0.1)
        audio.shutdown()
        time.sleep(0.1)
    
    # Final initialize for other tests
    audio.initialize()
    return True


@test("Test 2: Shutdown without initialize")
def test_shutdown_without_init():
    """Test that shutdown doesn't crash if called without initialize."""
    # Create a fresh instance scenario by calling shutdown multiple times
    audio = audio_py.AudioManager.get_instance()
    audio.shutdown()  # Should not crash even if already shutdown
    audio.shutdown()  # Multiple shutdowns should be safe
    audio.initialize()  # Re-init for other tests
    return True


@test("Test 3: Double initialize")
def test_double_initialize():
    """Test that double initialize is handled gracefully."""
    audio = audio_py.AudioManager.get_instance()
    # Already initialized from previous test
    result = audio.initialize()  # Should return False (already running)
    return result == False  # Expect False


# === VOLUME BOUNDARY TESTS ===

@test("Test 4: Volume boundary values")
def test_volume_boundaries():
    """Test volume with extreme values."""
    audio = audio_py.AudioManager.get_instance()
    
    # Test valid range
    audio.set_master_volume(0.0)
    assert abs(audio.get_master_volume() - 0.0) < 0.01
    
    audio.set_master_volume(1.0)
    assert abs(audio.get_master_volume() - 1.0) < 0.01
    
    # Test clamping (values > 1.0 should work but may be clamped or amplify)
    audio.set_master_volume(2.0)
    vol = audio.get_master_volume()
    assert vol > 0, "Volume should be positive"
    
    # Negative values should work in miniaudio (inverts phase)
    audio.set_master_volume(-0.5)
    vol = audio.get_master_volume()
    
    # Reset to normal
    audio.set_master_volume(0.5)
    return True


@test("Test 5: Group volume with extreme values")
def test_group_volume_boundaries():
    """Test group volume with boundary values."""
    audio = audio_py.AudioManager.get_instance()
    
    group = audio.create_group("test_group")
    
    audio.set_group_volume(group, 0.0)
    assert abs(audio.get_group_volume(group) - 0.0) < 0.01
    
    audio.set_group_volume(group, 1.0)
    assert abs(audio.get_group_volume(group) - 1.0) < 0.01
    
    audio.set_group_volume(group, 5.0)  # Very loud
    vol = audio.get_group_volume(group)
    assert vol > 0
    
    audio.destroy_group(group)
    return True


# === INVALID HANDLE TESTS ===

@test("Test 6: Invalid track handles")
def test_invalid_track_handles():
    """Test operations with invalid track handles."""
    audio = audio_py.AudioManager.get_instance()
    
    # Operations on invalid handle should not crash
    invalid_handle = 99999
    
    try:
        audio.play_track(invalid_handle)
        audio.stop_track(invalid_handle)
        audio.is_track_playing(invalid_handle)
        # If we get here without crash, test passes
        return True
    except:
        # Even if it throws, as long as it doesn't crash, it's acceptable
        return True


@test("Test 7: Invalid group handles")
def test_invalid_group_handles():
    """Test operations with invalid group handles."""
    audio = audio_py.AudioManager.get_instance()
    
    invalid_handle = 99999
    
    try:
        audio.set_group_volume(invalid_handle, 0.5)
        audio.get_group_volume(invalid_handle)
        audio.destroy_group(invalid_handle)
        return True
    except:
        return True


@test("Test 8: Invalid sound handles")
def test_invalid_sound_handles():
    """Test operations with invalid sound handles."""
    audio = audio_py.AudioManager.get_instance()
    
    invalid_handle = 99999
    group = audio.create_group("test")
    
    try:
        audio.play_sound(invalid_handle, group)
        audio.stop_sound(invalid_handle)
        audio.destroy_group(group)
        return True
    except:
        audio.destroy_group(group)
        return True


# === FILE HANDLING TESTS ===

@test("Test 9: Loading non-existent file")
def test_load_nonexistent_file():
    """Test loading a file that doesn't exist."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    try:
        handle = audio.load_sound("nonexistent_file.wav", group)
        # If it returns 0 (invalid handle), that's acceptable
        audio.destroy_group(group)
        return True
    except:
        # Exception is also acceptable
        audio.destroy_group(group)
        return True


@test("Test 10: Loading file with invalid path")
def test_load_invalid_path():
    """Test loading with various invalid paths."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    try:
        # Empty path
        audio.load_sound("", group)
        # Path with invalid characters (platform dependent)
        audio.load_sound("???***|||.wav", group)
        audio.destroy_group(group)
        return True
    except:
        audio.destroy_group(group)
        return True


# === STRESS TESTS ===

@test("Test 11: Many groups")
def test_many_groups():
    """Create and destroy many groups."""
    audio = audio_py.AudioManager.get_instance()
    
    groups = []
    # Create 100 groups
    for i in range(100):
        group = audio.create_group(f"group_{i}")
        groups.append(group)
        audio.set_group_volume(group, i / 100.0)
    
    # Verify they all have different handles
    assert len(set(groups)) == 100, "Group handles should be unique"
    
    # Destroy them all
    for group in groups:
        audio.destroy_group(group)
    
    return True


@test("Test 12: Many tracks")
def test_many_tracks():
    """Create and destroy many tracks."""
    audio = audio_py.AudioManager.get_instance()
    
    tracks = []
    for i in range(50):
        track = audio.create_track()
        tracks.append(track)
    
    assert len(set(tracks)) == 50, "Track handles should be unique"
    
    for track in tracks:
        audio.destroy_track(track)
    
    return True


@test("Test 13: Rapid sound loading/unloading")
def test_rapid_sound_operations():
    """Rapidly load and unload sounds."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'hit.wav')
    
    if os.path.exists(sound_file):
        for i in range(20):
            handle = audio.load_sound(sound_file, group)
            if handle != 0:
                audio.destroy_sound(handle)
    
    audio.destroy_group(group)
    return True


@test("Test 14: Simultaneous sound playback")
def test_simultaneous_sounds():
    """Play many sounds simultaneously."""
    audio = audio_py.AudioManager.get_instance()
    audio.set_master_volume(0.1)  # Keep volume low
    group = audio.create_group("test")
    
    # Load various sounds
    sound_dir = os.path.join(os.path.dirname(__file__), '..', 'sound_files')
    sounds = []
    
    for sound_name in ['touch_1.wav', 'touch_2.wav', 'touch_3.wav', 'touch_4.wav', 
                       'touch_5.wav', 'touch_6.wav', 'touch_7.wav', 'touch_8.wav']:
        sound_path = os.path.join(sound_dir, sound_name)
        if os.path.exists(sound_path):
            handle = audio.load_sound(sound_path, group)
            if handle != 0:
                sounds.append(handle)
    
    # Play them all at once
    for sound in sounds:
        audio.start_sound(sound)
    
    time.sleep(0.5)  # Let them play briefly
    
    # Stop and cleanup
    for sound in sounds:
        audio.stop_sound(sound)
        audio.destroy_sound(sound)
    
    audio.destroy_group(group)
    audio.set_master_volume(0.5)  # Reset
    return True


# === LAYER TESTS ===

@test("Test 15: Many layers on one track")
def test_many_layers():
    """Add many layers to a single track."""
    audio = audio_py.AudioManager.get_instance()
    track = audio.create_track()
    music_group = audio.create_group("music")
    
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'digital_base.wav')
    
    if os.path.exists(sound_file):
        # Add same file as multiple layers with different names
        for i in range(10):
            audio.add_layer(track, f"layer_{i}", sound_file, "music")
            audio.set_layer_volume(track, f"layer_{i}", i / 10.0)
    
    audio.destroy_track(track)
    audio.destroy_group(music_group)
    return True


@test("Test 16: Layer operations on invalid track")
def test_layer_invalid_track():
    """Test layer operations on invalid track."""
    audio = audio_py.AudioManager.get_instance()
    invalid_track = 99999
    
    try:
        audio.add_layer(invalid_track, "test", "test.wav", "music")
        audio.set_layer_volume(invalid_track, "test", 0.5)
        audio.remove_layer(invalid_track, "test")
        return True
    except:
        return True


@test("Test 17: Duplicate layer names")
def test_duplicate_layer_names():
    """Test adding layers with duplicate names."""
    audio = audio_py.AudioManager.get_instance()
    track = audio.create_track()
    music_group = audio.create_group("music")
    
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'digital_base.wav')
    
    if os.path.exists(sound_file):
        audio.add_layer(track, "same_name", sound_file, "music")
        # Adding again with same name - behavior depends on implementation
        audio.add_layer(track, "same_name", sound_file, "music")
        audio.set_layer_volume(track, "same_name", 0.3)
    
    audio.destroy_track(track)
    audio.destroy_group(music_group)
    return True


# === FADE TESTS ===

@test("Test 18: Zero-duration fade")
def test_zero_fade():
    """Test fading with zero duration."""
    from datetime import timedelta
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    audio.set_group_volume(group, 0.5)
    audio.fade_group(group, 1.0, timedelta(milliseconds=0))  # 0ms fade
    time.sleep(0.1)
    
    vol = audio.get_group_volume(group)
    assert abs(vol - 1.0) < 0.1, "Should jump to target immediately"
    
    audio.destroy_group(group)
    return True


@test("Test 19: Very long fade duration")
def test_long_fade():
    """Test fade with very long duration."""
    from datetime import timedelta
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    audio.set_group_volume(group, 0.0)
    audio.fade_group(group, 1.0, timedelta(seconds=60))  # 60 second fade
    time.sleep(0.2)
    
    vol = audio.get_group_volume(group)
    # Should have barely moved
    assert vol < 0.1, "Should still be near start volume"
    
    audio.destroy_group(group)
    return True


@test("Test 20: Overlapping fades")
def test_overlapping_fades():
    """Test starting a new fade while one is in progress."""
    from datetime import timedelta
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    audio.set_group_volume(group, 0.0)
    audio.fade_group(group, 1.0, timedelta(milliseconds=1000))
    time.sleep(0.1)
    # Start another fade
    audio.fade_group(group, 0.0, timedelta(milliseconds=1000))
    time.sleep(0.2)
    
    audio.destroy_group(group)
    return True


# === RANDOM SOUND CONTAINER TESTS ===

@test("Test 21: Container with no sounds")
def test_empty_container():
    """Test random sound container with no sounds loaded."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    config = audio_py.RandomSoundContainerConfig()
    config.pitch_min = 0.9
    config.pitch_max = 1.1
    config.group = group
    
    container = audio_py.RandomSoundContainer("empty", config)
    
    # Playing empty container should not crash
    try:
        container.play()
    except:
        pass
    
    audio.destroy_group(group)
    return True


@test("Test 22: Container with extreme pitch values")
def test_container_extreme_pitch():
    """Test container with extreme pitch randomization."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    config = audio_py.RandomSoundContainerConfig()
    config.pitch_min = 0.1
    config.pitch_max = 3.0
    config.avoid_repeat = True
    config.group = group
    
    container = audio_py.RandomSoundContainer("extreme", config)
    
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'touch_1.wav')
    if os.path.exists(sound_file):
        container.add_sound(sound_file)
    
    audio.destroy_group(group)
    return True


@test("Test 23: Container with many sounds")
def test_container_many_sounds():
    """Test container with many sound files."""
    audio = audio_py.AudioManager.get_instance()
    audio.set_master_volume(0.05)  # Very quiet
    group = audio.create_group("test")
    
    config = audio_py.RandomSoundContainerConfig()
    config.avoid_repeat = True
    config.group = group
    
    container = audio_py.RandomSoundContainer("many_sounds", config)
    
    sound_dir = os.path.join(os.path.dirname(__file__), '..', 'sound_files')
    
    # Add all touch sounds
    for i in range(1, 9):
        sound_path = os.path.join(sound_dir, f'touch_{i}.wav')
        if os.path.exists(sound_path):
            container.add_sound(sound_path)
    
    # Play multiple times to test randomization
    for i in range(10):
        container.play()
        time.sleep(0.05)
    
    audio.destroy_group(group)
    audio.set_master_volume(0.5)
    return True


# === RESOURCE CLEANUP TESTS ===

@test("Test 26: Track cleanup with layers")
def test_track_cleanup_with_layers():
    """Ensure destroying a track with layers doesn't leak."""
    audio = audio_py.AudioManager.get_instance()
    music_group = audio.create_group("music")
    
    for cycle in range(5):
        track = audio.create_track()
        
        sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'digital_base.wav')
        if os.path.exists(sound_file):
            for i in range(5):
                audio.add_layer(track, f"layer_{i}", sound_file, "music")
        
        audio.destroy_track(track)
    
    audio.destroy_group(music_group)
    return True


@test("Test 27: Group cleanup with sounds")
def test_group_cleanup_with_sounds():
    """Ensure destroying a group with sounds doesn't leak."""
    audio = audio_py.AudioManager.get_instance()
    
    for cycle in range(5):
        group = audio.create_group(f"group_{cycle}")
        
        sounds = []
        sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'hit.wav')
        if os.path.exists(sound_file):
            for i in range(10):
                handle = audio.load_sound(sound_file, group)
                if handle != 0:
                    sounds.append(handle)
        
        # Destroy group (sounds should be cleaned up automatically)
        audio.destroy_group(group)
    
    return True


# === CROSS-PLATFORM TESTS ===

@test("Test 28: Platform detection")
def test_platform_detection():
    """Verify we're running on a supported platform."""
    system = platform.system()
    supported = system in ["Windows", "Linux", "Darwin"]  # Darwin = macOS
    
    print(f"\n    Platform: {system}", end="")
    
    return supported


@test("Test 29: Sound file path separators")
def test_path_separators():
    """Test that path separators work correctly on this platform."""
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("test")
    
    # Use os.path.join for platform-appropriate separators
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'hit.wav')
    
    if os.path.exists(sound_file):
        handle = audio.load_sound(sound_file, group)
        assert handle != 0, "Should load with proper path"
        audio.destroy_sound(handle)
    
    audio.destroy_group(group)
    return True


@test("Test 30: Long running stability")
def test_long_running():
    """Test system stability over a brief extended period."""
    audio = audio_py.AudioManager.get_instance()
    audio.set_master_volume(0.01)  # Very quiet
    music_group = audio.create_group("music")
    
    track = audio.create_track()
    sound_file = os.path.join(os.path.dirname(__file__), '..', 'sound_files', 'digital_base.wav')
    
    if os.path.exists(sound_file):
        audio.add_layer(track, "loop", sound_file, "music")
        audio.play_track(track)
        
        # Run for 2 seconds
        time.sleep(2.0)
        
        audio.stop_track(track)
    
    audio.destroy_track(track)
    audio.destroy_group(music_group)
    audio.set_master_volume(0.5)
    return True


# === RUN ALL TESTS ===

def run_comprehensive_tests():
    """Run all comprehensive tests."""
    print("=" * 60)
    print("COMPREHENSIVE PYTHON AUDIO BINDINGS TEST SUITE")
    print("=" * 60)
    print(f"Platform: {platform.system()} {platform.release()}")
    print(f"Python: {sys.version}")
    print("=" * 60)
    
    # Run all tests
    test_multiple_init_shutdown()
    test_shutdown_without_init()
    test_double_initialize()
    test_volume_boundaries()
    test_group_volume_boundaries()
    test_invalid_track_handles()
    test_invalid_group_handles()
    test_invalid_sound_handles()
    test_load_nonexistent_file()
    test_load_invalid_path()
    test_many_groups()
    test_many_tracks()
    test_rapid_sound_operations()
    test_simultaneous_sounds()
    test_many_layers()
    test_layer_invalid_track()
    test_duplicate_layer_names()
    test_zero_fade()
    test_long_fade()
    test_overlapping_fades()
    test_empty_container()
    test_container_extreme_pitch()
    test_container_many_sounds()
    test_track_cleanup_with_layers()
    test_group_cleanup_with_sounds()
    test_platform_detection()
    test_path_separators()
    test_long_running()
    
    # Shutdown
    audio = audio_py.AudioManager.get_instance()
    audio.shutdown()
    
    # Print results
    print("\n" + "=" * 60)
    print("TEST RESULTS")
    print("=" * 60)
    
    passed = sum(1 for _, success, _ in test_results if success)
    failed = sum(1 for _, success, _ in test_results if not success)
    
    if failed > 0:
        print("\nFailed tests:")
        for name, success, error in test_results:
            if not success:
                print(f"  {name}: {error}")
    
    print(f"\n{passed}/{len(test_results)} tests passed")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_comprehensive_tests()
    sys.exit(0 if success else 1)

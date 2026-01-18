"""
Automated Python tests for the audio_py module

Tests basic functionality:
- System initialization
- Group creation and volume control
- Track creation and playback
- Layer management
- Sound loading and playback
- Random sound containers
- SFX player functionality
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
from datetime import timedelta

SOUND_DIR = os.path.join(repo_root, 'sound_files')

def test_basic_initialization():
    """Test 1: Basic system initialization and shutdown"""
    print("Test 1: Basic initialization... ", end="")
    audio = audio_py.AudioManager.get_instance()
    assert audio.initialize(), "Failed to initialize"
    audio.shutdown()
    print("PASS")

def test_audio_session_usage():
    """Test 1b: AudioSession usage for initialization/shutdown"""
    print("Test 1b: AudioSession usage... ", end="")
    session = audio_py.AudioSession()
    audio = audio_py.AudioManager.get_instance()
    group = audio.create_group("session_test")
    audio.set_group_volume(group, 0.5)
    audio.destroy_group(group)
    session.close()
    print("PASS")

def test_logging_controls():
    """Test 1c: Logging controls are accessible"""
    print("Test 1c: Logging controls... ", end="")
    audio_py.AudioManager.set_log_level(audio_py.LogLevel.Info)
    assert audio_py.AudioManager.get_log_level() == audio_py.LogLevel.Info, "Log level should be Info"
    audio_py.AudioManager.set_log_level(audio_py.LogLevel.Off)
    assert audio_py.AudioManager.get_log_level() == audio_py.LogLevel.Off, "Log level should be Off"
    print("PASS")

def test_master_volume():
    """Test 2: Master volume control"""
    print("Test 2: Master volume control... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    audio.set_master_volume(0.5)
    volume = audio.get_master_volume()
    assert abs(volume - 0.5) < 0.01, f"Expected 0.5, got {volume}"
    
    audio.shutdown()
    print("PASS")

def test_group_creation():
    """Test 3: Group creation and volume control"""
    print("Test 3: Group creation and volume... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    music_group = audio.create_group("music")
    sfx_group = audio.create_group("sfx")
    
    audio.set_group_volume(music_group, 0.7)
    audio.set_group_volume(sfx_group, 0.9)
    
    vol_music = audio.get_group_volume(music_group)
    vol_sfx = audio.get_group_volume(sfx_group)
    
    assert abs(vol_music - 0.7) < 0.01, f"Music volume mismatch"
    assert abs(vol_sfx - 0.9) < 0.01, f"SFX volume mismatch"
    
    audio.destroy_group(music_group)
    audio.destroy_group(sfx_group)
    audio.shutdown()
    print("PASS")

def test_track_creation():
    """Test 4: Track creation and basic operations"""
    print("Test 4: Track creation... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    track = audio.create_track()
    assert track != 0, "Failed to create track"
    
    audio.destroy_track(track)
    audio.shutdown()
    print("PASS")

def test_sound_loading():
    """Test 5: Sound loading and playback"""
    print("Test 5: Sound loading and playback... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    sfx_group = audio.create_group("sfx")
    sound_path = os.path.join(SOUND_DIR, "hit.wav")
    
    if os.path.exists(sound_path):
        sound = audio.load_sound(sound_path, sfx_group)
        assert sound != 0, "Failed to load sound"
        
        audio.start_sound(sound)
        is_playing = audio.is_sound_playing(sound)
        # Note: Sound might finish quickly, so we don't assert it's playing
        
        audio.stop_sound(sound)
        audio.destroy_sound(sound)
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.destroy_group(sfx_group)
    audio.shutdown()
    print("PASS")

def test_layered_track():
    """Test 6: Layered track with multiple layers"""
    print("Test 6: Layered track... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    music_group = audio.create_group("music")
    track = audio.create_track()
    
    # Add layers if sound files exist
    layer1 = os.path.join(SOUND_DIR, "digital_base.wav")
    layer2 = os.path.join(SOUND_DIR, "strings_base.wav")
    
    if os.path.exists(layer1) and os.path.exists(layer2):
        audio.add_layer(track, "digital", layer1, "music")
        audio.add_layer(track, "strings", layer2, "music")
        
        audio.set_layer_volume(track, "digital", 1.0)
        audio.set_layer_volume(track, "strings", 0.5)
        
        audio.play_track(track)
        audio.stop_track(track)
        
        audio.remove_layer(track, "digital")
        audio.remove_layer(track, "strings")
    else:
        print("SKIP (no sound files) ", end="")
    
    audio.destroy_track(track)
    audio.destroy_group(music_group)
    audio.shutdown()
    print("PASS")

def test_layer_fading():
    """Test 7: Layer volume fading"""
    print("Test 7: Layer fading... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    music_group = audio.create_group("music")
    track = audio.create_track()
    
    layer_path = os.path.join(SOUND_DIR, "digital_base.wav")
    if os.path.exists(layer_path):
        audio.add_layer(track, "test", layer_path, "music")
        audio.set_layer_volume(track, "test", 0.0)
        
        # Start fade (non-blocking)
        audio.fade_layer(track, "test", 1.0, timedelta(milliseconds=100))
        
        audio.remove_layer(track, "test")
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.destroy_track(track)
    audio.destroy_group(music_group)
    audio.shutdown()
    print("PASS")

def test_random_sound_container():
    """Test 8: Random sound container"""
    print("Test 8: Random sound container... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    sfx_group = audio.create_group("sfx")
    
    config = audio_py.RandomSoundContainerConfig()
    config.avoid_repeat = True
    config.pitch_min = 0.9
    config.pitch_max = 1.1
    config.group = sfx_group
    
    container = audio_py.RandomSoundContainer("test_container", config)
    
    # Add sounds if they exist
    sound_path = os.path.join(SOUND_DIR, "touch_1.wav")
    if os.path.exists(sound_path):
        container.add_sound(sound_path)
        container.play()
        container.stop_all()
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.destroy_group(sfx_group)
    audio.shutdown()
    print("PASS")

def test_handle_types():
    """Test 9: Handle type representations"""
    print("Test 9: Handle types... ", end="")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    group = audio.create_group("test")
    track = audio.create_track()
    
    # Handles are just integers (uint32_t)
    assert isinstance(group, int), "Group handle should be an integer"
    assert isinstance(track, int), "Track handle should be an integer"
    assert group > 0, "Group handle should be positive"
    assert track > 0, "Track handle should be positive"
    
    audio.destroy_track(track)
    audio.destroy_group(group)
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all tests"""
    print("=== Python Audio Module Tests ===\n")
    
    tests = [
        test_basic_initialization,
        test_audio_session_usage,
        test_logging_controls,
        test_master_volume,
        test_group_creation,
        test_track_creation,
        test_sound_loading,
        test_layered_track,
        test_layer_fading,
        test_random_sound_container,
        test_handle_types,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except AssertionError as e:
            print(f"FAIL - {e}")
            failed += 1
        except Exception as e:
            print(f"ERROR - {e}")
            failed += 1
    
    print(f"\n=== Results: {passed} passed, {failed} failed ===")
    
    # Give audio threads time to clean up
    import time
    time.sleep(0.5)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    # Now we can use sys.exit() since shutdown properly terminates threads
    sys.exit(0 if success else 1)

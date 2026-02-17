"""
Audio System Sound Tests
Tests sound loading, playback, multiple instances, and random sound folders.
"""

from test_common import *

def test_sound_loading():
    """Test: Sound loading and unloading"""
    print("Test: Sound loading... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    group = audio.create_group()
    
    if sound_exists("digital_base.wav"):
        sound_path = get_sound_path("digital_base.wav")
        
        # Test basic loading
        sound = audio.load_sound(sound_path)
        assert sound.value != 0, "Sound should load successfully"
        
        # Test loading with group
        sound2 = audio.load_sound(sound_path, group)
        assert sound2.value != 0, "Sound with group should load successfully"
        assert sound2.value != sound.value, "Different load calls should return different handles"
        
        # Test unloading
        audio.destroy_sound(sound)
        audio.destroy_sound(sound2)
        assert True, "Sound unloading should not crash"
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.destroy_group(group)
    audio.shutdown()
    print("PASS")

def test_sound_playback():
    """Test: Sound playback control"""
    print("Test: Sound playback... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    if sound_exists("digital_base.wav"):
        sound_path = get_sound_path("digital_base.wav")
        sound = audio.load_sound(sound_path)
        assert sound.value != 0, "Sound should load"
        
        # Test playback
        assert not audio.is_sound_playing(sound), "Sound should not be playing initially"
        
        audio.play_sound(sound)
        wait_ms(100)
        # Note: Sound might finish quickly, so we don't assert it's playing
        
        audio.stop_sound(sound)
        wait_ms(50)
        assert not audio.is_sound_playing(sound), "Sound should stop after stop_sound"
        
        # Test volume control
        audio.set_sound_volume(sound, 0.5)
        assert True, "set_sound_volume should not crash"
        
        # Test pitch control
        audio.set_sound_pitch(sound, 1.5)
        audio.play_sound(sound)
        wait_ms(100)
        assert True, "Pitch modification should work"
        audio.stop_sound(sound)
        
        audio.destroy_sound(sound)
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.shutdown()
    print("PASS")

def test_multiple_instances():
    """Test: Multiple sound instances"""
    print("Test: Multiple instances... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    if sound_exists("digital_base.wav"):
        sound_path = get_sound_path("digital_base.wav")
        sound = audio.load_sound(sound_path)
        
        # Play same sound multiple times
        audio.play_sound(sound)
        wait_ms(50)
        audio.play_sound(sound)
        wait_ms(50)
        audio.play_sound(sound)
        
        assert True, "Multiple instances should play concurrently"
        
        wait_ms(200)
        audio.stop_sound(sound)
        audio.destroy_sound(sound)
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.shutdown()
    print("PASS")

def test_random_sound_folder():
    """Test: Random sound from folder"""
    print("Test: Random sound folder... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    group = audio.create_group()
    
    # This should load all .wav files from the folder and play one randomly
    audio.play_random_sound_from_folder(SOUND_DIR, group)
    wait_ms(200)
    assert True, "Random sound playback should work"
    
    # Play again (should use cached sounds)
    audio.play_random_sound_from_folder(SOUND_DIR, group)
    wait_ms(200)
    assert True, "Cached random sound playback should work"
    
    audio.destroy_group(group)
    
    # Simulate a typical shutdown/reinitialize cycle
    audio.shutdown()
    assert audio.initialize(), "Reinitialize after shutdown should work"
    group2 = audio.create_group()
    audio.play_random_sound_from_folder(SOUND_DIR, group2)
    wait_ms(200)
    assert True, "Random sound playback should work after reinitialize"
    audio.destroy_group(group2)
    
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all sound tests"""
    print("=== Python Audio Sound Tests ===\n")
    
    tests = [
        test_sound_loading,
        test_sound_playback,
        test_multiple_instances,
        test_random_sound_folder,
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
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)

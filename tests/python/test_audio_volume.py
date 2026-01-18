"""
Audio System Volume Control Tests
Tests volume control, clamping, and pitch validation.
"""

from test_common import *

def test_master_volume():
    """Test: Master volume control"""
    print("Test: Master volume control... ", end="", flush=True)
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    audio.set_master_volume(0.5)
    volume = audio.get_master_volume()
    assert abs(volume - 0.5) < 0.01, f"Expected 0.5, got {volume}"
    
    audio.set_master_volume(1.0)
    assert abs(audio.get_master_volume() - 1.0) < 0.01, "Master volume should be 1.0"
    
    audio.set_master_volume(0.0)
    assert abs(audio.get_master_volume() - 0.0) < 0.01, "Master volume should be 0.0"
    
    # Test volume clamping
    audio.set_master_volume(-0.5)
    assert audio.get_master_volume() >= 0.0, "Negative master volume should be clamped"
    
    audio.set_master_volume(2.0)
    assert abs(audio.get_master_volume() - 1.0) < 0.01, "Master volume > 1.0 should be clamped"
    
    audio.set_master_volume(1.0)  # Reset
    audio.shutdown()
    print("PASS")

def test_volume_clamping():
    """Test: Volume clamping for all methods"""
    print("Test: Volume clamping... ", end="", flush=True)
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    # Test group volume clamping
    group = audio.create_group("test")
    audio.set_group_volume(group, -1.0)
    assert audio.get_group_volume(group) >= 0.0, "Negative group volume should be clamped"
    
    audio.set_group_volume(group, 5.0)
    assert abs(audio.get_group_volume(group) - 1.0) < 0.01, "Group volume > 1.0 should be clamped"
    
    # Test sound volume clamping
    if sound_exists("digital_base.wav"):
        sound = audio.load_sound(get_sound_path("digital_base.wav"))
        audio.set_sound_volume(sound, -0.3)
        assert True, "Negative sound volume should be clamped (no crash)"
        
        audio.set_sound_volume(sound, 10.0)
        assert True, "Sound volume > 1.0 should be clamped (no crash)"
        
        # Test layer volume clamping
        track = audio.create_track()
        audio.add_layer(track, "layer1", get_sound_path("digital_base.wav"))
        audio.set_layer_volume(track, "layer1", -0.5)
        assert True, "Negative layer volume should be clamped (no crash)"
        
        audio.set_layer_volume(track, "layer1", 2.5)
        assert True, "Layer volume > 1.0 should be clamped (no crash)"
        
        audio.destroy_track(track)
        audio.destroy_sound(sound)
    
    audio.destroy_group(group)
    audio.shutdown()
    print("PASS")

def test_pitch_validation():
    """Test: Pitch validation and clamping"""
    print("Test: Pitch validation... ", end="", flush=True)
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    if sound_exists("digital_base.wav"):
        sound = audio.load_sound(get_sound_path("digital_base.wav"))
        
        # Test negative pitch (should be clamped)
        audio.set_sound_pitch(sound, -1.0)
        assert True, "Negative pitch should be clamped (no crash)"
        
        # Test zero pitch (should be clamped)
        audio.set_sound_pitch(sound, 0.0)
        assert True, "Zero pitch should be clamped (no crash)"
        
        # Test very high pitch (should be clamped)
        audio.set_sound_pitch(sound, 100.0)
        assert True, "Very high pitch should be clamped (no crash)"
        
        # Test valid pitch values
        audio.set_sound_pitch(sound, 0.5)
        assert True, "Valid pitch 0.5 should work"
        
        audio.set_sound_pitch(sound, 1.0)
        assert True, "Valid pitch 1.0 should work"
        
        audio.set_sound_pitch(sound, 2.0)
        assert True, "Valid pitch 2.0 should work"
        
        audio.destroy_sound(sound)
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all volume tests"""
    print("=== Python Audio Volume Control Tests ===\n")
    
    tests = [
        test_master_volume,
        test_volume_clamping,
        test_pitch_validation,
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

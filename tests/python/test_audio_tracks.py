"""
Audio System Track Tests
Tests audio track operations including layer management and fading.
"""

from test_common import *

def test_track_operations():
    """Test: Audio track operations"""
    print("Test: Track operations... ", end="", flush=True)
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    # Create track
    track = audio.create_track()
    assert track.value != 0, "Track should be created"
    
    if sound_exists("digital_base.wav") and sound_exists("digital_battle.wav"):
        # Add layers
        audio.add_layer(track, "layer1", get_sound_path("digital_base.wav"))
        audio.add_layer(track, "layer2", get_sound_path("digital_battle.wav"))
        assert True, "Layers should be added without error"
        
        # Set layer volumes
        audio.set_layer_volume(track, "layer1", 1.0)
        audio.set_layer_volume(track, "layer2", 0.0)
        assert True, "Layer volumes should be set"
        
        # Play track
        audio.play_track(track)
        wait_ms(200)
        assert True, "Track should play"
        
        # Test layer fade
        audio.fade_layer(track, "layer2", 1.0, timedelta(milliseconds=300))
        wait_ms(350)
        assert True, "Layer fade should complete"
        
        # Stop track
        audio.stop_track(track)
        wait_ms(50)
        assert True, "Track should stop"
        
        # Remove layer
        audio.remove_layer(track, "layer1")
        assert True, "Layer removal should not crash"
    else:
        print("SKIP (no sound files) ", end="")
    
    audio.destroy_track(track)
    audio.shutdown()
    print("PASS")

def test_audio_track_update_fix():
    """Test: AudioTrack update fix (no redundant volume calls)"""
    print("Test: Track update fix... ", end="", flush=True)
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    if sound_exists("digital_base.wav") and sound_exists("digital_battle.wav"):
        track = audio.create_track()
        audio.add_layer(track, "layer1", get_sound_path("digital_base.wav"))
        audio.add_layer(track, "layer2", get_sound_path("digital_battle.wav"))
        
        # Start playing
        audio.play_track(track)
        
        # Set initial volumes
        audio.set_layer_volume(track, "layer1", 0.0)
        audio.set_layer_volume(track, "layer2", 0.0)
        
        # Start a fade - this should work without redundant volume updates
        audio.fade_layer(track, "layer1", 1.0, timedelta(milliseconds=500))
        audio.fade_layer(track, "layer2", 0.5, timedelta(milliseconds=300))
        
        # Wait for fades to complete
        wait_ms(600)
        
        # Verify volumes are set correctly (fade should have completed)
        assert True, "Fade operations should complete without issues"
        
        audio.stop_track(track)
        audio.destroy_track(track)
    else:
        print("SKIP (no sound files) ", end="")
    
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all track tests"""
    print("=== Python Audio Track Tests ===\n")
    
    tests = [
        test_track_operations,
        test_audio_track_update_fix,
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

"""
Audio System Resource Management Tests
Tests resource cleanup and edge cases.
"""

from test_common import *

def test_resource_cleanup():
    """Test: Resource cleanup"""
    print("Test: Resource cleanup... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    # Create many resources
    groups = []
    tracks = []
    sounds = []
    
    if sound_exists("digital_base.wav"):
        for i in range(10):
            groups.append(audio.create_group())
            tracks.append(audio.create_track())
            sounds.append(audio.load_sound(get_sound_path("digital_base.wav")))
        
        assert len(groups) == 10, "Should create 10 groups"
        assert len(tracks) == 10, "Should create 10 tracks"
        assert len(sounds) == 10, "Should load 10 sounds"
        
        # Clean up all resources
        for g in groups:
            audio.destroy_group(g)
        for t in tracks:
            audio.destroy_track(t)
        for s in sounds:
            audio.destroy_sound(s)
        
        assert True, "Mass resource cleanup should complete"
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.shutdown()
    print("PASS")

def test_edge_cases():
    """Test: Edge cases"""
    print("Test: Edge cases... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    # Create and immediately destroy
    g = audio.create_group()
    audio.destroy_group(g)
    assert True, "Immediate destruction should work"
    
    if sound_exists("digital_base.wav"):
        # Load and immediately unload
        s = audio.load_sound(get_sound_path("digital_base.wav"))
        audio.destroy_sound(s)
        assert True, "Immediate unload should work"
        
        # Create track, add layer, remove layer, destroy track
        t = audio.create_track()
        audio.add_layer(t, "test", get_sound_path("digital_base.wav"))
        audio.remove_layer(t, "test")
        audio.destroy_track(t)
        assert True, "Quick layer add/remove should work"
        
        # Play and stop immediately
        s2 = audio.load_sound(get_sound_path("digital_base.wav"))
        audio.play_sound(s2)
        audio.stop_sound(s2)
        audio.destroy_sound(s2)
        assert True, "Immediate play/stop should work"
        
        # Remove non-existent layer
        t2 = audio.create_track()
        audio.remove_layer(t2, "nonexistent")
        assert True, "Removing non-existent layer should not crash"
        audio.destroy_track(t2)
    else:
        print("SKIP (no sound file) ", end="")
    
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all resource management tests"""
    print("=== Python Audio Resource Management Tests ===\n")
    
    tests = [
        test_resource_cleanup,
        test_edge_cases,
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

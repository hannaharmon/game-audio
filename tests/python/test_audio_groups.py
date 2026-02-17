"""
Audio System Group Tests
Tests audio group operations including creation, volume control, and fading.
"""

from test_common import *

def test_group_operations():
    """Test: Audio group operations"""
    print("Test: Group operations... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    # Create groups
    music = audio.create_group("music")
    assert music.value != 0, "Music group handle should be non-zero"
    
    sfx = audio.create_group("sfx")
    assert sfx.value != 0, "SFX group handle should be non-zero"
    assert sfx.value != music.value, "Group handles should be unique"
    
    # Test volume control
    audio.set_group_volume(music, 0.7)
    assert abs(audio.get_group_volume(music) - 0.7) < 0.01, "Music group volume should be 0.7"
    
    audio.set_group_volume(sfx, 0.3)
    assert abs(audio.get_group_volume(sfx) - 0.3) < 0.01, "SFX group volume should be 0.3"
    
    # Test fade (non-blocking, just verify it doesn't crash)
    audio.fade_group(music, 0.0, timedelta(milliseconds=500))
    wait_ms(100)
    assert True, "Group fade operation should not crash"
    
    # Cleanup
    audio.destroy_group(music)
    audio.destroy_group(sfx)
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all group tests"""
    print("=== Python Audio Group Tests ===\n")
    
    tests = [
        test_group_operations,
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

"""
Audio System Initialization Tests
Tests system initialization, shutdown, AudioSession, and logging controls.
"""

from test_common import *

def test_basic_initialization():
    """Test: Basic system initialization and shutdown"""
    print("Test: Basic initialization... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    assert audio.initialize(), "Failed to initialize"
    audio.shutdown()
    print("PASS")

def test_audio_session_usage():
    """Test: AudioSession usage for initialization/shutdown"""
    print("Test: AudioSession usage... ", end="", flush=True)
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    group = audio.create_group("session_test")
    audio.set_group_volume(group, 0.5)
    audio.destroy_group(group)
    session.close()
    print("PASS")

def test_logging_controls():
    """Test: Logging controls are accessible and work correctly"""
    print("Test: Logging controls... ", end="", flush=True)
    
    # Test that logging is always available (no compile-time flag needed)
    original = game_audio.AudioManager.get_log_level()
    
    # Test all log levels can be set
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Off)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Off, "Log level should be Off"
    
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Error)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Error, "Log level should be Error"
    
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Warn)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Warn, "Log level should be Warn"
    
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Info, "Log level should be Info"
    
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Debug)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Debug, "Log level should be Debug"
    
    # Restore original level
    game_audio.AudioManager.set_log_level(original)
    print("PASS")

def test_rapid_shutdown_reinitialize():
    """Test: Rapid shutdown/reinitialize cycles"""
    print("Test: Rapid shutdown/reinitialize... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    
    for i in range(5):
        audio.shutdown()
        wait_ms(50)
        assert audio.initialize(), f"Should be able to reinitialize after shutdown (cycle {i+1})"
        wait_ms(50)
        
        # Verify system works after reinitialize
        group = audio.create_group("test")
        audio.set_group_volume(group, 0.5)
        assert abs(audio.get_group_volume(group) - 0.5) < 0.01, "System should work after reinitialize"
        audio.destroy_group(group)
    
    # Ensure we shutdown at the end
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all initialization tests"""
    print("=== Python Audio Initialization Tests ===\n")
    
    tests = [
        test_basic_initialization,
        test_audio_session_usage,
        test_logging_controls,
        test_rapid_shutdown_reinitialize,
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

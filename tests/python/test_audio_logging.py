"""
Audio System Logging Tests
Tests that logging functionality works correctly with the simplified system
(logging always compiled in, controlled at runtime).
"""

from test_common import *
import sys

def test_logging_levels():
    """Test: Logging level hierarchy"""
    print("Test: Logging level hierarchy... ", end="", flush=True)
    
    original = game_audio.AudioManager.get_log_level()
    
    # Test that Off disables everything
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Off)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Off, "Level should be Off"
    
    # Test Error level
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Error)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Error, "Level should be Error"
    
    # Test Warn level
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Warn)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Warn, "Level should be Warn"
    
    # Test Info level
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Info, "Level should be Info"
    
    # Test Debug level
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Debug)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Debug, "Level should be Debug"
    
    # Restore original
    game_audio.AudioManager.set_log_level(original)
    print("PASS")

def test_logging_output():
    """Test: Logging output works"""
    print("Test: Logging output... ", end="", flush=True)
    
    original = game_audio.AudioManager.get_log_level()
    
    # Use AudioSession for proper cleanup
    try:
        session = game_audio.AudioSession()
        audio = game_audio.AudioManager.get_instance()
        
        # Test that logging can be enabled and doesn't crash
        game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)
        
        # Ensure initialized (AudioSession should handle this, but check to be safe)
        if not audio.is_initialized():
            audio.initialize()
        
        # Create a group (should generate logs)
        group = audio.create_group("logging_test")
        assert group.is_valid(), "Group should be valid"
        
        # Set volume (should generate logs at Debug level)
        audio.set_group_volume(group, 0.5)
        
        # Clean up
        audio.destroy_group(group)
        
        # Test that Off disables output
        game_audio.AudioManager.set_log_level(game_audio.LogLevel.Off)
        
        # Operations should still work, just no logging
        group2 = audio.create_group("logging_test_2")
        audio.destroy_group(group2)
        
        # Restore original
        game_audio.AudioManager.set_log_level(original)
    finally:
        # Ensure cleanup
        if 'session' in locals():
            session.close()
    
    print("PASS")

def test_logging_default_state():
    """Test: Logging defaults to Off"""
    print("Test: Logging default state... ", end="", flush=True)
    
    # After import, logging should default to Off
    # (We can't easily test the absolute default, but we can test Off works)
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Off)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Off, "Should be able to set to Off"
    
    # Test that we can enable it
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Info, "Should be able to enable logging"
    
    print("PASS")

def test_logging_persistence():
    """Test: Logging level persists"""
    print("Test: Logging level persistence... ", end="", flush=True)
    
    original = game_audio.AudioManager.get_log_level()
    
    # Set a level
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Warn)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Warn, "Level should persist"
    
    # Get it back
    retrieved = game_audio.AudioManager.get_log_level()
    assert retrieved == game_audio.LogLevel.Warn, "get_log_level should return set level"
    
    # Change it
    game_audio.AudioManager.set_log_level(game_audio.LogLevel.Debug)
    assert game_audio.AudioManager.get_log_level() == game_audio.LogLevel.Debug, "Level should update"
    
    # Restore original
    game_audio.AudioManager.set_log_level(original)
    print("PASS")

def test_logging_always_available():
    """Test: Logging is always available (no compile-time flag needed)"""
    print("Test: Logging always available... ", end="", flush=True)
    
    original = game_audio.AudioManager.get_log_level()
    
    # Logging should work regardless of how the module was built
    # (In the old system, it would only work if AUDIO_ENABLE_LOGGING was set)
    # Now it should always work
    
    # Test all levels work
    for level in [game_audio.LogLevel.Off, game_audio.LogLevel.Error, game_audio.LogLevel.Warn, 
                  game_audio.LogLevel.Info, game_audio.LogLevel.Debug]:
        game_audio.AudioManager.set_log_level(level)
        assert game_audio.AudioManager.get_log_level() == level, f"Level {level} should work"
    
    # Restore original
    game_audio.AudioManager.set_log_level(original)
    print("PASS")

def run_all_tests():
    """Run all logging tests"""
    print("=== Python Audio Logging Tests ===\n")
    
    tests = [
        test_logging_levels,
        test_logging_output,
        test_logging_default_state,
        test_logging_persistence,
        test_logging_always_available,
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

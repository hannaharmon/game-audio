#!/usr/bin/env python3
"""
Test script to verify a game_audio wheel installation works correctly.
Run this after installing a wheel: python test_wheel.py
"""

import sys
import traceback

def test_import():
    """Test that the module can be imported"""
    print("Test 1: Importing game_audio module...", end=" ")
    try:
        import game_audio
        print("✓ PASS")
        return game_audio
    except Exception as e:
        print(f"✗ FAIL: {e}")
        traceback.print_exc()
        return None

def test_version(game_audio):
    """Test that version is accessible"""
    print("Test 2: Checking version...", end=" ")
    try:
        version = game_audio.__version__
        print(f"✓ PASS (version: {version})")
        return True
    except Exception as e:
        print(f"✗ FAIL: {e}")
        return False

def test_log_levels(game_audio):
    """Test that LogLevel enum is accessible"""
    print("Test 3: Checking LogLevel enum...", end=" ")
    try:
        assert hasattr(game_audio, 'LogLevel'), "LogLevel not found"
        assert hasattr(game_audio.LogLevel, 'Off'), "LogLevel.Off not found"
        assert hasattr(game_audio.LogLevel, 'Error'), "LogLevel.Error not found"
        assert hasattr(game_audio.LogLevel, 'Warn'), "LogLevel.Warn not found"
        assert hasattr(game_audio.LogLevel, 'Info'), "LogLevel.Info not found"
        assert hasattr(game_audio.LogLevel, 'Debug'), "LogLevel.Debug not found"
        print("✓ PASS")
        return True
    except Exception as e:
        print(f"✗ FAIL: {e}")
        return False

def test_audio_manager(game_audio):
    """Test that AudioManager is accessible and can be used"""
    print("Test 4: Testing AudioManager...", end=" ")
    try:
        # Check that AudioManager exists
        assert hasattr(game_audio, 'AudioManager'), "AudioManager not found"
        
        # Test get_instance
        audio = game_audio.AudioManager.get_instance()
        assert audio is not None, "get_instance() returned None"
        
        # Test initialization
        audio.initialize()
        assert audio.is_initialized(), "AudioManager not initialized after initialize()"
        
        # Test setting log level
        game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)
        
        # Test creating a group
        group = audio.create_group("test_group")
        assert group is not None, "create_group() returned None"
        assert group.is_valid(), "Group handle is not valid"
        
        # Test setting master volume
        audio.set_master_volume(0.5)
        
        # Cleanup
        audio.shutdown()
        print("✓ PASS")
        return True
    except Exception as e:
        print(f"✗ FAIL: {e}")
        traceback.print_exc()
        return False

def test_audio_session(game_audio):
    """Test that AudioSession works"""
    print("Test 5: Testing AudioSession...", end=" ")
    try:
        assert hasattr(game_audio, 'AudioSession'), "AudioSession not found"
        
        # Create a session
        session = game_audio.AudioSession()
        assert session is not None, "AudioSession() returned None"
        
        # Get audio manager through session
        audio = game_audio.AudioManager.get_instance()
        assert audio.is_initialized(), "AudioManager not initialized via AudioSession"
        
        # Test that we can use the audio system
        group = audio.create_group("session_test")
        assert group.is_valid(), "Group handle is not valid"
        
        # Session will cleanup on close/destructor
        session.close()
        print("✓ PASS")
        return True
    except Exception as e:
        print(f"✗ FAIL: {e}")
        traceback.print_exc()
        return False

def test_handle_types(game_audio):
    """Test that handle types are accessible"""
    print("Test 6: Testing handle types...", end=" ")
    try:
        assert hasattr(game_audio, 'TrackHandle'), "TrackHandle not found"
        assert hasattr(game_audio, 'GroupHandle'), "GroupHandle not found"
        assert hasattr(game_audio, 'SoundHandle'), "SoundHandle not found"
        
        # Test invalid handles
        invalid_track = game_audio.TrackHandle.invalid()
        assert not invalid_track.is_valid(), "Invalid track handle should not be valid"
        
        invalid_group = game_audio.GroupHandle.invalid()
        assert not invalid_group.is_valid(), "Invalid group handle should not be valid"
        
        invalid_sound = game_audio.SoundHandle.invalid()
        assert not invalid_sound.is_valid(), "Invalid sound handle should not be valid"
        
        print("✓ PASS")
        return True
    except Exception as e:
        print(f"✗ FAIL: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("Testing game_audio wheel installation")
    print("=" * 60)
    print()
    
    results = []
    
    # Test 1: Import
    game_audio = test_import()
    if game_audio is None:
        print("\n✗ Cannot continue - import failed")
        sys.exit(1)
    results.append(True)
    print()
    
    # Test 2: Version
    results.append(test_version(game_audio))
    print()
    
    # Test 3: LogLevel enum
    results.append(test_log_levels(game_audio))
    print()
    
    # Test 4: AudioManager
    results.append(test_audio_manager(game_audio))
    print()
    
    # Test 5: AudioSession
    results.append(test_audio_session(game_audio))
    print()
    
    # Test 6: Handle types
    results.append(test_handle_types(game_audio))
    print()
    
    # Summary
    print("=" * 60)
    passed = sum(results)
    total = len(results)
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed! Wheel installation is working correctly.")
        sys.exit(0)
    else:
        print(f"✗ {total - passed} test(s) failed. Please check the errors above.")
        sys.exit(1)

if __name__ == "__main__":
    main()

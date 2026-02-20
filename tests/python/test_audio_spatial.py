"""
Python tests for spatial audio functionality.

Tests Vec3 operations, listener management, sound positioning,
and spatial audio integration.
"""

import sys
import os

# Add build directory to path
build_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'Debug')
if os.path.exists(build_dir):
    sys.path.insert(0, build_dir)

import game_audio
import math

def test_vec3_basic():
    """Test Vec3 basic operations"""
    print("\nTEST: Vec3 Basic Operations")
    
    # Default constructor
    v1 = game_audio.Vec3()
    assert v1.x == 0.0 and v1.y == 0.0 and v1.z == 0.0, "Default Vec3 should be (0,0,0)"
    print("  PASS: Default constructor")
    
    # Component constructor
    v2 = game_audio.Vec3(1.0, 2.0, 3.0)
    assert v2.x == 1.0 and v2.y == 2.0 and v2.z == 3.0, "Vec3 constructor should set components"
    print("  PASS: Component constructor")
    
    # Equality
    v3 = game_audio.Vec3(1.0, 2.0, 3.0)
    assert v2 == v3, "Equal Vec3s should compare equal"
    assert v1 != v2, "Different Vec3s should compare not equal"
    print("  PASS: Equality comparison")
    
    # Length
    v4 = game_audio.Vec3(3.0, 4.0, 0.0)
    assert abs(v4.length() - 5.0) < 0.001, "Vec3 length should be calculated correctly"
    print("  PASS: Length calculation")
    
    # Length squared
    assert abs(v4.length_squared() - 25.0) < 0.001, "Vec3 length squared should be correct"
    print("  PASS: Length squared calculation")

def test_vec3_arithmetic():
    """Test Vec3 arithmetic operations"""
    print("\nTEST: Vec3 Arithmetic Operations")
    
    v1 = game_audio.Vec3(1.0, 2.0, 3.0)
    v2 = game_audio.Vec3(4.0, 5.0, 6.0)
    
    # Addition
    sum_vec = v1 + v2
    assert sum_vec.x == 5.0 and sum_vec.y == 7.0 and sum_vec.z == 9.0, "Vec3 addition should work"
    print("  PASS: Addition")
    
    # Subtraction
    diff = v2 - v1
    assert diff.x == 3.0 and diff.y == 3.0 and diff.z == 3.0, "Vec3 subtraction should work"
    print("  PASS: Subtraction")
    
    # Scalar multiplication
    scaled = v1 * 2.0
    assert scaled.x == 2.0 and scaled.y == 4.0 and scaled.z == 6.0, "Vec3 scalar multiplication should work"
    print("  PASS: Scalar multiplication")
    
    # Scalar division
    divided = scaled / 2.0
    assert abs(divided.x - 1.0) < 0.001 and abs(divided.y - 2.0) < 0.001 and abs(divided.z - 3.0) < 0.001, "Vec3 scalar division should work"
    print("  PASS: Scalar division")
    
    # In-place operations
    v3 = game_audio.Vec3(1.0, 1.0, 1.0)
    v3 += v1
    assert v3.x == 2.0 and v3.y == 3.0 and v3.z == 4.0, "Vec3 += should work"
    print("  PASS: In-place addition")
    
    v3 -= v1
    assert abs(v3.x - 1.0) < 0.001 and abs(v3.y - 1.0) < 0.001 and abs(v3.z - 1.0) < 0.001, "Vec3 -= should work"
    print("  PASS: In-place subtraction")

def test_vec3_normalization():
    """Test Vec3 normalization"""
    print("\nTEST: Vec3 Normalization")
    
    v1 = game_audio.Vec3(3.0, 4.0, 0.0)
    original_length = v1.length()
    
    # Normalize in place
    v1.normalize()
    assert abs(v1.length() - 1.0) < 0.001, "Normalized Vec3 should have length 1"
    print("  PASS: Normalize in place")
    
    # Normalized copy
    v2 = game_audio.Vec3(3.0, 4.0, 0.0)
    normalized = v2.normalized()
    assert abs(normalized.length() - 1.0) < 0.001, "Normalized() should return normalized copy"
    assert abs(v2.length() - original_length) < 0.001, "Normalized() should not modify original"
    print("  PASS: Normalized copy")

def test_vec3_distance():
    """Test Vec3 distance calculations"""
    print("\nTEST: Vec3 Distance Calculations")
    
    p1 = game_audio.Vec3(0.0, 0.0, 0.0)
    p2 = game_audio.Vec3(3.0, 4.0, 0.0)
    
    dist = p1.distance(p2)
    assert abs(dist - 5.0) < 0.001, "Distance between (0,0,0) and (3,4,0) should be 5"
    print("  PASS: Distance calculation")
    
    dist_sq = p1.distance_squared(p2)
    assert abs(dist_sq - 25.0) < 0.001, "Distance squared should be 25"
    print("  PASS: Distance squared calculation")
    
    # Distance to self should be zero
    assert abs(p1.distance(p1)) < 0.001, "Distance to self should be zero"
    print("  PASS: Distance to self is zero")

def test_listener_position():
    """Test listener position management"""
    print("\nTEST: Listener Position Management")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Set listener position
    pos1 = game_audio.Vec3(5.0, 10.0, 15.0)
    audio.set_listener_position(pos1)
    
    retrieved = audio.get_listener_position()
    assert abs(retrieved.x - 5.0) < 0.01 and abs(retrieved.y - 10.0) < 0.01 and abs(retrieved.z - 15.0) < 0.01, "Listener position should be set correctly"
    print("  PASS: Set and get listener position")
    
    # Update position
    pos2 = game_audio.Vec3(20.0, 30.0, 40.0)
    audio.set_listener_position(pos2)
    retrieved = audio.get_listener_position()
    assert abs(retrieved.x - 20.0) < 0.01, "Listener position should update correctly"
    print("  PASS: Update listener position")
    
    session.close()

def test_listener_direction():
    """Test listener direction management"""
    print("\nTEST: Listener Direction Management")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Set listener direction
    forward = game_audio.Vec3(0.0, 0.0, -1.0)
    audio.set_listener_direction(forward)
    
    retrieved = audio.get_listener_direction()
    assert abs(retrieved.z - (-1.0)) < 0.01, "Listener direction should be set correctly"
    print("  PASS: Set and get listener direction")
    
    session.close()

def test_sound_position():
    """Test sound position management"""
    print("\nTEST: Sound Position Management")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    assert sound.is_valid(), "Sound should load"
    
    # Set sound position
    pos1 = game_audio.Vec3(10.0, 20.0, 30.0)
    audio.set_sound_position(sound, pos1)
    
    retrieved = audio.get_sound_position(sound)
    assert abs(retrieved.x - 10.0) < 0.01 and abs(retrieved.y - 20.0) < 0.01 and abs(retrieved.z - 30.0) < 0.01, "Sound position should be set correctly"
    print("  PASS: Set and get sound position")
    
    # Update position
    pos2 = game_audio.Vec3(50.0, 60.0, 70.0)
    audio.set_sound_position(sound, pos2)
    retrieved = audio.get_sound_position(sound)
    assert abs(retrieved.x - 50.0) < 0.01, "Sound position should update correctly"
    print("  PASS: Update sound position")
    
    # Test invalid handle
    try:
        audio.set_sound_position(game_audio.SoundHandle.invalid(), pos1)
        assert False, "Should throw exception for invalid handle"
    except game_audio.InvalidHandleException:
        print("  PASS: Invalid handle throws exception")
    
    audio.destroy_sound(sound)
    session.close()

def test_sound_distance_attenuation():
    """Test sound distance attenuation parameters"""
    print("\nTEST: Sound Distance Attenuation Parameters")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    
    # Set min distance
    audio.set_sound_min_distance(sound, 5.0)
    min_dist = audio.get_sound_min_distance(sound)
    assert abs(min_dist - 5.0) < 0.01, "Sound min distance should be set correctly"
    print("  PASS: Set min distance")
    
    # Set max distance
    audio.set_sound_max_distance(sound, 100.0)
    max_dist = audio.get_sound_max_distance(sound)
    assert abs(max_dist - 100.0) < 0.01, "Sound max distance should be set correctly"
    print("  PASS: Set max distance")
    
    # Set rolloff
    audio.set_sound_rolloff(sound, 2.0)
    rolloff = audio.get_sound_rolloff(sound)
    assert abs(rolloff - 2.0) < 0.01, "Sound rolloff should be set correctly"
    print("  PASS: Set rolloff")
    
    audio.destroy_sound(sound)
    session.close()

def test_sound_spatialization_enabled():
    """Test sound spatialization enable/disable"""
    print("\nTEST: Sound Spatialization Enable/Disable")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    
    # Spatialization should be enabled by default
    enabled = audio.is_sound_spatialization_enabled(sound)
    assert enabled == True, "Spatialization should be enabled by default"
    print("  PASS: Spatialization enabled by default")
    
    # Disable spatialization
    audio.set_sound_spatialization_enabled(sound, False)
    enabled = audio.is_sound_spatialization_enabled(sound)
    assert enabled == False, "Spatialization should be disabled"
    print("  PASS: Disable spatialization")
    
    # Re-enable spatialization
    audio.set_sound_spatialization_enabled(sound, True)
    enabled = audio.is_sound_spatialization_enabled(sound)
    assert enabled == True, "Spatialization should be re-enabled"
    print("  PASS: Re-enable spatialization")
    
    audio.destroy_sound(sound)
    session.close()

def test_spatial_audio_integration():
    """Test spatial audio integration (listener + sound)"""
    print("\nTEST: Spatial Audio Integration")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Set up listener
    listener_pos = game_audio.Vec3(0.0, 0.0, 0.0)
    audio.set_listener_position(listener_pos)
    
    # Create sound at a distance
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    sound_pos = game_audio.Vec3(5.0, 0.0, 0.0)  # 5 units to the right
    audio.set_sound_position(sound, sound_pos)
    audio.set_sound_min_distance(sound, 1.0)
    audio.set_sound_max_distance(sound, 20.0)
    
    # Verify distance calculation
    distance = listener_pos.distance(sound_pos)
    assert abs(distance - 5.0) < 0.001, "Distance between listener and sound should be 5"
    print("  PASS: Distance calculation")
    
    # Move listener closer
    new_listener_pos = game_audio.Vec3(2.0, 0.0, 0.0)
    audio.set_listener_position(new_listener_pos)
    new_distance = new_listener_pos.distance(sound_pos)
    assert abs(new_distance - 3.0) < 0.001, "Distance should decrease when listener moves closer"
    print("  PASS: Listener movement affects distance")
    
    audio.destroy_sound(sound)
    session.close()

def test_play_sound_at_position():
    """Test playing sound at specific position"""
    print("\nTEST: Play Sound at Specific Position")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Set up listener
    audio.set_listener_position(game_audio.Vec3(0.0, 0.0, 0.0))
    
    # Load a sound
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    audio.set_sound_min_distance(sound, 1.0)
    audio.set_sound_max_distance(sound, 20.0)
    
    # Play at position 1
    pos1 = game_audio.Vec3(5.0, 0.0, 0.0)
    audio.play_sound(sound, pos1)
    import time
    time.sleep(0.1)
    assert audio.is_sound_playing(sound), "Sound should be playing at position 1"
    print("  PASS: Sound plays at position 1")
    
    # Play at position 2 (should overlap with position 1)
    pos2 = game_audio.Vec3(10.0, 0.0, 0.0)
    audio.play_sound(sound, pos2)
    time.sleep(0.1)
    assert audio.is_sound_playing(sound), "Sound should still be playing (overlapping instances)"
    print("  PASS: Overlapping sounds work correctly")
    
    # Play at position 3 (should overlap with both)
    pos3 = game_audio.Vec3(-5.0, 0.0, 0.0)
    audio.play_sound(sound, pos3)
    time.sleep(0.1)
    assert audio.is_sound_playing(sound), "Sound should still be playing (multiple overlapping instances)"
    print("  PASS: Multiple overlapping instances work")
    
    # Stop all instances
    audio.stop_sound(sound)
    time.sleep(0.1)
    assert not audio.is_sound_playing(sound), "All instances should be stopped"
    print("  PASS: All instances stopped correctly")
    
    audio.destroy_sound(sound)
    session.close()

def test_overlapping_spatial_sounds():
    """Test overlapping spatial sounds at different positions"""
    print("\nTEST: Overlapping Spatial Sounds at Different Positions")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Set up listener
    audio.set_listener_position(game_audio.Vec3(0.0, 0.0, 0.0))
    
    # Load a sound
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    sound = audio.load_sound(os.path.join(sound_dir, "digital_base.wav"))
    audio.set_sound_min_distance(sound, 1.0)
    audio.set_sound_max_distance(sound, 50.0)
    
    # Play multiple instances at different positions simultaneously
    positions = [
        game_audio.Vec3(5.0, 0.0, 0.0),
        game_audio.Vec3(-5.0, 0.0, 0.0),
        game_audio.Vec3(0.0, 0.0, 5.0),
        game_audio.Vec3(0.0, 0.0, -5.0),
        game_audio.Vec3(3.0, 3.0, 3.0)
    ]
    
    for pos in positions:
        audio.play_sound(sound, pos)
    
    import time
    time.sleep(0.2)
    assert audio.is_sound_playing(sound), "All instances should be playing simultaneously"
    print("  PASS: Multiple overlapping spatial sounds play correctly")
    
    audio.stop_sound(sound)
    time.sleep(0.1)
    
    audio.destroy_sound(sound)
    session.close()

def test_random_container_get_random_sound():
    """Test RandomSoundContainer GetRandomSound method"""
    print("\nTEST: RandomSoundContainer GetRandomSound")
    
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Create a random sound container
    config = game_audio.RandomSoundContainerConfig()
    container = game_audio.RandomSoundContainer("test_container", config)
    
    # Add multiple sounds
    sound_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'sound_files')
    container.add_sound(os.path.join(sound_dir, "digital_base.wav"))
    container.add_sound(os.path.join(sound_dir, "hit.wav"))
    
    assert container.get_sound_count() == 2, "Container should have 2 sounds"
    print("  PASS: Container has correct sound count")
    
    # Get random sounds multiple times
    sound1 = container.get_random_sound()
    assert sound1.value != 0, "GetRandomSound should return valid handle"
    print("  PASS: GetRandomSound returns valid handle")
    
    sound2 = container.get_random_sound()
    assert sound2.value != 0, "GetRandomSound should return valid handle"
    print("  PASS: GetRandomSound works multiple times")
    
    # Test with empty container
    empty_container = game_audio.RandomSoundContainer("empty", config)
    invalid = empty_container.get_random_sound()
    assert invalid.value == 0, "GetRandomSound on empty container should return invalid handle"
    print("  PASS: Empty container returns invalid handle")
    
    session.close()

def main():
    """Run all spatial audio tests"""
    print("========================================")
    print("Python Spatial Audio Tests")
    print("========================================")
    
    try:
        test_vec3_basic()
        test_vec3_arithmetic()
        test_vec3_normalization()
        test_vec3_distance()
        test_listener_position()
        test_listener_direction()
        test_sound_position()
        test_sound_distance_attenuation()
        test_sound_spatialization_enabled()
        test_spatial_audio_integration()
        test_play_sound_at_position()
        test_overlapping_spatial_sounds()
        test_random_container_get_random_sound()
        
        print("\n========================================")
        print("ALL TESTS PASSED OK")
        print("========================================")
        return 0
    except Exception as e:
        print(f"\n========================================")
        print(f"TEST FAILED: {e}")
        print("========================================")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())

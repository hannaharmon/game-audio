"""
Overlapping Spatial Sounds Example

Demonstrates playing multiple overlapping spatialized sounds from the same
audio file at different positions. This is useful for scenarios like:
- Multiple enemies shooting (gunshots)
- Multiple explosions happening simultaneously
- Multiple footsteps from different NPCs

This example shows how to use the new PlaySound(position) overload to
play the same sound at different positions without affecting existing
playback instances.
"""

import os
import sys
import time

import game_audio

# Sound directory
SOUND_DIR = os.path.join(os.path.dirname(__file__), '..', 'sound_files')

def main():
    """Main function - demonstrate overlapping spatial sounds"""
    print("=== Overlapping Spatial Sounds Example ===")
    print(f"Sound directory: {SOUND_DIR}\n")
    
    # Initialize audio system
    print("Initializing audio system...")
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Create a sound group for spatialized sounds
    sfx_group = audio.create_group()
    audio.set_group_volume(sfx_group, 1.0)
    
    # Load a short sound effect (gunshot, explosion, etc.)
    # Using a short sound file for demonstration
    sound_file = os.path.join(SOUND_DIR, "hit.wav")
    if not os.path.exists(sound_file):
        print(f"Error: Sound file not found: {sound_file}")
        print("Please ensure sound files are available in the sound_files directory.")
        return
    
    print(f"Loading sound: {sound_file}")
    gunshot_sound = audio.load_sound(sound_file, sfx_group)
    
    # Configure spatial audio parameters
    audio.set_sound_min_distance(gunshot_sound, 1.0)
    audio.set_sound_max_distance(gunshot_sound, 50.0)
    audio.set_sound_rolloff(gunshot_sound, 1.0)
    
    # Set up listener at origin
    listener_position = game_audio.Vec3(0.0, 0.0, 0.0)
    audio.set_listener_position(listener_position)
    print("\nListener at origin (0, 0, 0)")
    
    print("\n=== Scenario: Multiple Enemies Shooting ===")
    print("We'll play the same gunshot sound at different positions")
    print("to simulate multiple enemies shooting simultaneously.\n")
    
    # Simulate multiple enemies shooting at different positions
    enemy_positions = [
        game_audio.Vec3(5.0, 0.0, 0.0),   # Enemy 1: 5 units to the right
        game_audio.Vec3(-5.0, 0.0, 0.0),  # Enemy 2: 5 units to the left
        game_audio.Vec3(0.0, 0.0, 5.0),   # Enemy 3: 5 units forward
        game_audio.Vec3(0.0, 0.0, -5.0),  # Enemy 4: 5 units backward
        game_audio.Vec3(3.0, 0.0, 3.0),   # Enemy 5: diagonal
    ]
    
    print("Playing gunshots at different positions...")
    for i, pos in enumerate(enemy_positions, 1):
        print(f"  Enemy {i} shoots at position {pos}")
        audio.play_sound(gunshot_sound, pos)
        time.sleep(0.1)  # Small delay to hear each one
    
    print("\nAll gunshots are now playing simultaneously at different positions!")
    print("Notice how each one maintains its own position even though")
    print("they're all from the same sound file.\n")
    
    # Wait for sounds to finish
    print("Waiting for sounds to finish...")
    time.sleep(2.0)
    
    # Demonstrate rapid fire from same position
    print("\n=== Scenario: Rapid Fire from Same Position ===")
    print("Playing multiple gunshots rapidly from the same position...")
    rapid_fire_pos = game_audio.Vec3(10.0, 0.0, 0.0)
    for i in range(5):
        print(f"  Shot {i+1} at {rapid_fire_pos}")
        audio.play_sound(gunshot_sound, rapid_fire_pos)
        time.sleep(0.2)
    
    print("\nAll shots are playing simultaneously at the same position!")
    print("Each shot maintains its own playback instance.\n")
    
    # Wait for sounds to finish
    print("Waiting for sounds to finish...")
    time.sleep(2.0)
    
    print("\n=== Key Points ===")
    print("1. Load the sound file ONCE (e.g., at game start)")
    print("2. Use play_sound(sound, position) to play at specific positions")
    print("3. Each call creates a new playback instance with its own position")
    print("4. Existing instances keep their positions unchanged")
    print("5. Finished instances are automatically cleaned up")
    print("\nThis makes it easy to handle overlapping spatialized sounds!")
    
    print("\nExample complete!")

if __name__ == "__main__":
    main()

"""
Spatial Audio Example - Demonstrates 3D spatialized audio

This example shows how to use spatialized audio in the game audio module.
A sound source is placed at a 3D position, and the listener (player) can
move around to hear the sound get louder when closer and quieter when farther away.

Usage:
    python spatial_audio_example.py

Controls:
    w/s - Move listener forward/backward
    a/d - Move listener left/right
    q/e - Move listener up/down
    p   - Play sound at fixed position
    r   - Reset listener position
    q   - Quit
"""

import sys
import os

# Add build directory to path to find the game_audio module
build_dir = os.path.join(os.path.dirname(__file__), '..', 'build', 'Debug')
if os.path.exists(build_dir):
    sys.path.insert(0, build_dir)

import game_audio

# Sound directory
SOUND_DIR = os.path.join(os.path.dirname(__file__), '..', 'sound_files')

def main():
    """Main function - setup and run spatial audio demo"""
    print("=== Spatial Audio Example ===")
    print(f"Sound directory: {SOUND_DIR}\n")
    
    # Initialize audio system
    print("Initializing audio system...")
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    # Create a sound group for spatialized sounds
    sfx_group = audio.create_group()
    audio.set_group_volume(sfx_group, 1.0)
    
    # Load a looping music file for continuous spatial audio
    sound_file = os.path.join(SOUND_DIR, "clarinet.wav")
    if not os.path.exists(sound_file):
        print(f"Error: Sound file not found: {sound_file}")
        print("Please ensure sound files are available in the sound_files directory.")
        return
    
    print(f"Loading sound: {sound_file}")
    sound = audio.load_sound(sound_file, sfx_group)
    
    # Configure spatial audio for the sound
    # Position the sound at (5, 0, 0) - 5 units to the right
    sound_position = game_audio.Vec3(5.0, 0.0, 0.0)
    audio.set_sound_position(sound, sound_position)
    
    # Set distance attenuation parameters
    # min_distance: sound is at full volume within 1 unit
    # max_distance: sound fades to silence beyond 20 units
    # rolloff: 1.0 = linear falloff, 2.0 = inverse square (more realistic)
    audio.set_sound_min_distance(sound, 1.0)
    audio.set_sound_max_distance(sound, 20.0)
    audio.set_sound_rolloff(sound, 1.0)  # Linear falloff
    
    print("\nSound configured at position:", sound_position)
    print("Min distance: 1.0, Max distance: 20.0, Rolloff: 1.0")
    
    # Set the sound to loop continuously
    audio.set_sound_looping(sound, True)
    print("Sound set to loop continuously")
    
    # Start playing the sound immediately
    print("\nStarting playback...")
    audio.play_sound(sound)
    
    # Set up listener (player/camera position)
    # Start at origin, facing forward (negative Z in OpenGL-style coordinates)
    listener_position = game_audio.Vec3(0.0, 0.0, 0.0)
    listener_direction = game_audio.Vec3(0.0, 0.0, -1.0)  # Forward
    listener_up = game_audio.Vec3(0.0, 1.0, 0.0)  # Up
    
    audio.set_listener_position(listener_position)
    audio.set_listener_direction(listener_direction)
    audio.set_listener_up(listener_up)
    
    print("\nListener initialized at origin")
    print("Sound is 5 units to the right of the listener")
    
    # Print instructions
    print("\n=== Controls ===")
    print("w/s - Move listener forward/backward (Z axis)")
    print("a/d - Move listener left/right (X axis)")
    print("q/e - Move listener up/down (Y axis)")
    print("p   - Restart sound playback")
    print("r   - Reset listener position to origin")
    print("x   - Quit")
    print()
    
    # Movement speed
    move_speed = 0.5
    
    # Main loop
    running = True
    while running:
        try:
            command = input("> ").strip().lower()
            
            if not command:
                continue
            
            if command == 'w':
                # Move forward (negative Z)
                listener_position.z -= move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 's':
                # Move backward (positive Z)
                listener_position.z += move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 'a':
                # Move left (negative X)
                listener_position.x -= move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 'd':
                # Move right (positive X)
                listener_position.x += move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 'q':
                # Move up (positive Y)
                listener_position.y += move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 'e':
                # Move down (negative Y)
                listener_position.y -= move_speed
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener position: {listener_position}, Distance to sound: {distance:.2f}")
            
            elif command == 'p':
                # Restart the sound playback
                audio.play_sound(sound)
                distance = listener_position.distance(sound_position)
                print(f"Restarting sound at position {sound_position}")
                print(f"Listener distance: {distance:.2f} units")
            
            elif command == 'r':
                # Reset listener position
                listener_position = game_audio.Vec3(0.0, 0.0, 0.0)
                audio.set_listener_position(listener_position)
                distance = listener_position.distance(sound_position)
                print(f"Listener reset to origin, Distance to sound: {distance:.2f}")
            
            elif command == 'x':
                # Quit
                running = False
            
            else:
                print("Unknown command. Use w/s/a/d/q/e/p/r/x")
        
        except (EOFError, KeyboardInterrupt):
            print("\nExiting...")
            break
    
    # Cleanup
    print("Shutting down audio system...")
    session.close()
    print("Done!")

if __name__ == "__main__":
    main()

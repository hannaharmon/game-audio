"""
Python Interactive Audio Test - mirrors test_audio_2.cpp

Demonstrates layered music with digital and strings tracks.

Commands:
  v [0.0-1.0] - Set master volume
  m [0.0-1.0] - Set music volume
  s [0.0-1.0] - Set SFX volume
  x           - Play random sound effect
  o           - Toggle music on/off
  t           - Toggle music type (digital/strings)
  b           - Toggle battle mode (calm/intense)
  q           - Quit
"""

import sys
import os
import random
from datetime import timedelta

# Add build directory to path to find the game_audio module
build_dir = os.path.join(os.path.dirname(__file__), '..', 'build', 'Debug')
if os.path.exists(build_dir):
    sys.path.insert(0, build_dir)

import game_audio

# Sound directory
SOUND_DIR = os.path.join(os.path.dirname(__file__), '..', 'sound_files')

# State variables
digital_mode = True
battle = False
music_on = True

def setup_audio():
    """Initialize the audio system and create the layered music track"""
    print("Initializing audio system...")
    session = game_audio.AudioSession()
    audio = game_audio.AudioManager.get_instance()
    
    print("Audio system initialized successfully")
    
    # Create groups
    print("Creating audio groups...")
    music_group = audio.create_group()
    sfx_group = audio.create_group()
    
    # Set initial volumes
    print("Setting initial group volumes...")
    audio.set_group_volume(music_group, 0.7)
    audio.set_group_volume(sfx_group, 1.0)
    
    # Create layered music track
    print("Creating layered music track...")
    music_track = audio.create_track()
    
    # Add layers
    print("Adding layers to music track...")
    audio.add_layer(music_track, "digital_base",
                   os.path.join(SOUND_DIR, "digital_base.wav"), music_group)
    audio.add_layer(music_track, "digital_battle",
                   os.path.join(SOUND_DIR, "digital_battle.wav"), music_group)
    audio.add_layer(music_track, "strings_base",
                   os.path.join(SOUND_DIR, "strings_base.wav"), music_group)
    audio.add_layer(music_track, "strings_battle",
                   os.path.join(SOUND_DIR, "strings_battle.wav"), music_group)
    
    print("Setting initial layer volumes...")
    # Start with digital base
    audio.set_layer_volume(music_track, "digital_base", 1.0)
    audio.set_layer_volume(music_track, "digital_battle", 0.0)
    audio.set_layer_volume(music_track, "strings_base", 0.0)
    audio.set_layer_volume(music_track, "strings_battle", 0.0)
    
    # Start playing
    print("Starting playback...")
    audio.play_track(music_track)
    
    # Load sound effects
    touch_sounds = []
    for i in range(1, 9):
        sfx_path = os.path.join(SOUND_DIR, f"touch_{i}.wav")
        if os.path.exists(sfx_path):
            touch_sounds.append(audio.load_sound(sfx_path, sfx_group))
    
    hit_sfx = audio.load_sound(os.path.join(SOUND_DIR, "hit.wav"), sfx_group)
    
    return session, audio, music_track, music_group, sfx_group, touch_sounds, hit_sfx

def process_input(command, audio, music_track, music_group, sfx_group, touch_sounds, hit_sfx):
    """Process user input commands"""
    global digital_mode, battle, music_on
    
    parts = command.strip().split()
    if not parts:
        return True
    
    cmd = parts[0].lower()
    
    # Set master volume
    if cmd == 'v' and len(parts) > 1:
        try:
            volume = float(parts[1])
            audio.set_master_volume(volume)
            print(f"Master volume set to {volume}")
        except ValueError:
            print("Invalid volume value")
    
    # Set music volume
    elif cmd == 'm' and len(parts) > 1:
        try:
            volume = float(parts[1])
            audio.set_group_volume(music_group, volume)
            print(f"Music volume set to {volume}")
        except ValueError:
            print("Invalid volume value")
    
    # Set SFX volume
    elif cmd == 's' and len(parts) > 1:
        try:
            volume = float(parts[1])
            audio.set_group_volume(sfx_group, volume)
            print(f"SFX volume set to {volume}")
        except ValueError:
            print("Invalid volume value")
    
    # Play random sound effect
    elif cmd == 'x':
        if touch_sounds:
            sfx = random.choice(touch_sounds)
            audio.play_sound(sfx)
            print("Playing random touch sound")
        else:
            audio.play_sound(hit_sfx)
            print("Playing hit sound")
    
    # Toggle music on/off
    elif cmd == 'o':
        music_on = not music_on
        target = 0.7 if music_on else 0.0
        audio.fade_group(music_group, target, timedelta(seconds=2))
        print(f"Music {'ON' if music_on else 'OFF'}")
    
    # Toggle music type (digital/strings)
    elif cmd == 't':
        digital_mode = not digital_mode
        print(f"Switching to {'DIGITAL' if digital_mode else 'STRINGS'} mode")
        
        if digital_mode:
            # Fade in digital layers
            base_layer = "digital_battle" if battle else "digital_base"
            audio.fade_layer(music_track, base_layer, 1.0, timedelta(seconds=2))
            # Fade out strings layers
            audio.fade_layer(music_track, "strings_base", 0.0, timedelta(seconds=2))
            audio.fade_layer(music_track, "strings_battle", 0.0, timedelta(seconds=2))
        else:
            # Fade in strings layers
            base_layer = "strings_battle" if battle else "strings_base"
            audio.fade_layer(music_track, base_layer, 1.0, timedelta(seconds=2))
            # Fade out digital layers
            audio.fade_layer(music_track, "digital_base", 0.0, timedelta(seconds=2))
            audio.fade_layer(music_track, "digital_battle", 0.0, timedelta(seconds=2))
    
    # Toggle battle mode
    elif cmd == 'b':
        battle = not battle
        print(f"Battle mode: {'INTENSE' if battle else 'CALM'}")
        
        if digital_mode:
            if battle:
                audio.fade_layer(music_track, "digital_base", 0.0, timedelta(seconds=2))
                audio.fade_layer(music_track, "digital_battle", 1.0, timedelta(seconds=2))
            else:
                audio.fade_layer(music_track, "digital_battle", 0.0, timedelta(seconds=2))
                audio.fade_layer(music_track, "digital_base", 1.0, timedelta(seconds=2))
        else:
            if battle:
                audio.fade_layer(music_track, "strings_base", 0.0, timedelta(seconds=2))
                audio.fade_layer(music_track, "strings_battle", 1.0, timedelta(seconds=2))
            else:
                audio.fade_layer(music_track, "strings_battle", 0.0, timedelta(seconds=2))
                audio.fade_layer(music_track, "strings_base", 1.0, timedelta(seconds=2))
    
    # Quit
    elif cmd == 'q':
        return False
    
    else:
        print("Unknown command")
    
    return True

def main():
    """Main function - setup and run interactive loop"""
    print("=== Python Audio Module Test ===")
    print(f"Sound directory: {SOUND_DIR}\n")
    
    # Setup audio system
    session, audio, music_track, music_group, sfx_group, touch_sounds, hit_sfx = setup_audio()
    
    # Print commands
    print("\n=== Interactive Audio Test ===")
    print("Commands:")
    print("  v [0.0-1.0] - Set master volume")
    print("  m [0.0-1.0] - Set music volume")
    print("  s [0.0-1.0] - Set SFX volume")
    print("  x           - Play random sound effect")
    print("  o           - Toggle music on/off")
    print("  t           - Toggle music type (digital/strings)")
    print("  b           - Toggle battle mode (calm/intense)")
    print("  q           - Quit")
    print()
    
    # Main loop
    running = True
    while running:
        try:
            command = input("> ")
            running = process_input(command, audio, music_track, music_group, 
                                   sfx_group, touch_sounds, hit_sfx)
        except (EOFError, KeyboardInterrupt):
            print("\nExiting...")
            break
    
    # Cleanup
    print("Shutting down audio system...")
    session.close()
    print("Done!")

if __name__ == "__main__":
    main()

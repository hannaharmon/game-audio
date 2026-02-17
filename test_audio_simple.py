import game_audio
import os

# Create session - this keeps the audio system alive
session = game_audio.AudioSession()
audio = game_audio.AudioManager.get_instance()

music_group = audio.create_group("music")
audio.set_group_volume(music_group, 0.5)

music_track = audio.create_track()

# Get the full path to the audio file
# Adjust this path to match where your alto_flute.wav file is located
script_dir = os.path.dirname(os.path.abspath(__file__))
sound_dir = os.path.join(script_dir, "sound_files")
audio_file_path = os.path.join(sound_dir, "alto_flute.wav")

# Check if file exists
if not os.path.exists(audio_file_path):
    print(f"ERROR: Audio file not found at: {audio_file_path}")
    print("Please make sure alto_flute.wav is in the sound_files directory")
    session.close()
    exit(1)

# Add layer: (track, layer_name, filepath, group_name)
# Layer name is just an identifier, filepath is the actual file
layer_name = "alto_flute"
audio.add_layer(music_track, layer_name, audio_file_path, "music")

# IMPORTANT: Layers start at volume 0.0 by default, so you must set it > 0 to hear it
# Use the layer name (not "music") to set the volume
audio.set_layer_volume(music_track, layer_name, 0.5)

# Start playing
audio.play_track(music_track)

print("Audio playing... Press Ctrl+C to stop")
print(f"Playing: {audio_file_path}")

# Keep the script running to keep audio playing
# The session must stay alive for audio to continue
try:
    while True:
        import time
        time.sleep(0.1)  # Small sleep to avoid busy-waiting
except KeyboardInterrupt:
    print("\nStopping audio...")
    session.close()
    print("Done!")

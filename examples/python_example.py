# Python example using the audio module
import audio_py
from datetime import timedelta

# Initialize the audio system (recommended: keep session alive for app lifetime)
session = audio_py.AudioSession()
audio = audio_py.AudioManager.get_instance()

print("Audio system initialized successfully!")

# Create audio groups
music_group = audio.create_group("music")
sfx_group = audio.create_group("sfx")

# Set initial group volumes
audio.set_group_volume(music_group, 0.7)  # Music quieter than SFX
audio.set_group_volume(sfx_group, 1.0)

print(f"Created music group: {music_group}")
print(f"Created SFX group: {sfx_group}")

# Create a layered music track
music_track = audio.create_track()
print(f"Created track: {music_track}")

# Add layers to the music track
# Note: Update these paths to point to your actual sound files
audio.add_layer(music_track, "kick", "sound_files/kick.wav", "music")
audio.add_layer(music_track, "bass", "sound_files/bass.wav", "music")
audio.add_layer(music_track, "melody", "sound_files/melody.wav", "music")

print("Added layers to track")

# Set initial layer volumes
audio.set_layer_volume(music_track, "kick", 1.0)
audio.set_layer_volume(music_track, "bass", 0.8)
audio.set_layer_volume(music_track, "melody", 0.0)  # Start with melody muted

# Start playing
audio.play_track(music_track)
print("Started playing track")

# Fade in the melody layer over 2 seconds
audio.fade_layer(music_track, "melody", 0.7, timedelta(seconds=2))
print("Fading in melody...")

# Load and play a sound effect
sfx = audio.load_sound("sound_files/explosion.wav", sfx_group)
audio.start_sound(sfx)
print("Played sound effect")

# Random sound container example
config = audio_py.RandomSoundContainerConfig()
config.avoid_repeat = True
config.pitch_min = 0.95
config.pitch_max = 1.05
config.group = sfx_group

footsteps = audio_py.RandomSoundContainer("footsteps", config)
footsteps.load_from_folder("sound_files/footsteps")
footsteps.play()
print("Played random footstep")

# Cleanup
print("\nPress Enter to shutdown...")
input()

session.close()
print("Audio system shut down")

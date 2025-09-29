# Game Audio Module

A C++ audio system built on miniaudio for game development.

## Overview

This audio module provides a high-level interface for managing game audio. It supports:

- **Layered Music**: Tracks with multiple synchronized audio layers that can fade in/out independently
- **Sound Groups**: Categorize and control collections of sounds
- **Volume Control**: Master volume, group volume, and individual sound volume
- **Smooth Transitions**: Fade sounds in and out with customizable durations

## Architecture

The system uses a layered architecture:

- **AudioManager**: User-facing API (singleton) - the main interface for game code
- **AudioTrack**: Manages collections of synchronized audio layers
- **AudioGroup**: Groups sounds together for collective control
- **Sound**: Wraps individual sound instances
- **AudioSystem**: Low-level integration with miniaudio

## Usage Example

```cpp
// Initialize the audio system
auto& audio = audio::AudioManager::GetInstance();
if (!audio.Initialize()) {
    // Handle initialization failure
    return;
}

// Create audio groups
auto music_group = audio.CreateGroup("music");
auto sfx_group = audio.CreateGroup("sfx");

// Set initial group volumes
audio.SetGroupVolume(music_group, 0.7f);  // Music quieter than SFX
audio.SetGroupVolume(sfx_group, 1.0f);

// Create a layered music track
auto music_track = audio.CreateTrack();

// Add layers to the music track
audio.AddLayer(music_track, "kick", "sound_files/kick.wav", "music");
audio.AddLayer(music_track, "bass", "sound_files/bass.wav", "music");
audio.AddLayer(music_track, "melody", "sound_files/melody.wav", "music");

// Set initial layer volumes
audio.SetLayerVolume(music_track, "kick", 1.0f);
audio.SetLayerVolume(music_track, "bass", 0.8f);
audio.SetLayerVolume(music_track, "melody", 0.0f);  // Start with melody muted

// Start playing
audio.PlayTrack(music_track);

// Load a sound effect
auto sfx = audio.LoadSound("sound_files/explosion.wav");

// During gameplay: fade in the melody layer
audio.FadeLayer(music_track, "melody", 0.7f, 2000ms);

// Play a sound effect
audio.StartSound(sfx);
```

## Documentation

The codebase is documented using Doxygen. To generate the documentation:

1. Ensure Doxygen is installed on your system
   - Windows: Download and install from [Doxygen's website](https://www.doxygen.nl/download.html)
   - macOS: `brew install doxygen`
   - Linux: `sudo apt-get install doxygen` (Ubuntu/Debian) or `sudo yum install doxygen` (Fedora/CentOS)

2. Run the documentation generation script:
   - Windows: `generate_docs.bat`
   - Linux/Mac: `./generate_docs.sh`
   
   Alternatively, if you're using CMake:
   ```bash
   cd build
   cmake --build . --target docs
   ```

3. Open `docs/html/index.html` in a web browser

## Building

The project uses CMake for building:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running

After building the project, you can run the test application to verify everything is working correctly:

### Windows
```cmd
cd build\Debug
test_audio.exe
```

### macOS/Linux
```bash
cd build
./test_audio
```

When the test application is running, you can use the following commands:
- `v [0.0-1.0]` - Set master volume (e.g., `v 0.5` for 50% volume)
- `m [0.0-1.0]` - Set music volume
- `s [0.0-1.0]` - Set SFX volume
- `x` - Play a sound effect
- `b` - Toggle battle mode (fades in/out different layers)
- `q` - Quit the application

The test application demonstrates layered music with multiple instrument tracks that can be faded in and out independently, as well as sound effect playback.

## Future Enhancements

Planned future enhancements include:
- 3D audio positioning
- DSP effects (reverb, EQ)
- Advanced music transitions
- Random sound variations
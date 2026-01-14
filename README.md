# Game Audio Module

A C++ audio system built on miniaudio for game development.

## Overview

This audio module provides a high-level interface for managing game audio. It supports:

- **Layered Music**: Tracks with multiple synchronized audio layers that can fade in/out independently
- **Sound Groups**: Categorize and control collections of sounds
- **Volume Control**: Master volume, group volume, and individual sound volume
- **Smooth Transitions**: Fade sounds in and out with customizable durations
- **Random Sound Containers**: Play randomized sounds with pitch variation (similar to Wwise random containers)
- **Music Player**: High-level music track management with smooth transitions
- **SFX Player**: Centralized sound effect playback system

## Architecture

The system uses a layered architecture:

### Core Components
- **AudioManager**: User-facing API (singleton) - the main interface for game code
- **AudioTrack**: Manages collections of synchronized audio layers
- **AudioGroup**: Groups sounds together for collective control
- **Sound**: Wraps individual sound instances
- **AudioSystem**: Low-level integration with miniaudio

### High-Level Utilities
- **MusicPlayer**: Singleton for managing music tracks and transitions between game states
- **SFXPlayer**: Singleton for playing sound effects with randomization
- **RandomSoundContainer**: Container for playing randomized sounds with pitch variation and repeat avoidance

## Usage Example

### Basic Setup
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

### Using High-Level Components

#### Music Player
```cpp
// Initialize the music player
auto& music = audio::MusicPlayer::Get();
music.Initialize(music_group);

// Fade to a specific track
music.FadeTo("parchment", 2.0f);  // Fade to parchment music over 2 seconds

// Set track volume
music.SetVolume("notebook", 0.8f);
```

#### SFX Player
```cpp
// Initialize the SFX player
auto& sfx = audio::SFXPlayer::Get();
sfx.Initialize(sfx_group);

// Play sound effects (automatically randomized if multiple variants exist)
sfx.Play("footstep");
sfx.PlayWithVolume("explosion", 0.5f);
```

#### Random Sound Container
```cpp
// Create a random sound container with configuration
audio::RandomSoundContainerConfig config;
config.avoidRepeat = true;        // Don't play the same sound twice in a row
config.pitchMin = 0.95f;          // Slightly lower pitch
config.pitchMax = 1.05f;          // Slightly higher pitch
config.group = sfx_group;

auto footsteps = std::make_unique<audio::RandomSoundContainer>("footsteps", config);
footsteps->LoadFromFolder("sound_files/footsteps");  // Load all .wav files from folder

// Play a random footstep sound
footsteps->Play();
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

### Platform-Specific Notes

#### Windows
The project links against `winmm` automatically. Build outputs will be in `build/Debug` or `build/Release`.

#### macOS/Linux
No additional dependencies required. The miniaudio library handles platform-specific audio backends automatically.

## Project Structure

```
miniaudio/
├── audio/                    # Audio system source files
│   ├── audio_manager.h/cpp   # Main API
│   ├── audio_system.h/cpp    # Low-level miniaudio interface
│   ├── audio_track.h/cpp     # Layered music tracks
│   ├── audio_group.h/cpp     # Sound grouping
│   ├── sound.h/cpp           # Individual sounds
│   ├── music_player.h/cpp    # High-level music management
│   ├── sfx_player.h/cpp      # High-level SFX management
│   └── random_sound_container.h/cpp  # Randomized sound playback
├── include/
│   └── miniaudio/            # miniaudio library
│       ├── miniaudio.h
│       └── miniaudio.cpp
├── examples/                 # Example applications
│   ├── test_audio.cpp        # Basic test with instrument layers
│   └── test_audio_2.cpp      # Advanced test with mode switching
├── sound_files/              # Audio assets
├── docs/                     # Generated documentation
├── CMakeLists.txt            # Build configuration
├── Doxyfile                  # Doxygen configuration
└── README.md                 # This file
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

### Test Application Commands

When the test application is running, you can use the following commands:

#### test_audio.cpp (Basic Example)
- `v [0.0-1.0]` - Set master volume (e.g., `v 0.5` for 50% volume)
- `m [0.0-1.0]` - Set music volume
- `s [0.0-1.0]` - Set SFX volume
- `x` - Play a sound effect
- `b` - Toggle battle mode (fades in/out different instrument layers)
- `q` - Quit the application

#### test_audio_2.cpp (Advanced Example)
- `v [0.0-1.0]` - Set master volume
- `m [0.0-1.0]` - Set music volume
- `s [0.0-1.0]` - Set SFX volume
- `x` - Play a random sound effect
- `o` - Toggle music on/off (fade in/out)
- `t` - Toggle music type (digital/strings)
- `b` - Toggle battle mode (calm/intense)
- `q` - Quit the application

The test applications demonstrate:
- Layered music with multiple synchronized audio tracks
- Independent volume control for each layer
- Smooth crossfading between layers
- Dynamic music adaptation based on game state
- Sound effect playback with randomization

## Future Enhancements

Planned future enhancements include:
- 3D audio positioning and spatialization
- DSP effects (reverb, EQ, compression)
- Advanced music transitions (crossfade, stinger support)
- Streaming audio for large files
- Audio resource pooling and memory management
- MIDI support
- Recording and voice chat integration

## License

This project uses the miniaudio library, which is available under public domain or MIT-0 license.
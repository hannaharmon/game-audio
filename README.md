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
- **Python Bindings**: Use the full audio system from Python projects via pybind11

## Python Usage

The audio module can be used directly from Python projects, including those using the Basilisk game engine.

### Installation

#### Option 1: Build and Install Locally

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install . --prefix ../python
cd ..
pip install -e .
```

#### Option 2: Add as CMake Dependency (Basilisk-style)

In your Python project's `CMakeLists.txt`:

```cmake
# Add the audio module as a subdirectory or via FetchContent
add_subdirectory(path/to/audio_module)

# Link your project to the audio module (if needed)
# The audio_py module will be built automatically
```

### Python API Example

```python
import audio_py
from datetime import timedelta

# Initialize the audio system
audio = audio_py.AudioManager.get_instance()
audio.initialize()

# Create audio groups
music_group = audio.create_group("music")
sfx_group = audio.create_group("sfx")

# Set volumes
audio.set_master_volume(0.8)
audio.set_group_volume(music_group, 0.7)

# High-level music control
music = audio_py.MusicPlayer.get()
music.initialize(music_group)
music.fade_to("parchment", 2.0)  # Fade to track over 2 seconds

# High-level SFX control
sfx = audio_py.SFXPlayer.get()
sfx.initialize(sfx_group)
sfx.play("footstep")

# Random sound containers
config = audio_py.RandomSoundContainerConfig()
config.avoid_repeat = True
config.pitch_min = 0.95
config.pitch_max = 1.05

footsteps = audio_py.RandomSoundContainer("footsteps", config)
footsteps.load_from_folder("sound_files/footsteps")
footsteps.play()

# Cleanup
audio.shutdown()
```

See [examples/python_example.py](examples/python_example.py) for a complete working example.

### Running Python Examples

After building the project:

```bash
# Run the interactive Python example (mirrors test_audio_2.cpp)
python examples/python_interactive.py

# Run the Python tests
python tests/test_python.py
```

The Python bindings are in `build/Debug/audio_py.pyd` (Windows) or `build/audio_py.so` (Linux/Mac). The examples automatically add this to the Python path.

## C++ Usage Example

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

### Python Bindings
- **audio_py**: Python module exposing the full C++ API via pybind11
- Compatible with Basilisk game engine and other Python projects
- Automatic type stub generation for IDE autocomplete support

## C++ Usage Example

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

The codebase is documented using Doxygen.

### Viewing Documentation

**C++ API Reference**: Full Doxygen documentation is automatically generated and deployed to GitHub Pages on every commit. Visit the [online documentation](https://github.com/<your-username>/<repo-name>/wiki) for the complete C++ API reference.

**Python API**: See [PYTHON_BINDINGS.md](PYTHON_BINDINGS.md) for Python-specific usage, examples, and integration guides.

### C++ Integration

To use this audio module in a C++ project, add it as a CMake dependency:

```cmake
# Method 1: Via FetchContent (recommended)
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/<your-username>/<repo-name>
    GIT_TAG main
)
FetchContent_MakeAvailable(audio_module)

# Link to your target
target_link_libraries(your_game PRIVATE audio_module)

# Method 2: As a subdirectory
add_subdirectory(path/to/audio_module)
target_link_libraries(your_game PRIVATE audio_module)
```

Then include headers:
```cpp
#include "audio_manager.h"
#include "music_player.h"
#include "sfx_player.h"
```

### Building Documentation Locally (optional)

If you need to build docs locally:
```bash
cd build
cmake --build . --target docs
# Open docs/html/index.html
```

## Building

The project uses CMake and builds two C++ executables plus optional Python bindings:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

**Build Outputs:**
- `test_audio.exe` - Interactive test application for manual testing
- `test_automated.exe` - Automated test suite (50 tests, no user input required)
- `audio_py` - Python module (if Python bindings are enabled)

### Build Options

- **`BUILD_PYTHON_BINDINGS`** (default: ON) - Build Python bindings via pybind11

To disable Python bindings:
```bash
cmake .. -DBUILD_PYTHON_BINDINGS=OFF
```

### Platform-Specific Notes

#### Windows
The project links against `winmm` automatically. Build outputs will be in `build/Debug` or `build/Release`.

#### macOS/Linux
No additional dependencies required. The miniaudio library handles platform-specific audio backends automatically (ALSA/PulseAudio on Linux, Core Audio on macOS).

## Testing

A comprehensive automated test suite with **70 tests** verifies system reliability:

```powershell
# Run all tests (Windows)
cd tests
.\run_all_tests.ps1
```

**Test Suite Includes:**

**Functional Tests (50 tests)** - `test_automated.exe`
- System initialization & shutdown
- Master volume control
- Audio group operations (create, destroy, volume, fading)
- Sound loading/unloading (with and without groups)
- Sound playback control (play, stop, volume, pitch)
- Multi-layer track operations
- Multiple concurrent sound instances
- Random sound folder playback
- Error handling for invalid handles/files
- Mass resource cleanup
- Concurrent operations
- Edge cases (rapid create/destroy, etc.)

**Cross-Platform Tests (20 tests)** - `test_cross_platform.ps1`
- Platform-specific code isolation
- Standard library usage validation
- Path handling compatibility
- Compiler compatibility
- Build system portability
- Type safety verification

All tests run automatically and verify actual behavior (not just "doesn't crash"). The system is production-ready when all tests pass.

## Configuration

1. **Command-line argument** (highest priority):
   ```bash
   ./test_audio "/path/to/sounds/"
   ```

2. **CMake compile-time definition** (default):
   ```cmake
   target_compile_definitions(test_audio PRIVATE SOUND_FILES_DIR="/path/to/sounds/")
   ```

3. **Fallback default** (if neither above is set):
   ```cpp
   #define SOUND_FILES_DIR "../../sound_files/"
   ```

## Project Structure

```
audio/
├── audio_system.h/cpp        # Core audio backend (miniaudio wrapper)
├── audio_manager.h/cpp       # High-level audio management
├── audio_group.h/cpp         # Volume groups system
├── audio_track.h/cpp         # Multi-layer audio tracks
├── sound.h/cpp               # Sound data management
├── music_player.h            # High-level music management (header-only)
├── sfx_player.h/cpp          # High-level SFX management
└── random_sound_container.h/cpp  # Randomized sound playback
bindings/                     # Python bindings (pybind11)
├── bindings.cpp              # Main pybind11 module
├── audio_manager.cpp         # AudioManager bindings
├── music_player.cpp          # MusicPlayer bindings
├── sfx_player.cpp            # SFXPlayer bindings
└── random_sound_container.cpp  # RandomSoundContainer bindings
audio_py/                     # Python package
└── __init__.py               # Python package initialization
include/
└── miniaudio/                # miniaudio library
    ├── miniaudio.h
    └── miniaudio.cpp
examples/                     # Example applications
├── test_audio.cpp            # Basic test with instrument layers
├── test_audio_2.cpp          # Advanced test with mode switching
└── python_example.py         # Python usage example
tests/                        # Automated test suite
├── test_automated.cpp        # 50 functional tests
├── run_all_tests.ps1         # Main test runner
└── test_cross_platform.ps1   # 20 platform compatibility tests
sound_files/                  # Audio assets
build/                        # Build output directory
└── Debug/
    ├── test_audio.exe        # Interactive test application
    ├── test_automated.exe    # Automated test suite
    └── audio_py.pyd          # Python module (Windows) / .so (Linux/Mac)
docs/                         # Generated documentation
CMakeLists.txt                # Build configuration
pyproject.toml                # Python package configuration
Doxyfile                      # Doxygen configuration
README.md                     # This file
```

**Note:** All headers use `#pragma once` for consistency and simplicity.

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
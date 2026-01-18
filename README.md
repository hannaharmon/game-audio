# Game Audio Module

A C++20 audio system built on miniaudio with full Python bindings for game development.

## Features

- **Layered Music**: Multi-track audio with independent layer control and fading
- **Sound Groups**: Categorize and control collections of sounds
- **Volume Control**: Master, group, and individual sound volume
- **Smooth Transitions**: Fade sounds in/out with customizable durations
- **Random Sound Containers**: Randomized playback with pitch variation
- **High-Level API**: Core primitives designed for user-defined wrappers
- **Python Bindings**: Full pybind11 bindings for Python projects (including [Basilisk engine](https://github.com/BasiliskGroup/BasiliskEngine))
- **Cross-Platform**: Windows, macOS, Linux via miniaudio

## Quick Start

### For Python Users

**1. Add to your project's CMakeLists.txt:**
```cmake
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v1.0.0  # Use a specific version tag for stability
)
FetchContent_MakeAvailable(audio_module)
```

**Important**: Always use version tags (e.g., `v1.0.0`) rather than `main` branch. Using `main` means your project may break when breaking changes are merged. Version tags provide stability, predictability, and control over when you upgrade. See [RELEASE_MANAGEMENT.md](RELEASE_MANAGEMENT.md) for details.

**2. Use in Python (recommended):**
```python
import audio_py

# Initialize (keep the session alive for the app lifetime)
session = audio_py.AudioSession()
audio = audio_py.AudioManager.get_instance()

# Create groups and play
music_group = audio.create_group("music")
sfx_group = audio.create_group("sfx")

# Cleanup (optional; session destructor will also handle this)
session.close()
```

**Direct Usage (advanced/engine-controlled):**
```python
import audio_py

audio = audio_py.AudioManager.get_instance()
audio.initialize()

music_group = audio.create_group("music")
sfx_group = audio.create_group("sfx")

audio.shutdown()
```

**Full Guide**: [PYTHON_BINDINGS.md](PYTHON_BINDINGS.md)

### For C++ Users

**1. Add to your CMakeLists.txt:**
```cmake
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v1.0.0  # Pin to specific version for stability
)
FetchContent_MakeAvailable(audio_module)
target_link_libraries(your_game PRIVATE audio_module)
```

**Important**: Always use version tags (e.g., `v1.0.0`) rather than `main` branch. Using `main` means your project may break when breaking changes are merged. Version tags provide stability, predictability, and control over when you upgrade. See [RELEASE_MANAGEMENT.md](RELEASE_MANAGEMENT.md) for details.

**2. Use in C++ (recommended):**
```cpp
#include "audio_manager.h"
#include "audio_session.h"

// Initialize (keep the session alive for the app lifetime)
audio::AudioSession session;
auto& audio = audio::AudioManager::GetInstance();

// Create groups
auto music = audio.CreateGroup("music");
auto sfx = audio.CreateGroup("sfx");

// Cleanup handled automatically by AudioSession destructor (or call session.Close())
```

**Direct Usage (advanced/engine-controlled):**
```cpp
#include "audio_manager.h"

auto& audio = audio::AudioManager::GetInstance();
audio.Initialize();

auto music = audio.CreateGroup("music");
auto sfx = audio.CreateGroup("sfx");

audio.Shutdown();
```

**Full API Reference**: [Online Documentation](https://hannaharmon.github.io/game-audio)

## Examples

- **Python**: [examples/python_example.py](examples/python_example.py)
- **C++ Basic**: [examples/test_audio.cpp](examples/test_audio.cpp)
- **C++ Advanced**: [examples/test_audio_2.cpp](examples/test_audio_2.cpp)

## Building Locally

```bash
# Build (cross-platform via PowerShell)
./scripts/build.ps1 -Configurations Debug,Release  # Windows (C++ + Python)
./scripts/build.ps1 -Configurations Release        # Linux/macOS

# Run all tests (C++ + Python)
./tests/scripts/run_all_tests.ps1

# Run only C++ tests
./tests/scripts/run_cpp_tests.ps1

# Run only Python tests
./tests/scripts/run_python_tests.ps1
```

**Build Options:**
- `-DBUILD_PYTHON_BINDINGS=OFF` - Disable Python bindings
- `-DBUILD_AUDIO_TESTS=OFF` - Disable test builds
- `-DBUILD_AUDIO_EXAMPLES=OFF` - Disable example builds
- `-DAUDIO_ENABLE_LOGGING=ON` - Enable runtime logging output (default off)

## Architecture

**Core Components:**
- `AudioManager` - Main API (singleton)
- `AudioSession` - RAII helper for scoped initialization/shutdown
- `AudioTrack` - Multi-layer synchronized audio
- `AudioGroup` - Volume group management
- `Sound` - Individual sound instances
- `AudioSystem` - miniaudio wrapper

**Handles:**
- `TrackHandle`, `GroupHandle`, `SoundHandle` are opaque handle types returned by the API.

**High-Level Utilities:**
- `RandomSoundContainer` - Wwise-style random containers

## Testing

**Test Organization:**
```
tests/
  cpp/                      # C++ tests organized by functionality
    test_audio_initialization.cpp
    test_audio_volume.cpp
    test_audio_groups.cpp
    test_audio_sounds.cpp
    test_audio_tracks.cpp
    test_audio_validation.cpp
    test_audio_threading.cpp
    test_audio_resources.cpp
    test_common.h/cpp        # Shared test utilities
  python/                   # Python tests organized by functionality
    test_audio_initialization.py
    test_audio_volume.py
    test_audio_groups.py
    test_audio_sounds.py
    test_audio_tracks.py
    test_audio_validation.py
    test_audio_threading.py
    test_audio_resources.py
    test_common.py          # Shared test utilities
  scripts/                  # Test runners
    run_all_tests.ps1
    run_cpp_tests.ps1
    run_python_tests.ps1
```

**Comprehensive automated tests covering:**
- System initialization and lifecycle
- Volume control and clamping
- Group operations
- Sound loading and playback
- Track and layer management
- Input validation and error handling
- Thread safety and concurrent operations
- Resource management and cleanup
- Cross-platform compatibility

## Documentation

- **Python**: [PYTHON_BINDINGS.md](PYTHON_BINDINGS.md)
- **C++ API**: [Online Doxygen Docs](https://hannaharmon.github.io/game-audio)
- **Examples**: See `examples/` directory

## Logging

Logging is disabled by default. To enable runtime diagnostics, build with:

```
cmake -DAUDIO_ENABLE_LOGGING=ON ...
```

Then set the log level at runtime:

```
// C++
audio::AudioManager::SetLogLevel(audio::LogLevel::Info);
```

```
# Python
audio_py.AudioManager.set_log_level(audio_py.LogLevel.Info)
```

## License

This project is released under the Unlicense. See `LICENSE` for full terms and third-party notices (including miniaudio).
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

**Option 1: Install via pip (Recommended for most users)**

#### From PyPI (Recommended - Simplest Version Management)

```bash
# Install latest version
pip install game-audio-py

# Install specific version
pip install game-audio-py==1.1.0

# Install version range (e.g., any 1.x version, but not 2.0+)
pip install "game-audio-py>=1.1.0,<2.0.0"

# Upgrade to latest
pip install --upgrade game-audio-py

# Downgrade to specific version
pip install game-audio-py==1.0.0
```

#### From GitHub Releases (Alternative - For Specific Versions)

If you need a specific version or PyPI is unavailable, install directly from GitHub releases:

```bash
# Install specific version (pip will auto-select the correct wheel for your platform)
pip install https://github.com/hannaharmon/game-audio/releases/download/v1.1.0/game_audio_py-1.1.0-*.whl

# Or specify exact wheel for your platform (Windows example)
pip install https://github.com/hannaharmon/game-audio/releases/download/v1.1.0/game_audio_py-1.1.0-cp311-cp311-win_amd64.whl
```

**Note**: When installing from GitHub releases, you must uninstall before switching to PyPI (or vice versa), as pip treats them as different sources.

**Option 2: Build from source with CMake**

If you need to build from source or integrate into a CMake project:

**1. Add to your project's CMakeLists.txt:**
```cmake
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v1.1.0  # Use a specific version tag for stability
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

**Note**: For use with game engines like [Basilisk Engine](https://github.com/BasiliskGroup/BasiliskEngine), you can simply use `pip install game-audio-py` instead of adding it to your CMakeLists.txt. This makes integration much simpler!

### For C++ Users

**1. Add to your CMakeLists.txt:**
```cmake
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v1.1.0  # Pin to specific version for stability
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

- **Python**: [examples/python_interactive.py](examples/python_interactive.py)
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

Run the comprehensive test suite:

```bash
# Run all tests (C++ + Python, both source build and installed wheel)
./tests/scripts/run_all_tests.ps1

# Run only C++ tests
./tests/scripts/run_cpp_tests.ps1

# Run only Python tests (source build)
./tests/scripts/run_python_tests.ps1

# Run only Python tests (installed wheel)
./tests/scripts/run_python_tests.ps1 -UseWheel
```

**The test suite covers:**
- **System initialization and lifecycle** - AudioSession, AudioManager initialization/shutdown
- **Logging controls** - Runtime log level configuration and output
- **Volume control** - Master, group, and individual sound volume with proper clamping
- **Group operations** - Creation, destruction, volume control, and management
- **Sound loading and playback** - File loading, playback control, and state management
- **Track and layer management** - Multi-track audio, layer control, and synchronization
- **Input validation** - Error handling for invalid handles, paths, and parameters
- **Thread safety** - Concurrent operations and resource access
- **Resource management** - Proper cleanup, handle validation, and memory management
- **Cross-platform compatibility** - Platform-specific code isolation and portability checks

Tests run automatically on every push via GitHub Actions, validating both source builds and installed Python wheels on Windows, Linux, and macOS.

## Documentation

- **Python**: [PYTHON_BINDINGS.md](PYTHON_BINDINGS.md)
- **C++ API**: [Online Doxygen Docs](https://hannaharmon.github.io/game-audio)
- **Examples**: See `examples/` directory

## Logging

Logging is always available but defaults to `Off`. Control it at runtime:

```cpp
// C++
audio::AudioManager::SetLogLevel(audio::LogLevel::Info);  // Enable logging
audio::AudioManager::SetLogLevel(audio::LogLevel::Off);    // Disable logging
```

```python
# Python
audio_py.AudioManager.set_log_level(audio_py.LogLevel.Info)  # Enable logging
audio_py.AudioManager.set_log_level(audio_py.LogLevel.Off)    # Disable logging
```

## License

This project is released under the Unlicense. See `LICENSE` for full terms and third-party notices (including miniaudio).
# Game Audio Module - Python Bindings Setup Guide

> **For Python Users**: This is your primary guide. The Python API is a 1:1 mapping of the C++ API, so C++ method names translate directly to Python (e.g., `Initialize()` → `initialize()`). The [online C++ API documentation](https://hannaharmon.github.io/game-audio) provides detailed parameter information applicable to both languages.
> 
> **For C++ Users**: See the [README.md](README.md#c-integration) for CMake integration instructions and the [online API docs](https://hannaharmon.github.io/game-audio) for full API reference.

This document explains how to use the audio module in Python projects, including [Basilisk engine](https://github.com/BasiliskGroup/BasiliskEngine) projects.

## Quick Start

### Installation Methods

#### Method 1: Install Pre-built Wheel (Recommended)

The easiest way to use the audio module is to install a pre-built wheel:

#### From PyPI (Recommended - Simplest Version Management)

```bash
# Install latest version
pip install game-audio

# Install specific version
pip install game-audio==2.0.0

# Install version range (e.g., any 1.x version, but not 2.0+)
pip install "game-audio>=2.0.0,<3.0.0"

# Upgrade to latest
pip install --upgrade game-audio

# Downgrade to specific version
pip install game-audio==1.0.0
```

#### From GitHub Releases (Alternative - For Specific Versions)

If you need a specific version or PyPI is unavailable, install directly from GitHub releases:

```bash
# Install specific version (pip will auto-select the correct wheel for your platform)
pip install https://github.com/hannaharmon/game-audio/releases/download/v2.0.0/game_audio-2.0.0-*.whl

# Or specify exact wheel for your platform (Windows example)
pip install https://github.com/hannaharmon/game-audio/releases/download/v2.0.0/game_audio-2.0.0-cp311-cp311-win_amd64.whl
```

**Note**: 
- Wheels are automatically built for Windows, Linux, and macOS for Python 3.8-3.12 on every release. Check the [GitHub Releases](https://github.com/hannaharmon/game-audio/releases) page for available wheels.
- When installing from GitHub releases, you must uninstall before switching to PyPI (or vice versa), as pip treats them as different sources.

#### Method 2: Build from Source with CMake

If you need to build from source or integrate into a CMake project:

**1. Build the Python Module**

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

This will create the `game_audio` Python module in the build directory.

**2. Install Locally**

##### Option A: Development Install (editable)
```bash
pip install -e .
```

##### Option B: Regular Install
```bash
cmake --install . --prefix ./python
pip install .
```

##### Option C: Build Python Wheel Locally
```bash
pip install build scikit-build-core cmake pybind11 ninja
python -m build --wheel
pip install dist/game_audio-*.whl
```

### 3. Use in Python (recommended)

```python
import game_audio

# Initialize (keep the session alive for the app lifetime)
session = game_audio.AudioSession()
audio = game_audio.AudioManager.get_instance()

# Create groups
music = audio.create_group("music")
sfx = audio.create_group("sfx")

# Use the audio system
audio.set_master_volume(0.8)

# Cleanup (optional; session destructor will also handle this)
session.close()
```

### Direct Usage (advanced/engine-controlled)
```python
import game_audio

audio = game_audio.AudioManager.get_instance()
audio.initialize()

music = audio.create_group("music")
sfx = audio.create_group("sfx")

audio.set_master_volume(0.8)
audio.shutdown()
```

## Logging and Diagnostics

Logging is always available but defaults to `Off`. Control it at runtime:

```python
import game_audio

# Enable logging (default is Off, so no output until you enable it)
game_audio.AudioManager.set_log_level(game_audio.LogLevel.Info)  # Enable info-level logging
game_audio.AudioManager.set_log_level(game_audio.LogLevel.Debug)  # Enable debug-level logging
game_audio.AudioManager.set_log_level(game_audio.LogLevel.Off)    # Disable logging
```

## Using with Basilisk Engine

If you're using this audio module with the [Basilisk game engine](https://github.com/BasiliskGroup/BasiliskEngine), you have several integration options:

### Method 1: Install via pip (Simplest - Recommended)

Just install the package using pip, no CMake configuration needed:

```bash
pip install game-audio
```

Then in your Python code:
```python
import game_audio
# Use the module as normal
```

This is the simplest method and works seamlessly with Basilisk Engine's Python-based workflow.

### Method 2: Via FetchContent (For CMake Integration)

If you need to integrate the audio module into your CMake build system:

```cmake
include(FetchContent)

FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v2.0.0  # Pin to specific version for stability
)
FetchContent_MakeAvailable(audio_module)
```

**Note**: Always use version tags (e.g., `v1.0.0`) rather than `main` branch. This ensures your project won't break when new versions are released. See [RELEASE_MANAGEMENT.md](RELEASE_MANAGEMENT.md) for version information.

### Method 3: As a Subdirectory

```cmake
# In your project's CMakeLists.txt
add_subdirectory(path/to/audio_module)

# The game_audio module will be built automatically alongside your project
```

## Python API Reference

### Handle Types

Audio resources are returned as opaque handle objects:

- `TrackHandle`
- `GroupHandle`
- `SoundHandle`

Handles have a `.value` property if you need the underlying integer, but the
recommended usage is to pass the handle objects directly back into API calls.

**Checking Handle Validity:**

You can check if a handle is valid in two ways (both work identically):

```python
# Method 1: Using is_valid() method (matches C++ API)
if handle.is_valid():
    # Use handle
    pass

# Method 2: Using Python's truthiness (more Pythonic)
if handle:
    # Use handle
    pass

# Both are equivalent - use whichever style you prefer
```

**Getting Invalid Handles:**

```python
# Get an invalid handle (useful for default arguments)
invalid_group = game_audio.GroupHandle.invalid()
```

### AudioManager (Singleton)
Main interface for the audio system.

```python
audio = game_audio.AudioManager.get_instance()
audio.initialize()
audio.shutdown()
audio.set_master_volume(0.8)
```

### AudioSession (Scoped Lifecycle)
RAII-style helper for initialization and shutdown (recommended for scripts/tests).

```python
session = game_audio.AudioSession()
audio = game_audio.AudioManager.get_instance()
audio.set_master_volume(0.8)
session.close()
```

### RandomSoundContainer
Randomized sound playback with pitch variation.

```python
config = game_audio.RandomSoundContainerConfig()
config.avoid_repeat = True
config.pitch_min = 0.95
config.pitch_max = 1.05

container = game_audio.RandomSoundContainer("footsteps", config)
container.load_from_folder("sounds/footsteps")
container.play()
```

## Type Hints and Autocomplete

The build system automatically generates Python type stubs (`.pyi` files) for IDE autocomplete support. These will be generated in the `game_audio/` directory.

To enable stub generation, install pybind11-stubgen:
```bash
pip install pybind11-stubgen
```

## Troubleshooting

### Module Not Found
If you get `ModuleNotFoundError: No module named 'game_audio'`:
1. Make sure you built the project: `cmake --build build`
2. The module file should be in `build/game_audio.pyd` (Windows) or `build/game_audio.so` (Linux/Mac)
3. Either install the package or add the build directory to your PYTHONPATH

### Import Errors
If you get import errors when using the module:
1. Make sure all dependencies are built (miniaudio, etc.)
2. On Windows, ensure the runtime dependencies (DLLs) are accessible
3. Check that your Python version matches the one used to build the module

### CMake Configuration Issues
If CMake can't find Python:
```bash
cmake .. -DPYTHON_EXECUTABLE=/path/to/python
```

To disable Python bindings entirely:
```bash
cmake .. -DBUILD_PYTHON_BINDINGS=OFF
```

## Example Projects

See the `examples/` directory for complete working examples:
- `python_interactive.py` - Interactive Python example with layered music
- `test_audio.cpp` - C++ usage for comparison

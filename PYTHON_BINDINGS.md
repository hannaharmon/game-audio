# Game Audio Module - Python Bindings Setup Guide

> **For Python Users**: This is your primary guide. The Python API is a 1:1 mapping of the C++ API, so C++ method names translate directly to Python (e.g., `Initialize()` → `initialize()`). The [online C++ API documentation](https://hannaharmon.github.io/game-audio) provides detailed parameter information applicable to both languages.
> 
> **For C++ Users**: See the [README.md](README.md#c-integration) for CMake integration instructions and the [online API docs](https://hannaharmon.github.io/game-audio) for full API reference.

This document explains how to use the audio module in Python projects, including [Basilisk engine](https://github.com/BasiliskGroup/BasiliskEngine) projects.

## Quick Start

### 1. Build the Python Module

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

This will create the `audio_py` Python module in the build directory.

### 2. Install Locally

#### Option A: Development Install (editable)
```bash
pip install -e .
```

#### Option B: Regular Install
```bash
cmake --install . --prefix ./python
pip install .
```

#### Option C: Build Python Wheel
```bash
pip install build
python -m build
pip install dist/game_audio_py-*.whl
```

### 3. Use in Python

```python
import audio_py

# Initialize
audio = audio_py.AudioManager.get_instance()
audio.initialize()

# Create groups
music = audio.create_group("music")
sfx = audio.create_group("sfx")

# Use the audio system
audio.set_master_volume(0.8)
```

## Using with Basilisk Engine

If you're using this audio module with the [Basilisk game engine](https://github.com/BasiliskGroup/BasiliskEngine), you can integrate it in your project's CMakeLists.txt:

### Method 1: As a Subdirectory

```cmake
# In your project's CMakeLists.txt
add_subdirectory(path/to/audio_module)

# The audio_py module will be built automatically alongside your project
```

### Method 2: Via FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG main
)
FetchContent_MakeAvailable(audio_module)
```

### Method 3: Install and Import

```cmake
# Install the audio module first
find_package(AudioModule REQUIRED)
```

## Python API Reference

### AudioManager (Singleton)
Main interface for the audio system.

```python
audio = audio_py.AudioManager.get_instance()
audio.initialize()
audio.shutdown()
audio.set_master_volume(0.8)
```

### RandomSoundContainer
Randomized sound playback with pitch variation.

```python
config = audio_py.RandomSoundContainerConfig()
config.avoid_repeat = True
config.pitch_min = 0.95
config.pitch_max = 1.05

container = audio_py.RandomSoundContainer("footsteps", config)
container.load_from_folder("sounds/footsteps")
container.play()
```

## Type Hints and Autocomplete

The build system automatically generates Python type stubs (`.pyi` files) for IDE autocomplete support. These will be generated in the `audio_py/` directory.

To enable stub generation, install pybind11-stubgen:
```bash
pip install pybind11-stubgen
```

## Troubleshooting

### Module Not Found
If you get `ModuleNotFoundError: No module named 'audio_py'`:
1. Make sure you built the project: `cmake --build build`
2. The module file should be in `build/audio_py.pyd` (Windows) or `build/audio_py.so` (Linux/Mac)
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
- `python_example.py` - Basic Python usage
- `test_audio.cpp` - C++ usage for comparison

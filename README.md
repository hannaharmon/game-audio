# Game Audio Module

A C++20 audio system built on miniaudio with full Python bindings for game development.

## Features

- **Layered Music**: Multi-track audio with independent layer control and fading
- **Sound Groups**: Categorize and control collections of sounds
- **Volume Control**: Master, group, and individual sound volume
- **Smooth Transitions**: Fade sounds in/out with customizable durations
- **Random Sound Containers**: Randomized playback with pitch variation (Wwise-style)
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
    GIT_TAG main
)
FetchContent_MakeAvailable(audio_module)
```

**2. Use in Python:**
```python
import audio_py

# Initialize
audio = audio_py.AudioManager.get_instance()
audio.initialize()

# Create groups and play
music_group = audio.create_group("music")
sfx_group = audio.create_group("sfx")

# Cleanup
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
    GIT_TAG main
)
FetchContent_MakeAvailable(audio_module)
target_link_libraries(your_game PRIVATE audio_module)
```

**2. Use in C++:**
```cpp
#include "audio_manager.h"
#include "music_player.h"
#include "sfx_player.h"

// Initialize
auto& audio = audio::AudioManager::GetInstance();
audio.Initialize();

// Create groups
auto music = audio.CreateGroup("music");
auto sfx = audio.CreateGroup("sfx");

// Cleanup
audio.Shutdown();
```

**Full API Reference**: [Online Documentation](https://hannaharmon.github.io/game-audio)

## Examples

- **Python**: [examples/python_example.py](examples/python_example.py)
- **C++ Basic**: [examples/test_audio.cpp](examples/test_audio.cpp)
- **C++ Advanced**: [examples/test_audio_2.cpp](examples/test_audio_2.cpp)

## Building Locally

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

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
- `AudioTrack` - Multi-layer synchronized audio
- `AudioGroup` - Volume group management
- `Sound` - Individual sound instances
- `AudioSystem` - miniaudio wrapper

**High-Level Utilities:**
- `RandomSoundContainer` - Wwise-style random containers

## Testing

**Test Organization:**
```
tests/
  cpp/                      # C++ tests
    test_audio_manager_basic.cpp
    test_audio_manager_error_handling.cpp
  python/                   # Python tests
    test_audio_manager_basic.py
    test_audio_manager_error_handling.py
    test_audio_manager_comprehensive.py
  scripts/                  # Test runners
    run_all_tests.ps1
    run_cpp_tests.ps1
    run_python_tests.ps1
```

**79 automated tests covering:**
- C++ functionality (42 tests)
- C++ error handling (37 tests)
- Python bindings (52 tests: 10 basic + 12 error + 30 comprehensive)
- Cross-platform compatibility
- Resource management and cleanup

## Documentation

- **Python**: [PYTHON_BINDINGS.md](PYTHON_BINDINGS.md)
- **C++ API**: [Online Doxygen Docs](https://hannaharmon.github.io/game-audio)
- **Examples**: See `examples/` directory

## License

This project uses miniaudio (public domain / MIT-0).
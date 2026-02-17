# CMakeLists.txt for Game Audio Module
# 
# This file defines the build configuration for the audio module
# and test application. It's designed to work on Windows, macOS, and Linux.

cmake_minimum_required(VERSION 3.10)
project(AudioModule VERSION 2.0
                    DESCRIPTION "Game Audio Module using miniaudio"
                    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDE IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options for building components
option(BUILD_AUDIO_TESTS "Build audio system tests" ON)
option(BUILD_AUDIO_EXAMPLES "Build audio system examples" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
# Note: Logging is always enabled at compile time but defaults to Off at runtime.
# Use set_log_level() to enable logging output.

# Source files for audio library (shared between test executables)
set(AUDIO_SOURCES
    src/audio_system.cpp
    src/audio_group.cpp
    src/sound.cpp
    src/audio_track.cpp
    src/audio_manager.cpp
    src/audio_session.cpp
    src/random_sound_container.cpp
    src/logging.cpp
    include/miniaudio/miniaudio.cpp
)

# Create audio library (used by both C++ tests and Python bindings)
add_library(audio_lib STATIC ${AUDIO_SOURCES})
set_target_properties(audio_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(audio_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)
# Logging is always compiled in (controlled at runtime via set_log_level)

# On Windows, link with winmm
if(WIN32)
    target_link_libraries(audio_lib PUBLIC winmm)
endif()

# Build tests and examples only if requested (disabled when used as subdirectory)
if(BUILD_AUDIO_EXAMPLES)
    # Interactive test executable
    add_executable(test_audio
        examples/test_audio_2.cpp
    )
    target_link_libraries(test_audio PRIVATE audio_lib)
    
    # Define the sound files directory path at compile time
    target_compile_definitions(test_audio PRIVATE
        SOUND_FILES_DIR="${CMAKE_SOURCE_DIR}/sound_files/"
    )
endif()

if(BUILD_AUDIO_TESTS)
    # Common test utilities
    add_library(test_common STATIC tests/cpp/test_common.cpp)
    target_include_directories(test_common PUBLIC tests/cpp)
    
    # Test executables - organized by functionality
    set(TEST_EXECUTABLES
        test_audio_initialization
        test_audio_logging
        test_audio_volume
        test_audio_groups
        test_audio_sounds
        test_audio_tracks
        test_audio_validation
        test_audio_threading
        test_audio_resources
    )
    
    foreach(TEST_NAME ${TEST_EXECUTABLES})
        add_executable(${TEST_NAME}
            tests/cpp/${TEST_NAME}.cpp
        )
        # Explicitly set include directories for test targets
        # (audio_lib has PUBLIC includes, but being explicit helps IDEs)
        target_include_directories(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests/cpp
        )
        target_link_libraries(${TEST_NAME} PRIVATE audio_lib test_common)
        target_compile_definitions(${TEST_NAME} PRIVATE
            SOUND_FILES_DIR="${CMAKE_SOURCE_DIR}/sound_files/"
        )
    endforeach()
endif()

# ======================================================
# Python Bindings (optional)
# ======================================================
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)

if(BUILD_PYTHON_BINDINGS)
    # Allow callers to specify PYTHON_EXECUTABLE/Python_EXECUTABLE and map to Python3
    if(DEFINED PYTHON_EXECUTABLE AND NOT Python3_EXECUTABLE)
        set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE FILEPATH "Python3 executable" FORCE)
    endif()
    if(DEFINED Python_EXECUTABLE AND NOT Python3_EXECUTABLE)
        set(Python3_EXECUTABLE ${Python_EXECUTABLE} CACHE FILEPATH "Python3 executable" FORCE)
    endif()

    # Find Python before pybind11 to ensure correct Python is used
    find_package(Python3 COMPONENTS Interpreter Development QUIET)
    
    if(Python3_FOUND)
        message(STATUS "Found Python ${Python3_VERSION}: ${Python3_EXECUTABLE}")
        set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE} CACHE PATH "Python executable" FORCE)
        
        # Fetch pybind11
        include(FetchContent)
        FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v3.0.1
        )
        FetchContent_MakeAvailable(pybind11)
        
        # Python bindings module
        file(GLOB_RECURSE BINDING_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.cpp
        )
        
        pybind11_add_module(game_audio ${BINDING_SOURCES})
        
        target_include_directories(game_audio PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/bindings
        )
        
        target_link_libraries(game_audio PRIVATE audio_lib)
        
        # Set output directory for Python module
        # Note: pybind11 will create platform-specific filenames (e.g., game_audio.cp311-win_amd64.pyd)
        # but Python can still import it as 'game_audio'
        set_target_properties(game_audio PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_NAME game_audio
        )
        
        # ======================================================
        # Generate Python type stubs with pybind11-stubgen
        # ======================================================
        set(STUB_OUTPUT_DIR ${CMAKE_BINARY_DIR}/stubs)
        set(STUB_FILE ${STUB_OUTPUT_DIR}/game_audio.pyi)
        
        # Always try to generate stubs - check availability at build time, not configure time
        # This allows pybind11-stubgen to be installed after CMake configuration
        file(MAKE_DIRECTORY ${STUB_OUTPUT_DIR})
        
        # Create wrapper script for stub generation (handles PYTHONPATH and checks availability)
        file(WRITE ${CMAKE_BINARY_DIR}/run_stubgen.py
"#!/usr/bin/env python3
import os
import sys
import subprocess

# Check if pybind11_stubgen is available
try:
    import pybind11_stubgen
except ImportError:
    print('Warning: pybind11-stubgen not available - skipping stub generation', file=sys.stderr)
    sys.exit(0)

pythonpath = os.environ.get('PYTHONPATH', '')
build_dir = sys.argv[1]
output_dir = sys.argv[2]
os.environ['PYTHONPATH'] = build_dir + os.pathsep + pythonpath if pythonpath else build_dir

try:
    result = subprocess.run(
        [sys.executable, '-m', 'pybind11_stubgen', '--output-dir', output_dir, 'game_audio', '--ignore-all-errors'],
        cwd=build_dir,
        capture_output=False
    )
    sys.exit(0)
except Exception as e:
    print(f'Warning: Stub generation failed (non-fatal): {e}', file=sys.stderr)
    sys.exit(0)
")
        
        # Create custom target to generate stubs (runs after game_audio is built)
        # The script will check if pybind11-stubgen is available at build time
        add_custom_target(generate_stubs ALL
            COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_BINARY_DIR}/run_stubgen.py
                ${CMAKE_BINARY_DIR}
                ${STUB_OUTPUT_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS game_audio
            COMMENT "Generating Python type stubs for game_audio module (if pybind11-stubgen available)"
            BYPRODUCTS ${STUB_FILE}
        )
        
        message(STATUS "Python type stub generation will be attempted (requires pybind11-stubgen at build time)")
        
        # ======================================================
        # Install Python module
        # ======================================================
        # Install the compiled module (.pyd on Windows, .so on Linux/Mac)
        # scikit-build-core sets CMAKE_INSTALL_PREFIX to site-packages automatically
        # The extension will be installed to game_audio/ directory alongside __init__.py
        # Use both LIBRARY and RUNTIME to ensure the extension is installed on all platforms
        install(TARGETS game_audio
            LIBRARY DESTINATION game_audio
            RUNTIME DESTINATION game_audio
            ARCHIVE DESTINATION game_audio
        )
        
        # Install the __init__.py file for the package
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/game_audio/__init__.py
            DESTINATION game_audio
        )
        
        # Install Python type stubs (.pyi files) if they were generated
        # These enable IDE autocomplete/IntelliSense for the compiled module
        # Stubs are generated by the generate_stubs target, then installed here
        # Use OPTIONAL so installation doesn't fail if stubs weren't generated
        install(FILES ${STUB_FILE}
            DESTINATION game_audio
            OPTIONAL
        )
        message(STATUS "Python type stubs will be installed if generated")
        
        message(STATUS "Python bindings enabled")
    else()
        message(WARNING "Python3 not found - Python bindings will not be built")
        message(WARNING "  Set PYTHON_EXECUTABLE or Python3_EXECUTABLE to specify Python location")
    endif()
endif()

# Add a custom target for generating documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
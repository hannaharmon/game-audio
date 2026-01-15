# CMakeLists.txt for Game Audio Module
# 
# This file defines the build configuration for the audio module
# and test application. It's designed to work on Windows, macOS, and Linux.

cmake_minimum_required(VERSION 3.10)
project(AudioModule VERSION 1.0
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

# Source files for audio library (shared between test executables)
set(AUDIO_SOURCES
    src/audio_system.cpp
    src/audio_group.cpp
    src/sound.cpp
    src/audio_track.cpp
    src/audio_manager.cpp
    src/sfx_player.cpp
    src/random_sound_container.cpp
    include/miniaudio/miniaudio.cpp
)

# Create audio library (used by both C++ tests and Python bindings)
add_library(audio_lib STATIC ${AUDIO_SOURCES})
target_include_directories(audio_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

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
    # Automated test executable
    add_executable(test_automated
        tests/test_automated.cpp
    )
    target_link_libraries(test_automated PRIVATE audio_lib)
    
    target_compile_definitions(test_automated PRIVATE
        SOUND_FILES_DIR="${CMAKE_SOURCE_DIR}/sound_files/"
    )
endif()

# ======================================================
# Python Bindings (optional)
# ======================================================
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)

if(BUILD_PYTHON_BINDINGS)
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
        
        pybind11_add_module(audio_py ${BINDING_SOURCES})
        
        target_include_directories(audio_py PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/bindings
        )
        
        target_link_libraries(audio_py PRIVATE audio_lib)
        
        # Set output directory for Python module
        set_target_properties(audio_py PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        # ======================================================
        # Generate Python type stubs with pybind11-stubgen
        # ======================================================
        set(STUB_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/audio_py)
        
        # Check if pybind11-stubgen is available
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -c "import pybind11_stubgen; print('OK')"
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE STUBGEN_AVAILABLE
        )
        
        if(STUBGEN_AVAILABLE EQUAL 0)
            file(MAKE_DIRECTORY ${STUB_OUTPUT_DIR})
            
            # Create wrapper script for stub generation
            file(WRITE ${CMAKE_BINARY_DIR}/run_stubgen.py
"#!/usr/bin/env python3
import os
import sys
import subprocess

pythonpath = os.environ.get('PYTHONPATH', '')
build_dir = sys.argv[1]
output_dir = sys.argv[2]
os.environ['PYTHONPATH'] = build_dir + os.pathsep + pythonpath if pythonpath else build_dir

try:
    result = subprocess.run(
        [sys.executable, '-m', 'pybind11_stubgen', '--output-dir', output_dir, 'audio_py', '--ignore-all-errors'],
        cwd=build_dir,
        capture_output=False
    )
    sys.exit(0)
except Exception as e:
    print(f'Warning: Stub generation failed (non-fatal): {e}', file=sys.stderr)
    sys.exit(0)
")
            
            add_custom_command(TARGET audio_py POST_BUILD
                COMMAND ${Python3_EXECUTABLE}
                    ${CMAKE_BINARY_DIR}/run_stubgen.py
                    ${CMAKE_BINARY_DIR}
                    ${STUB_OUTPUT_DIR}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating Python type stubs for audio_py module (non-fatal)"
            )
            
            message(STATUS "Python type stub generation enabled")
        else()
            message(WARNING "pybind11-stubgen not available - type stubs will not be generated")
            message(WARNING "  Install with: pip install pybind11-stubgen")
        endif()
        
        # ======================================================
        # Install Python module
        # ======================================================
        install(TARGETS audio_py
            LIBRARY DESTINATION audio_py
            RUNTIME DESTINATION audio_py
        )
        
        message(STATUS "Python bindings enabled")
    else()
        message(WARNING "Python3 not found - Python bindings will not be built")
        message(WARNING "  Set PYTHON_EXECUTABLE to specify Python location")
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
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

# Source files for audio library (shared between test executables)
set(AUDIO_SOURCES
    audio/audio_system.cpp
    audio/audio_group.cpp
    audio/sound.cpp
    audio/audio_track.cpp
    audio/audio_manager.cpp
    audio/sfx_player.cpp
    audio/random_sound_container.cpp
    include/miniaudio/miniaudio.cpp
)

# Interactive test executable
add_executable(test_audio
    examples/test_audio_2.cpp
    ${AUDIO_SOURCES}
)

# Automated test executable
add_executable(test_automated
    tests/test_automated.cpp
    ${AUDIO_SOURCES}
)

# Define the sound files directory path at compile time
target_compile_definitions(test_audio PRIVATE
    SOUND_FILES_DIR="${CMAKE_SOURCE_DIR}/sound_files/"
)
target_compile_definitions(test_automated PRIVATE
    SOUND_FILES_DIR="${CMAKE_SOURCE_DIR}/sound_files/"
)

# Include directories
target_include_directories(test_audio PRIVATE
    ${CMAKE_SOURCE_DIR}/audio
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(test_automated PRIVATE
    ${CMAKE_SOURCE_DIR}/audio
    ${CMAKE_SOURCE_DIR}/include
)

# On Windows, we might need to link with these libraries
if(WIN32)
    target_link_libraries(test_audio PRIVATE winmm)
    target_link_libraries(test_automated PRIVATE winmm)
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
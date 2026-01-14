# CMakeLists.txt for Game Audio Module
# 
# This file defines the build configuration for the audio module
# and test application. It's designed to work on Windows, macOS, and Linux.

cmake_minimum_required(VERSION 3.10)
project(AudioModule VERSION 1.0
                    DESCRIPTION "Game Audio Module using miniaudio"
                    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(test_audio
    examples/test_audio_2.cpp
    audio/audio_system.cpp
    audio/audio_group.cpp
    audio/sound.cpp
    audio/audio_track.cpp
    audio/audio_manager.cpp
    audio/music_player.cpp
    audio/sfx_player.cpp
    audio/random_sound_container.cpp
    include/miniaudio/miniaudio.cpp
)

# Include directories
target_include_directories(test_audio PRIVATE
    ${CMAKE_SOURCE_DIR}/audio
    ${CMAKE_SOURCE_DIR}/include
)

# On Windows, we might need to link with these libraries
if(WIN32)
    target_link_libraries(test_audio PRIVATE
        winmm
    )
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
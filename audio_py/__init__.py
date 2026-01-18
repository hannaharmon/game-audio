"""
Game Audio Module - Python Bindings

A Python wrapper for the C++ Game Audio Module built on miniaudio.
Provides high-level audio control for game development.

Example usage:
    >>> import audio_py
    >>> 
    >>> # Initialize the audio system
    >>> audio = audio_py.AudioManager.get_instance()
    >>> audio.initialize()
    >>> 
    >>> # Create audio groups
    >>> music_group = audio.create_group("music")
    >>> sfx_group = audio.create_group("sfx")
    >>> 
    >>> # Set volumes
    >>> audio.set_master_volume(0.8)
    >>> audio.set_group_volume(music_group, 0.7)
    >>> 
    >>> # Use the core primitives to build your own playback helpers
"""

__version__ = "1.0.0"

# Import the C++ extension module
try:
    from .audio_py import *
except ImportError:
    # If the module isn't built yet, provide helpful error message
    import sys
    print("Error: audio_py C++ extension not found.", file=sys.stderr)
    print("Make sure to build the project with: cmake --build build", file=sys.stderr)
    raise

__all__ = [
    # Core classes
    'AudioManager',
    'AudioSession',
    'RandomSoundContainer',
    'RandomSoundContainerConfig',
    
    # Handle types
    'TrackHandle',
    'GroupHandle',
    'SoundHandle',
]

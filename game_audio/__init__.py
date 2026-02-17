"""
Game Audio Module - Python Bindings

A Python wrapper for the C++ Game Audio Module built on miniaudio.
Provides high-level audio control for game development.

Example usage:
    >>> import game_audio
    >>> 
    >>> # Initialize the audio system
    >>> audio = game_audio.AudioManager.get_instance()
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

__version__ = "2.0.0"

# Import the C++ extension module
# The compiled extension is created by pybind11 and installed in this package directory
# Python's import system automatically handles platform-specific suffixes
# (e.g., game_audio.cp311-win_amd64.pyd on Windows, game_audio.cp310-x86_64-linux-gnu.so on Linux)
try:
    # Python's import system will automatically find the extension with platform-specific suffix
    # We use a relative import to avoid circular import issues
    from . import game_audio as _game_audio_ext
    # Import all public symbols from the extension module into this package's namespace
    from .game_audio import *
except ImportError as e:
    # Provide helpful error message with debugging info
    import sys
    import os
    package_dir = os.path.dirname(os.path.abspath(__file__))
    print("Error: game_audio C++ extension not found.", file=sys.stderr)
    print(f"Package directory: {package_dir}", file=sys.stderr)
    if os.path.exists(package_dir):
        files = os.listdir(package_dir)
        print(f"Files in package directory: {files}", file=sys.stderr)
        # Check for extension files
        ext_files = [f for f in files if f.startswith('game_audio.') and (f.endswith('.pyd') or f.endswith('.so'))]
        if ext_files:
            print(f"Found extension files: {ext_files}", file=sys.stderr)
        else:
            print("No extension files (.pyd/.so) found in package directory", file=sys.stderr)
    print(f"Import error: {e}", file=sys.stderr)
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

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
    # Import the compiled extension module
    # Python's import system automatically handles platform-specific suffixes
    # The extension module is created by pybind11 and named 'game_audio'
    # It should be in the same directory as this __init__.py
    import sys
    import os
    import importlib.util
    
    # Get the directory containing this __init__.py
    package_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Find and load the extension file directly (it has platform-specific suffix)
    extension_file = None
    if os.path.exists(package_dir):
        for file in os.listdir(package_dir):
            # Look for the extension file with platform-specific suffix
            if file.startswith('game_audio.') and (file.endswith('.pyd') or file.endswith('.so') or file.endswith('.dylib')):
                extension_file = os.path.join(package_dir, file)
                break
    
    if extension_file and os.path.exists(extension_file):
        # Load the extension file directly using importlib
        spec = importlib.util.spec_from_file_location('game_audio._extension', extension_file)
        if spec and spec.loader:
            ext_module = importlib.util.module_from_spec(spec)
            # Store in sys.modules to avoid re-loading and prevent circular imports
            sys.modules['game_audio._extension'] = ext_module
            spec.loader.exec_module(ext_module)
        else:
            raise ImportError(f"Could not load extension from {extension_file}")
    else:
        # Extension file not found - try standard import as fallback
        # This will work if Python's import system can find it automatically
        import importlib
        ext_module = importlib.import_module('game_audio.game_audio')
    
    # Import all public symbols from the extension module into this package's namespace
    for name in dir(ext_module):
        if not name.startswith('_'):
            globals()[name] = getattr(ext_module, name)
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

"""
Common test utilities for Python audio tests
"""

import sys
import os

# Setup paths
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
build_release = os.path.join(repo_root, 'build', 'Release')
build_debug = os.path.join(repo_root, 'build', 'Debug')
build_root = os.path.join(repo_root, 'build')

for path in (build_release, build_debug, build_root):
    if os.path.exists(path) and path not in sys.path:
        sys.path.insert(0, path)

import game_audio
from datetime import timedelta
import time

SOUND_DIR = os.path.join(repo_root, 'sound_files')

def wait_ms(ms):
    """Helper to wait for audio operations"""
    time.sleep(ms / 1000.0)

def get_sound_path(filename):
    """Get full path to a sound file"""
    return os.path.join(SOUND_DIR, filename)

def sound_exists(filename):
    """Check if a sound file exists"""
    return os.path.exists(get_sound_path(filename))

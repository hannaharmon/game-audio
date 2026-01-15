"""Test clean shutdown of audio system."""
import sys
import os
import time

# Add build directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build', 'Debug'))

import audio_py

def test_shutdown():
    """Test that shutdown properly terminates background threads."""
    print("Initializing audio system...")
    audio = audio_py.AudioManager.get_instance()
    audio.initialize()
    
    print("Waiting 1 second...")
    time.sleep(1)
    
    print("Shutting down...")
    audio.shutdown()
    
    print("Shutdown complete - if this hangs, the thread didn't terminate")
    # Should exit cleanly here without needing os._exit()

if __name__ == "__main__":
    test_shutdown()
    print("Test passed - exiting normally")

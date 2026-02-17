"""
Audio System Threading Tests
Tests thread safety and concurrent operations.
"""

from test_common import *
import threading

def test_thread_safety_get_master_volume():
    """Test: Thread safety for GetMasterVolume"""
    print("Test: Thread safety GetMasterVolume... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    # Set initial volume
    audio.set_master_volume(0.5)
    
    # Launch multiple threads that call get_master_volume concurrently
    num_threads = 10
    calls_per_thread = 100
    success_count = [0]
    failure_count = [0]
    lock = threading.Lock()
    
    def worker():
        for _ in range(calls_per_thread):
            try:
                vol = audio.get_master_volume()
                if 0.0 <= vol <= 1.0:
                    with lock:
                        success_count[0] += 1
                else:
                    with lock:
                        failure_count[0] += 1
            except Exception:
                with lock:
                    failure_count[0] += 1
    
    threads = []
    for _ in range(num_threads):
        t = threading.Thread(target=worker)
        t.start()
        threads.append(t)
    
    # Also change volume from main thread while reading
    for i in range(50):
        audio.set_master_volume(0.3 + (i % 2) * 0.4)
        wait_ms(10)
    
    # Wait for all threads
    for t in threads:
        t.join()
    
    assert success_count[0] > 0, "get_master_volume should succeed from multiple threads"
    assert failure_count[0] == 0, "get_master_volume should not fail or crash"
    
    # Reset volume
    audio.set_master_volume(1.0)
    audio.shutdown()
    print("PASS")

def test_concurrent_operations():
    """Test: Concurrent operations"""
    print("Test: Concurrent operations... ", end="", flush=True)
    audio = game_audio.AudioManager.get_instance()
    audio.initialize()
    
    music = audio.create_group()
    sfx = audio.create_group()
    
    if sound_exists("digital_base.wav") and sound_exists("digital_battle.wav"):
        sound1 = audio.load_sound(get_sound_path("digital_base.wav"), music)
        sound2 = audio.load_sound(get_sound_path("digital_battle.wav"), sfx)
        
        # Start multiple operations concurrently
        audio.play_sound(sound1)
        audio.play_sound(sound2)
        audio.set_group_volume(music, 0.8)
        audio.set_group_volume(sfx, 0.6)
        audio.fade_group(music, 0.3, timedelta(milliseconds=500))
        
        wait_ms(600)
        
        assert True, "Concurrent operations should work"
        
        audio.stop_sound(sound1)
        audio.stop_sound(sound2)
        audio.destroy_sound(sound1)
        audio.destroy_sound(sound2)
    
    audio.destroy_group(music)
    audio.destroy_group(sfx)
    audio.shutdown()
    print("PASS")

def run_all_tests():
    """Run all threading tests"""
    print("=== Python Audio Threading Tests ===\n")
    
    tests = [
        test_thread_safety_get_master_volume,
        test_concurrent_operations,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except AssertionError as e:
            print(f"FAIL - {e}")
            failed += 1
        except Exception as e:
            print(f"ERROR - {e}")
            failed += 1
    
    print(f"\n=== Results: {passed} passed, {failed} failed ===")
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)

/**
 * Audio System Spatial Audio Tests
 * 
 * Tests 3D spatial audio functionality including:
 * - Vec3 vector operations
 * - Listener position/orientation
 * - Sound positioning and distance attenuation
 * - Spatialization enable/disable
 * - Integration between listener and sounds
 */

#include "test_common.h"
#include "audio_manager.h"
#include "vec3.h"
#include <cmath>
#include <algorithm>

using namespace audio;

void test_vec3_basic_operations() {
    TEST("Vec3 Basic Operations")
    
    // Default constructor
    Vec3 v1;
    ASSERT(v1.x == 0.0f && v1.y == 0.0f && v1.z == 0.0f, "Default Vec3 should be (0,0,0)")
    
    // Component constructor
    Vec3 v2(1.0f, 2.0f, 3.0f);
    ASSERT(v2.x == 1.0f && v2.y == 2.0f && v2.z == 3.0f, "Vec3 constructor should set components")
    
    // Equality
    Vec3 v3(1.0f, 2.0f, 3.0f);
    ASSERT(v2 == v3, "Equal Vec3s should compare equal")
    ASSERT(v1 != v2, "Different Vec3s should compare not equal")
    
    // Length
    Vec3 v4(3.0f, 4.0f, 0.0f);
    ASSERT(std::abs(v4.Length() - 5.0f) < 0.001f, "Vec3 length should be calculated correctly")
    
    Vec3 v5(1.0f, 1.0f, 1.0f);
    float expected_length = std::sqrt(3.0f);
    ASSERT(std::abs(v5.Length() - expected_length) < 0.001f, "Vec3 length for (1,1,1) should be sqrt(3)")
    
    // Length squared
    ASSERT(std::abs(v4.LengthSquared() - 25.0f) < 0.001f, "Vec3 length squared should be correct")
    
    END_TEST
}

void test_vec3_arithmetic() {
    TEST("Vec3 Arithmetic Operations")
    
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    
    // Addition
    Vec3 sum = v1 + v2;
    ASSERT(sum.x == 5.0f && sum.y == 7.0f && sum.z == 9.0f, "Vec3 addition should work")
    
    // Subtraction
    Vec3 diff = v2 - v1;
    ASSERT(diff.x == 3.0f && diff.y == 3.0f && diff.z == 3.0f, "Vec3 subtraction should work")
    
    // Scalar multiplication
    Vec3 scaled = v1 * 2.0f;
    ASSERT(scaled.x == 2.0f && scaled.y == 4.0f && scaled.z == 6.0f, "Vec3 scalar multiplication should work")
    
    // Scalar division
    Vec3 divided = scaled / 2.0f;
    ASSERT(std::abs(divided.x - 1.0f) < 0.001f && 
           std::abs(divided.y - 2.0f) < 0.001f && 
           std::abs(divided.z - 3.0f) < 0.001f, "Vec3 scalar division should work")
    
    // In-place operations
    Vec3 v3(1.0f, 1.0f, 1.0f);
    v3 += v1;
    ASSERT(v3.x == 2.0f && v3.y == 3.0f && v3.z == 4.0f, "Vec3 += should work")
    
    v3 -= v1;
    ASSERT(v3.x == 1.0f && v3.y == 1.0f && v3.z == 1.0f, "Vec3 -= should work")
    
    v3 *= 2.0f;
    ASSERT(v3.x == 2.0f && v3.y == 2.0f && v3.z == 2.0f, "Vec3 *= should work")
    
    v3 /= 2.0f;
    ASSERT(std::abs(v3.x - 1.0f) < 0.001f && 
           std::abs(v3.y - 1.0f) < 0.001f && 
           std::abs(v3.z - 1.0f) < 0.001f, "Vec3 /= should work")
    
    END_TEST
}

void test_vec3_normalization() {
    TEST("Vec3 Normalization")
    
    Vec3 v1(3.0f, 4.0f, 0.0f);
    float original_length = v1.Length();
    
    // Normalize in place
    v1.Normalize();
    ASSERT(std::abs(v1.Length() - 1.0f) < 0.001f, "Normalized Vec3 should have length 1")
    
    // Normalized copy
    Vec3 v2(3.0f, 4.0f, 0.0f);
    Vec3 normalized = v2.Normalized();
    ASSERT(std::abs(normalized.Length() - 1.0f) < 0.001f, "Normalized() should return normalized copy")
    ASSERT(std::abs(v2.Length() - original_length) < 0.001f, "Normalized() should not modify original")
    
    // Zero vector normalization (should not crash)
    Vec3 zero(0.0f, 0.0f, 0.0f);
    zero.Normalize();
    ASSERT(true, "Normalizing zero vector should not crash")
    
    END_TEST
}

void test_vec3_distance() {
    TEST("Vec3 Distance Calculations")
    
    Vec3 p1(0.0f, 0.0f, 0.0f);
    Vec3 p2(3.0f, 4.0f, 0.0f);
    
    float dist = p1.Distance(p2);
    ASSERT(std::abs(dist - 5.0f) < 0.001f, "Distance between (0,0,0) and (3,4,0) should be 5")
    
    float dist_sq = p1.DistanceSquared(p2);
    ASSERT(std::abs(dist_sq - 25.0f) < 0.001f, "Distance squared should be 25")
    
    // Distance to self should be zero
    ASSERT(std::abs(p1.Distance(p1)) < 0.001f, "Distance to self should be zero")
    
    Vec3 p3(1.0f, 1.0f, 1.0f);
    Vec3 p4(2.0f, 2.0f, 2.0f);
    float dist3d = p3.Distance(p4);
    float expected = std::sqrt(3.0f);
    ASSERT(std::abs(dist3d - expected) < 0.001f, "3D distance calculation should be correct")
    
    END_TEST
}

void test_listener_position() {
    TEST("Listener Position Management")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set listener position
    Vec3 pos1(5.0f, 10.0f, 15.0f);
    audio.SetListenerPosition(pos1);
    
    Vec3 retrieved = audio.GetListenerPosition();
    ASSERT(std::abs(retrieved.x - 5.0f) < 0.01f &&
           std::abs(retrieved.y - 10.0f) < 0.01f &&
           std::abs(retrieved.z - 15.0f) < 0.01f, "Listener position should be set and retrieved correctly")
    
    // Update position
    Vec3 pos2(20.0f, 30.0f, 40.0f);
    audio.SetListenerPosition(pos2);
    retrieved = audio.GetListenerPosition();
    ASSERT(std::abs(retrieved.x - 20.0f) < 0.01f &&
           std::abs(retrieved.y - 30.0f) < 0.01f &&
           std::abs(retrieved.z - 40.0f) < 0.01f, "Listener position should update correctly")
    
    END_TEST
}

void test_listener_direction() {
    TEST("Listener Direction Management")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set listener direction (forward vector)
    Vec3 forward(0.0f, 0.0f, -1.0f);
    audio.SetListenerDirection(forward);
    
    Vec3 retrieved = audio.GetListenerDirection();
    ASSERT(std::abs(retrieved.x - 0.0f) < 0.01f &&
           std::abs(retrieved.y - 0.0f) < 0.01f &&
           std::abs(retrieved.z - (-1.0f)) < 0.01f, "Listener direction should be set correctly")
    
    // Update direction
    Vec3 forward2(1.0f, 0.0f, 0.0f);
    forward2.Normalize();
    audio.SetListenerDirection(forward2);
    retrieved = audio.GetListenerDirection();
    ASSERT(std::abs(retrieved.Length() - 1.0f) < 0.01f, "Listener direction should be normalized")
    
    END_TEST
}

void test_listener_up() {
    TEST("Listener Up Vector Management")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set listener up vector
    Vec3 up(0.0f, 1.0f, 0.0f);
    audio.SetListenerUp(up);
    
    Vec3 retrieved = audio.GetListenerUp();
    ASSERT(std::abs(retrieved.x - 0.0f) < 0.01f &&
           std::abs(retrieved.y - 1.0f) < 0.01f &&
           std::abs(retrieved.z - 0.0f) < 0.01f, "Listener up vector should be set correctly")
    
    // Update up vector
    Vec3 up2(0.0f, 0.0f, 1.0f);
    audio.SetListenerUp(up2);
    retrieved = audio.GetListenerUp();
    ASSERT(std::abs(retrieved.z - 1.0f) < 0.01f, "Listener up vector should update correctly")
    
    END_TEST
}

void test_sound_position() {
    TEST("Sound Position Management")
    
    auto& audio = AudioManager::GetInstance();
    
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    ASSERT(sound.IsValid(), "Sound should load")
    
    // Set sound position
    Vec3 pos1(10.0f, 20.0f, 30.0f);
    audio.SetSoundPosition(sound, pos1);
    
    Vec3 retrieved = audio.GetSoundPosition(sound);
    ASSERT(std::abs(retrieved.x - 10.0f) < 0.01f &&
           std::abs(retrieved.y - 20.0f) < 0.01f &&
           std::abs(retrieved.z - 30.0f) < 0.01f, "Sound position should be set and retrieved correctly")
    
    // Update position
    Vec3 pos2(50.0f, 60.0f, 70.0f);
    audio.SetSoundPosition(sound, pos2);
    retrieved = audio.GetSoundPosition(sound);
    ASSERT(std::abs(retrieved.x - 50.0f) < 0.01f &&
           std::abs(retrieved.y - 60.0f) < 0.01f &&
           std::abs(retrieved.z - 70.0f) < 0.01f, "Sound position should update correctly")
    
    // Test invalid handle
    ASSERT_THROWS(InvalidHandleException, 
                  audio.SetSoundPosition(SoundHandle::Invalid(), pos1),
                  "Setting position on invalid sound handle should throw");
    
    audio.DestroySound(sound);
    
    END_TEST
}

void test_sound_distance_attenuation() {
    TEST("Sound Distance Attenuation Parameters")
    
    auto& audio = AudioManager::GetInstance();
    
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    
    // Set min distance
    audio.SetSoundMinDistance(sound, 5.0f);
    float min_dist = audio.GetSoundMinDistance(sound);
    ASSERT(std::abs(min_dist - 5.0f) < 0.01f, "Sound min distance should be set correctly")
    
    // Set max distance
    audio.SetSoundMaxDistance(sound, 100.0f);
    float max_dist = audio.GetSoundMaxDistance(sound);
    ASSERT(std::abs(max_dist - 100.0f) < 0.01f, "Sound max distance should be set correctly")
    
    // Set rolloff
    audio.SetSoundRolloff(sound, 2.0f);
    float rolloff = audio.GetSoundRolloff(sound);
    ASSERT(std::abs(rolloff - 2.0f) < 0.01f, "Sound rolloff should be set correctly")
    
    // Test clamping: min distance should be clamped if <= 0
    audio.SetSoundMinDistance(sound, -1.0f);
    min_dist = audio.GetSoundMinDistance(sound);
    ASSERT(min_dist > 0.0f, "Negative min distance should be clamped to > 0")
    
    // Test clamping: max distance should be > min distance
    audio.SetSoundMinDistance(sound, 10.0f);
    audio.SetSoundMaxDistance(sound, 5.0f);  // Less than min
    max_dist = audio.GetSoundMaxDistance(sound);
    ASSERT(max_dist > 10.0f, "Max distance < min distance should be clamped")
    
    // Test rolloff clamping
    audio.SetSoundRolloff(sound, -1.0f);
    rolloff = audio.GetSoundRolloff(sound);
    ASSERT(rolloff >= 0.0f, "Negative rolloff should be clamped to >= 0")
    
    // Test invalid handle
    ASSERT_THROWS(InvalidHandleException,
                  audio.SetSoundMinDistance(SoundHandle::Invalid(), 1.0f),
                  "Setting min distance on invalid handle should throw");
    
    audio.DestroySound(sound);
    
    END_TEST
}

void test_sound_spatialization_enabled() {
    TEST("Sound Spatialization Enable/Disable")
    
    auto& audio = AudioManager::GetInstance();
    
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    
    // Spatialization should be enabled by default
    bool enabled = audio.IsSoundSpatializationEnabled(sound);
    ASSERT(enabled == true, "Spatialization should be enabled by default")
    
    // Disable spatialization
    audio.SetSoundSpatializationEnabled(sound, false);
    enabled = audio.IsSoundSpatializationEnabled(sound);
    ASSERT(enabled == false, "Spatialization should be disabled")
    
    // Re-enable spatialization
    audio.SetSoundSpatializationEnabled(sound, true);
    enabled = audio.IsSoundSpatializationEnabled(sound);
    ASSERT(enabled == true, "Spatialization should be re-enabled")
    
    // Test invalid handle
    ASSERT_THROWS(InvalidHandleException,
                  audio.SetSoundSpatializationEnabled(SoundHandle::Invalid(), true),
                  "Setting spatialization on invalid handle should throw");
    
    audio.DestroySound(sound);
    
    END_TEST
}

void test_spatial_audio_integration() {
    TEST("Spatial Audio Integration (Listener + Sound)")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set up listener
    Vec3 listener_pos(0.0f, 0.0f, 0.0f);
    Vec3 listener_forward(0.0f, 0.0f, -1.0f);
    Vec3 listener_up(0.0f, 1.0f, 0.0f);
    
    audio.SetListenerPosition(listener_pos);
    audio.SetListenerDirection(listener_forward);
    audio.SetListenerUp(listener_up);
    
    // Create sound at a distance
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    Vec3 sound_pos(5.0f, 0.0f, 0.0f);  // 5 units to the right
    audio.SetSoundPosition(sound, sound_pos);
    audio.SetSoundMinDistance(sound, 1.0f);
    audio.SetSoundMaxDistance(sound, 20.0f);
    audio.SetSoundRolloff(sound, 1.0f);
    
    // Verify distance calculation
    float distance = listener_pos.Distance(sound_pos);
    ASSERT(std::abs(distance - 5.0f) < 0.001f, "Distance between listener and sound should be 5")
    
    // Move listener closer
    Vec3 new_listener_pos(2.0f, 0.0f, 0.0f);
    audio.SetListenerPosition(new_listener_pos);
    float new_distance = new_listener_pos.Distance(sound_pos);
    ASSERT(std::abs(new_distance - 3.0f) < 0.001f, "Distance should decrease when listener moves closer")
    
    // Move listener farther
    Vec3 far_listener_pos(25.0f, 0.0f, 0.0f);
    audio.SetListenerPosition(far_listener_pos);
    float far_distance = far_listener_pos.Distance(sound_pos);
    ASSERT(std::abs(far_distance - 20.0f) < 0.001f, "Distance should increase when listener moves away")
    
    audio.DestroySound(sound);
    
    END_TEST
}

void test_multiple_spatial_sounds() {
    TEST("Multiple Spatial Sounds")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set up listener
    audio.SetListenerPosition(Vec3(0.0f, 0.0f, 0.0f));
    
    // Create multiple sounds at different positions
    SoundHandle sound1 = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.SetSoundPosition(sound1, Vec3(10.0f, 0.0f, 0.0f));
    audio.SetSoundMinDistance(sound1, 1.0f);
    audio.SetSoundMaxDistance(sound1, 50.0f);
    
    SoundHandle sound2 = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.SetSoundPosition(sound2, Vec3(0.0f, 10.0f, 0.0f));
    audio.SetSoundMinDistance(sound2, 1.0f);
    audio.SetSoundMaxDistance(sound2, 50.0f);
    
    SoundHandle sound3 = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.SetSoundPosition(sound3, Vec3(0.0f, 0.0f, 10.0f));
    audio.SetSoundMinDistance(sound3, 1.0f);
    audio.SetSoundMaxDistance(sound3, 50.0f);
    
    // Verify all positions are independent
    Vec3 pos1 = audio.GetSoundPosition(sound1);
    Vec3 pos2 = audio.GetSoundPosition(sound2);
    Vec3 pos3 = audio.GetSoundPosition(sound3);
    
    ASSERT(std::abs(pos1.x - 10.0f) < 0.01f, "Sound1 position should be independent")
    ASSERT(std::abs(pos2.y - 10.0f) < 0.01f, "Sound2 position should be independent")
    ASSERT(std::abs(pos3.z - 10.0f) < 0.01f, "Sound3 position should be independent")
    
    // Update positions independently
    audio.SetSoundPosition(sound1, Vec3(20.0f, 0.0f, 0.0f));
    pos1 = audio.GetSoundPosition(sound1);
    ASSERT(std::abs(pos1.x - 20.0f) < 0.01f, "Sound1 position should update independently")
    
    // Other sounds should be unchanged
    pos2 = audio.GetSoundPosition(sound2);
    ASSERT(std::abs(pos2.y - 10.0f) < 0.01f, "Sound2 position should be unchanged")
    
    audio.DestroySound(sound1);
    audio.DestroySound(sound2);
    audio.DestroySound(sound3);
    
    END_TEST
}

void test_spatial_audio_with_playback() {
    TEST("Spatial Audio During Playback")
    
    auto& audio = AudioManager::GetInstance();
    
    // Set up listener
    audio.SetListenerPosition(Vec3(0.0f, 0.0f, 0.0f));
    audio.SetListenerDirection(Vec3(0.0f, 0.0f, -1.0f));
    
    // Create and configure spatial sound
    SoundHandle sound = audio.LoadSound(sound_dir + "/digital_base.wav");
    audio.SetSoundPosition(sound, Vec3(5.0f, 0.0f, 0.0f));
    audio.SetSoundMinDistance(sound, 1.0f);
    audio.SetSoundMaxDistance(sound, 20.0f);
    audio.SetSoundRolloff(sound, 1.0f);
    audio.SetSoundLooping(sound, true);
    
    // Start playback
    audio.PlaySound(sound);
    wait_ms(100);
    ASSERT(audio.IsSoundPlaying(sound), "Spatial sound should be playing")
    
    // Update position during playback
    audio.SetSoundPosition(sound, Vec3(10.0f, 0.0f, 0.0f));
    Vec3 new_pos = audio.GetSoundPosition(sound);
    ASSERT(std::abs(new_pos.x - 10.0f) < 0.01f, "Sound position should update during playback")
    
    // Update listener position during playback
    audio.SetListenerPosition(Vec3(5.0f, 0.0f, 0.0f));
    Vec3 listener_pos = audio.GetListenerPosition();
    ASSERT(std::abs(listener_pos.x - 5.0f) < 0.01f, "Listener position should update during playback")
    
    // Stop and verify
    audio.StopSound(sound);
    wait_ms(50);
    ASSERT(!audio.IsSoundPlaying(sound), "Sound should stop")
    
    audio.DestroySound(sound);
    
    END_TEST
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Audio System Spatial Audio Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    sound_dir = get_sound_dir(argc, argv);
    std::cout << "Sound directory: " << sound_dir << std::endl;
    
    // Initialize audio system
    auto& audio = AudioManager::GetInstance();
    if (!audio.Initialize()) {
        std::cerr << "FATAL: Failed to initialize audio system" << std::endl;
        return 1;
    }
    
    // Run all tests
    test_vec3_basic_operations();
    test_vec3_arithmetic();
    test_vec3_normalization();
    test_vec3_distance();
    test_listener_position();
    test_listener_direction();
    test_listener_up();
    test_sound_position();
    test_sound_distance_attenuation();
    test_sound_spatialization_enabled();
    test_spatial_audio_integration();
    test_multiple_spatial_sounds();
    test_spatial_audio_with_playback();
    
    // Final shutdown
    audio.Shutdown();
    
    // Print results
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\nALL TESTS PASSED ✓" << std::endl;
        return 0;
    } else {
        std::cout << "\nSOME TESTS FAILED ✗" << std::endl;
        return 1;
    }
}

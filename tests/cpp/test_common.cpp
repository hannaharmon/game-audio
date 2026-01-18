/**
 * Common test utilities implementation
 */

#include "test_common.h"

// Test result tracking (shared across test files)
int tests_passed = 0;
int tests_failed = 0;

// Path configuration (shared across test files)
std::string sound_dir;

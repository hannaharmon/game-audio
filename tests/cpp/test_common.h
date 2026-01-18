/**
 * Common test utilities and macros for audio system tests
 */

#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <stdexcept>

// Test result tracking (shared across test files)
extern int tests_passed;
extern int tests_failed;

// Path configuration (shared across test files)
extern std::string sound_dir;

#define TEST(name) \
    std::cout << "\nTEST: " << name << std::endl; \
    try {

#define ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "  FAIL: " << message << std::endl; \
        tests_failed++; \
        throw std::runtime_error(message); \
    } \
    tests_passed++; \
    std::cout << "  PASS: " << message << std::endl;

#define ASSERT_THROWS(exception_type, code, message) \
    do { \
        bool threw_correct_exception = false; \
        try { \
            code; \
        } catch (const exception_type& e) { \
            threw_correct_exception = true; \
            std::cout << "  PASS: " << message << " (caught: " << e.what() << ")" << std::endl; \
            tests_passed++; \
        } catch (const std::exception& e) { \
            std::cerr << "  FAIL: " << message << " - Wrong exception type: " << e.what() << std::endl; \
            tests_failed++; \
        } \
        if (!threw_correct_exception) { \
            std::cerr << "  FAIL: " << message << " - No exception thrown" << std::endl; \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_NO_THROW(code, message) \
    do { \
        try { \
            code; \
            std::cout << "  PASS: " << message << std::endl; \
            tests_passed++; \
        } catch (const std::exception& e) { \
            std::cerr << "  FAIL: " << message << " - Unexpected exception: " << e.what() << std::endl; \
            tests_failed++; \
        } \
    } while(0)

#define END_TEST \
    } catch (const std::exception& e) { \
        std::cerr << "  Exception: " << e.what() << std::endl; \
    }

// Helper to wait for audio operations
inline void wait_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Helper to get sound directory path
inline std::string get_sound_dir(int argc, char* argv[]) {
    if (argc > 1) {
        return std::string(argv[1]);
    } else {
#ifdef SOUND_FILES_DIR
        return SOUND_FILES_DIR;
#else
        return "../sound_files";
#endif
    }
}

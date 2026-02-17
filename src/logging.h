#pragma once

/**
 * @defgroup Logging Logging and Diagnostics
 * @brief Runtime logging utilities for the audio module.
 */

#include <string>
#include <sstream>

namespace audio {

/**
 * @brief Log severity levels for audio diagnostics.
 *
 * Logging defaults to Off at runtime. Use set_log_level() to enable.
 */
enum class LogLevel {
    Off = 0,   ///< Disable all logging
    Error = 1, ///< Fatal failures and unrecoverable errors
    Warn = 2,  ///< Recoverable or unusual conditions
    Info = 3,  ///< High-level lifecycle and state transitions
    Debug = 4, ///< Detailed diagnostics (noisy)
};

/**
 * @brief Global logger for the audio module (runtime level control).
 *
 * @ingroup Logging
 */
class Logger {
public:
    /**
     * @brief Set the global log level.
     */
    static void SetLevel(LogLevel level);

    /**
     * @brief Get the current log level.
     */
    static LogLevel GetLevel();

    /**
     * @brief Check if a log level is enabled.
     */
    static bool IsEnabled(LogLevel level);

    /**
     * @brief Emit a log message at the specified level.
     */
    static void Log(LogLevel level, const std::string& message);
};

/**
 * @brief Logging macro for stream-style messages.
 *
 * Logging is always compiled in but defaults to Off at runtime.
 * Use set_log_level() to enable logging output.
 */
#define AUDIO_LOG(level, stream_expr)                         \
    do {                                                       \
        if (audio::Logger::IsEnabled(level)) {                 \
            std::ostringstream _audio_log_stream;              \
            _audio_log_stream << stream_expr;                  \
            audio::Logger::Log(level, _audio_log_stream.str());\
        }                                                      \
    } while (0)

} // namespace audio

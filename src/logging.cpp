#include "logging.h"
#include <atomic>
#include <iostream>

namespace audio {

namespace {
    std::atomic<LogLevel> g_level{LogLevel::Off};
}

void Logger::SetLevel(LogLevel level) {
    g_level.store(level);
}

LogLevel Logger::GetLevel() {
    return g_level.load();
}

bool Logger::IsEnabled(LogLevel level) {
    return g_level.load() >= level && level != LogLevel::Off;
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (!IsEnabled(level)) {
        return;
    }
    if (level == LogLevel::Error || level == LogLevel::Warn) {
        std::cerr << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

} // namespace audio

#include "path_utils.h"
#include <mutex>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define MAX_PATH_LEN MAX_PATH
#define getcwd_func _getcwd
#else
#include <unistd.h>
#define MAX_PATH_LEN 4096
#define getcwd_func getcwd
#endif

namespace audio {

// Thread-safe storage for Python working directory
static std::mutex python_working_directory_mutex;
static std::string python_working_directory;

void SetPythonWorkingDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(python_working_directory_mutex);
    python_working_directory = path;
}

std::string GetPythonWorkingDirectory() {
    std::lock_guard<std::mutex> lock(python_working_directory_mutex);
    return python_working_directory;
}

std::filesystem::path GetCurrentWorkingDirectory() {
    // If Python working directory was set, use it
    {
        std::lock_guard<std::mutex> lock(python_working_directory_mutex);
        if (!python_working_directory.empty()) {
            return std::filesystem::path(python_working_directory);
        }
    }
    
    // Otherwise, try getcwd() / _getcwd()
    char buf[MAX_PATH_LEN];
    if (getcwd_func(buf, sizeof(buf)) != nullptr) {
        return std::filesystem::path(buf);
    }
    
    // Final fallback
    return std::filesystem::current_path();
}

std::string ResolvePath(const std::string& relativePath) {
    std::filesystem::path path(relativePath);
    
    // If path is already absolute, return as-is
    if (path.is_absolute()) {
        return path.string();
    }
    
    // Otherwise, resolve against working directory
    std::filesystem::path workingDir = GetCurrentWorkingDirectory();
    std::filesystem::path resolved = workingDir / path;
    
    // Normalize the path (resolve .. and . components)
    resolved = resolved.lexically_normal();
    
    return resolved.string();
}

}  // namespace audio

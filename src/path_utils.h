#pragma once

#include <string>
#include <filesystem>

/**
 * @file path_utils.h
 * @brief Utility functions for resolving file paths relative to a working directory
 * 
 * This module provides path resolution functionality that works correctly
 * on Linux, Mac, and Windows when Python scripts are run from different directories.
 * It maintains a Python working directory that can be set from Python bindings,
 * and resolves relative paths against this directory.
 */

namespace audio {

/**
 * @brief Set the Python working directory
 * 
 * This should be called from Python bindings to set the directory that
 * relative paths should be resolved against. Typically this would be set
 * to the directory containing the Python script.
 * 
 * @param path Path to set as the working directory
 */
void SetPythonWorkingDirectory(const std::string& path);

/**
 * @brief Get the current Python working directory
 * 
 * @return std::string The current working directory, or empty if not set
 */
std::string GetPythonWorkingDirectory();

/**
 * @brief Get the current working directory
 * 
 * Uses Python's working directory if set, otherwise falls back to getcwd().
 * This is the function that should be used to resolve relative paths.
 * 
 * @return std::filesystem::path The current working directory
 */
std::filesystem::path GetCurrentWorkingDirectory();

/**
 * @brief Resolve a relative path against the working directory
 * 
 * If the path is already absolute, it is returned as-is.
 * If the path is relative, it is resolved against the current working directory
 * (which uses Python working directory if set, otherwise getcwd()).
 * 
 * @param relativePath The path to resolve (can be absolute or relative)
 * @return std::string The resolved absolute path
 */
std::string ResolvePath(const std::string& relativePath);

}  // namespace audio

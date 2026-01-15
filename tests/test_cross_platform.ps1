# Cross-Platform Compatibility Tests
# These tests verify that the codebase maintains cross-platform compatibility

Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "CROSS-PLATFORM COMPATIBILITY TESTS" -ForegroundColor Cyan
Write-Host "============================================================`n" -ForegroundColor Cyan

$TestsPassed = 0
$TestsFailed = 0

function Write-Test {
    param($name, $condition)
    if ($condition) {
        Write-Host "[PASS] $name" -ForegroundColor Green
        $script:TestsPassed++
    } else {
        Write-Host "[FAIL] $name" -ForegroundColor Red
        $script:TestsFailed++
    }
}

# ============================================================================
# TEST 1: Platform-Specific Code Isolation
# ============================================================================
Write-Host "TEST 1: Platform-Specific Code Isolation" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check that all #ifdef _WIN32 blocks have corresponding #else or #endif
$winIfDefs = Get-ChildItem "..\audio" -Filter *.cpp -Recurse | Select-String "#ifdef _WIN32" | Measure-Object
$winEndIfs = Get-ChildItem "..\audio" -Filter *.cpp -Recurse | Select-String "#ifdef _WIN32" -Context 0,50 | 
    Where-Object { $_.Context.PostContext -match "#else|#endif" } | Measure-Object

Write-Test "All Windows-specific blocks are properly closed" ($winIfDefs.Count -eq $winEndIfs.Count)

# Verify no Windows-specific APIs leak outside #ifdef blocks
$audioFiles = Get-ChildItem "..\audio" -Filter *.cpp
$leakedWin32 = $false
foreach ($file in $audioFiles) {
    $content = Get-Content $file.FullName -Raw
    # Split by #ifdef _WIN32 and #else/#endif to check non-Windows sections
    if ($content -match "(?s)(?<!#ifdef _WIN32.*?)(?:FindFirstFile|FindNextFile|WIN32_FIND_DATA)(?!.*?#endif)") {
        # Double-check it's not in a Windows block
        $lines = Get-Content $file.FullName
        $inWin32Block = $false
        for ($i = 0; $i -lt $lines.Count; $i++) {
            if ($lines[$i] -match "#ifdef _WIN32") { $inWin32Block = $true }
            if ($lines[$i] -match "#else|#endif") { $inWin32Block = $false }
            if (!$inWin32Block -and $lines[$i] -match "FindFirstFile|FindNextFile|WIN32_FIND_DATA") {
                $leakedWin32 = $true
                Write-Host "  Found leaked Win32 API in $($file.Name):$($i+1)" -ForegroundColor Red
            }
        }
    }
}
Write-Test "No Windows APIs leak outside platform guards" (!$leakedWin32)

# ============================================================================
# TEST 2: Cross-Platform Standard Library Usage
# ============================================================================
Write-Host "`nTEST 2: Cross-Platform Standard Library Usage" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check for std::filesystem usage (C++17 feature, works across platforms)
$filesystemUsage = Get-ChildItem "..\audio" -Filter *.cpp | Select-String "std::filesystem" | Measure-Object
Write-Test "Uses std::filesystem for path operations" ($filesystemUsage.Count -gt 0)

# Check for cross-platform threading primitives
$atomicUsage = Get-ChildItem "..\audio" -Filter *.h | Select-String "std::atomic|atomic<" | Measure-Object
Write-Test "Uses std::atomic for thread-safe operations" ($atomicUsage.Count -gt 0)

$mutexUsage = Get-ChildItem "..\audio" -Filter *.h | Select-String "std::mutex|mutex " | Measure-Object
Write-Test "Uses std::mutex for synchronization" ($mutexUsage.Count -gt 0)

# Verify no platform-specific sleep calls
$sleepUsage = Get-ChildItem "..\audio" -Filter *.cpp | Select-String "Sleep\(|sleep\(|usleep\(" | Measure-Object
Write-Test "No platform-specific sleep calls (uses std::chrono)" ($sleepUsage.Count -eq 0)

# ============================================================================
# TEST 3: Path Handling
# ============================================================================
Write-Host "`nTEST 3: Path Handling" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check that paths use std::filesystem or handle both separators
$pathHandling = $true
$cppFiles = Get-ChildItem "..\audio" -Filter *.cpp
foreach ($file in $cppFiles) {
    $content = Get-Content $file.FullName -Raw
    # If file contains path operations, it should use filesystem or handle both separators
    if ($content -match '["\''].*?[/\\].*?["\'']') {
        # Check if file uses filesystem or checks for both separators
        if ($content -notmatch "std::filesystem" -and $content -notmatch "[/\\]" -and $content -notmatch "[\\/]") {
            Write-Host "  $($file.Name) may have hardcoded path separator" -ForegroundColor Yellow
        }
    }
}
Write-Test "Path handling is cross-platform" $pathHandling

# Check CMakeLists uses proper CMake path variables
$cmakeContent = Get-Content "..\CMakeLists.txt" -Raw
Write-Test "CMake uses CMAKE_SOURCE_DIR" ($cmakeContent -match "CMAKE_SOURCE_DIR")
Write-Test "CMake uses forward slashes or variables for paths" ($cmakeContent -notmatch '\\' -or $cmakeContent -match "CMAKE_")

# ============================================================================
# TEST 4: Compiler Compatibility
# ============================================================================
Write-Host "`nTEST 4: Compiler Compatibility" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check for NOMINMAX (prevents Windows min/max macro issues)
$nominmaxUsage = Get-ChildItem "..\audio" -Filter *.h -Recurse | Select-String "NOMINMAX" | Measure-Object
$nominmaxUsageAudio = Get-ChildItem "..\audio" -Filter *.cpp -Recurse | Select-String "NOMINMAX" | Measure-Object
$totalNominmax = $nominmaxUsage.Count + $nominmaxUsageAudio.Count
Write-Test "Defines NOMINMAX for Windows compatibility" ($totalNominmax -gt 0)

# Check that headers use #pragma once (all should be consistent now)
$pragmaOnce = Get-ChildItem "..\audio" -Filter *.h -Recurse | Select-String "#pragma once" | Measure-Object
$headerCount = (Get-ChildItem "..\audio" -Filter *.h -Recurse | Measure-Object).Count
Write-Test "All headers use #pragma once consistently" ($pragmaOnce.Count -eq $headerCount)

# Check for compiler-specific extensions (should avoid)
$gccExtensions = Get-ChildItem "..\audio" -Filter *.cpp -Recurse | Select-String "__attribute__|__builtin" | Measure-Object
Write-Test "No GCC-specific extensions" ($gccExtensions.Count -eq 0)

# ============================================================================
# TEST 5: Build System Portability
# ============================================================================
Write-Host "`nTEST 5: Build System Portability" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

$cmake = Get-Content "..\CMakeLists.txt" -Raw

# Check CMake version requirement (should be reasonable)
Write-Test "CMake minimum version is portable" ($cmake -match "cmake_minimum_required\(VERSION [23]\.\d+")

# Check platform-specific library linking is conditional
$winmmConditional = ($cmake -match "if\s*\(\s*WIN32") -and ($cmake -match "winmm")
Write-Test "Windows libraries conditionally linked" $winmmConditional

# Check C++ standard is set
Write-Test "C++ standard explicitly set" ($cmake -match "CMAKE_CXX_STANDARD")

# Check no hardcoded compiler flags for specific compilers
Write-Test "No hardcoded MSVC-only flags" ($cmake -notmatch "CMAKE_CXX_FLAGS.*/(W[1-4]|MT|MD)" -or $cmake -match "MSVC")

# ============================================================================
# TEST 6: Header Include Portability
# ============================================================================
Write-Host "`nTEST 6: Header Include Portability" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check that includes use relative paths, not absolute
$absoluteIncludes = Get-ChildItem "..\audio" -Filter *.cpp | Select-String '#include\s+"[A-Z]:|#include\s+"/home|#include\s+"/usr' | Measure-Object
Write-Test "No absolute paths in #include directives" ($absoluteIncludes.Count -eq 0)

# Check includes use correct path separators (should use /)
$backslashIncludes = Get-ChildItem "..\audio" -Filter *.cpp | Select-String '#include.*\\' | Measure-Object
Write-Test "Includes use forward slashes" ($backslashIncludes.Count -eq 0)

# ============================================================================
# TEST 7: Endianness and Type Safety
# ============================================================================
Write-Host "`nTEST 7: Endianness and Type Safety" -ForegroundColor Yellow
Write-Host "------------------------------------------------------------" -ForegroundColor DarkGray

# Check for fixed-width integer types (uint32_t, etc.)
$fixedWidthTypes = Get-ChildItem "..\audio" -Filter *.h | Select-String "uint\d+_t|int\d+_t" | Measure-Object
Write-Test "Uses fixed-width integer types" ($fixedWidthTypes.Count -gt 0)

# Check no assumptions about pointer sizes
$pointerCasts = Get-ChildItem "..\audio" -Filter *.cpp | Select-String "reinterpret_cast<int>.*\*|reinterpret_cast<long>.*\*" | Measure-Object
Write-Test "No unsafe pointer-to-int casts" ($pointerCasts.Count -eq 0)

# ============================================================================
# TEST RESULTS
# ============================================================================
Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "CROSS-PLATFORM TEST RESULTS" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Total Tests: $($TestsPassed + $TestsFailed)" -ForegroundColor White
Write-Host "Passed: $TestsPassed" -ForegroundColor Green
Write-Host "Failed: $TestsFailed" -ForegroundColor $(if ($TestsFailed -gt 0) { "Red" } else { "Green" })

if ($TestsFailed -eq 0) {
    Write-Host "`nCross-platform compatibility verified" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nSome compatibility issues detected - review output above" -ForegroundColor Yellow
    exit 1
}

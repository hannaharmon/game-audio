# Audio System Test Suite
# Runs comprehensive automated tests + cross-platform validation

$ErrorActionPreference = "Continue"
$TestsPassed = 0
$TestsFailed = 0

function Write-TestHeader {
    param($name)
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host $name -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
}

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
Write-TestHeader "BUILD"

$repoRoot = Join-Path (Join-Path $PSScriptRoot "..") ".."
Push-Location $repoRoot

if ($IsWindows -or $env:OS -match "Windows") {
    & (Join-Path $repoRoot "scripts\build.ps1") -Configurations Debug,Release
} else {
    & (Join-Path $repoRoot "scripts\build.ps1") -Configurations Debug
}

Pop-Location

# ============================================================================
Write-TestHeader "BUILD VERIFICATION"

if ($IsWindows -or $env:OS -match "Windows") {
    $buildBinDir = Join-Path $repoRoot "build\Debug"
    Write-Test "Basic tests executable exists" (Test-Path (Join-Path $buildBinDir "test_audio_manager_basic.exe"))
    Write-Test "Error handling tests executable exists" (Test-Path (Join-Path $buildBinDir "test_audio_manager_error_handling.exe"))
    Write-Test "Interactive test executable exists" (Test-Path (Join-Path $buildBinDir "test_audio.exe"))
} else {
    $buildBinDir = Join-Path $repoRoot "build"
    Write-Test "Basic tests executable exists" (Test-Path (Join-Path $buildBinDir "test_audio_manager_basic"))
    Write-Test "Error handling tests executable exists" (Test-Path (Join-Path $buildBinDir "test_audio_manager_error_handling"))
    Write-Test "Interactive test executable exists" (Test-Path (Join-Path $buildBinDir "test_audio"))
}
Write-Test "CMakeLists.txt exists" (Test-Path (Join-Path $repoRoot "CMakeLists.txt"))
Write-Test "Sound files directory exists" (Test-Path (Join-Path $repoRoot "sound_files"))

# ============================================================================
Write-TestHeader "C++ TESTS"

Write-Host "`nRunning C++ test suite...`n" -ForegroundColor Yellow
& "$PSScriptRoot\run_cpp_tests.ps1"
$cpp_exit_code = $LASTEXITCODE
Write-Host "" # blank line
Write-Test "All C++ tests passed" ($cpp_exit_code -eq 0)

# ============================================================================
Write-TestHeader "PYTHON TESTS"

Write-Host "`nRunning Python test suite...`n" -ForegroundColor Yellow
& "$PSScriptRoot\run_python_tests.ps1"
$python_exit_code = $LASTEXITCODE
Write-Host "" # blank line
Write-Test "All Python tests passed" ($python_exit_code -eq 0)

# ============================================================================
Write-TestHeader "CROSS-PLATFORM COMPATIBILITY"

Write-Host "`nRunning platform compatibility checks...`n" -ForegroundColor Yellow

& "$PSScriptRoot\test_cross_platform.ps1"
$cross_platform_exit = $LASTEXITCODE

Write-Host "" # blank line
Write-Test "Cross-platform validation passed" ($cross_platform_exit -eq 0)

# ============================================================================
Write-TestHeader "TEST SUMMARY"

Write-Host "Total Tests: $($TestsPassed + $TestsFailed)" -ForegroundColor White
Write-Host "Passed: $TestsPassed" -ForegroundColor Green
Write-Host "Failed: $TestsFailed" -ForegroundColor $(if ($TestsFailed -gt 0) { "Red" } else { "Green" })

if ($TestsFailed -eq 0 -and $cross_platform_exit -eq 0 -and $cpp_exit_code -eq 0 -and $python_exit_code -eq 0) {
    Write-Host "`nAll tests passed" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nSome tests failed" -ForegroundColor Red
    exit 1
}

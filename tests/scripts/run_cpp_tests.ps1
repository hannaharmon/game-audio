#!/usr/bin/env pwsh
# C++ Test Runner
# Runs all C++ test executables for the audio system

Write-Host "==================================="
Write-Host "Audio System C++ Tests"
Write-Host "==================================="
Write-Host ""

# Detect platform
if ($IsWindows -or $env:OS -match "Windows") {
    $platform = "Windows"
    $buildDir = "build\Debug"
    $exe = ".exe"
} elseif ($IsLinux) {
    $platform = "Linux"
    $buildDir = "build"
    $exe = ""
} elseif ($IsMacOS) {
    $platform = "macOS"
    $buildDir = "build"
    $exe = ""
} else {
    $platform = "Unknown"
    $buildDir = "build"
    $exe = ""
}

Write-Host "Platform: $platform"
Write-Host ""

# Navigate to repository root
$repoRoot = Join-Path (Join-Path $PSScriptRoot "..") ".."
Push-Location $repoRoot

# Check if build exists
if (-not (Test-Path $buildDir)) {
    Write-Host "ERROR: Build directory not found at $buildDir" -ForegroundColor Red
    Write-Host "Please build the project first with:" -ForegroundColor Yellow
    if ($IsWindows -or $env:OS -match "Windows") {
        Write-Host "  cd build && cmake --build . --config Debug" -ForegroundColor Yellow
    } else {
        Write-Host "  cmake -B build && cmake --build build" -ForegroundColor Yellow
    }
    Pop-Location
    exit 1
}

# Run basic functionality tests
Write-Host "Running basic functionality tests..." -ForegroundColor Cyan
$basicExe = Join-Path $buildDir "test_audio_manager_basic$exe"
if (Test-Path $basicExe) {
    & $basicExe
    $basicResult = $LASTEXITCODE
} else {
    Write-Host "ERROR: Basic tests executable not found at $basicExe" -ForegroundColor Red
    $basicResult = 1
}

Write-Host ""

# Run error handling tests
Write-Host "Running error handling tests..." -ForegroundColor Cyan
$errorExe = Join-Path $buildDir "test_audio_manager_error_handling$exe"
if (Test-Path $errorExe) {
    & $errorExe
    $errorResult = $LASTEXITCODE
} else {
    Write-Host "ERROR: Error handling tests executable not found at $errorExe" -ForegroundColor Red
    $errorResult = 1
}

Pop-Location

Write-Host ""
Write-Host "==================================="
Write-Host "C++ Test Summary"
Write-Host "==================================="

if ($basicResult -eq 0) {
    Write-Host "Basic Tests: PASSED" -ForegroundColor Green
} else {
    Write-Host "Basic Tests: FAILED" -ForegroundColor Red
}

if ($errorResult -eq 0) {
    Write-Host "Error Handling Tests: PASSED" -ForegroundColor Green
} else {
    Write-Host "Error Handling Tests: FAILED" -ForegroundColor Red
}

Write-Host ""

if ($basicResult -eq 0 -and $errorResult -eq 0) {
    Write-Host "All C++ tests passed on $platform!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some C++ tests failed on $platform" -ForegroundColor Red
    exit 1
}

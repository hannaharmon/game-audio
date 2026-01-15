#!/usr/bin/env pwsh
# Audio System Test Runner - Cross-Platform
# Runs both basic and comprehensive test suites

Write-Host "==================================="
Write-Host "Audio System Python Tests"
Write-Host "==================================="
Write-Host ""

# Detect platform
if ($IsWindows -or $env:OS -match "Windows") {
    $platform = "Windows"
    $buildDir = "build\Debug"
} elseif ($IsLinux) {
    $platform = "Linux"
    $buildDir = "build"
} elseif ($IsMacOS) {
    $platform = "macOS"
    $buildDir = "build"
} else {
    $platform = "Unknown"
    $buildDir = "build"
}

Write-Host "Platform: $platform"
Write-Host "Python: $(python --version)"
Write-Host ""

# Check if build exists
$buildPath = Join-Path $PSScriptRoot ".." $buildDir
if (-not (Test-Path $buildPath)) {
    Write-Host "ERROR: Build directory not found at $buildPath" -ForegroundColor Red
    Write-Host "Please build the project first with:" -ForegroundColor Yellow
    if ($IsWindows -or $env:OS -match "Windows") {
        Write-Host "  cd build && cmake --build . --config Debug" -ForegroundColor Yellow
    } else {
        Write-Host "  cmake -B build && cmake --build build" -ForegroundColor Yellow
    }
    exit 1
}

Write-Host "Running basic tests..." -ForegroundColor Cyan
python (Join-Path $PSScriptRoot "test_python.py")
$basicResult = $LASTEXITCODE

Write-Host ""
Write-Host "Running comprehensive tests..." -ForegroundColor Cyan
python (Join-Path $PSScriptRoot "test_comprehensive.py")
$comprehensiveResult = $LASTEXITCODE

Write-Host ""
Write-Host "==================================="
Write-Host "Test Summary"
Write-Host "==================================="

if ($basicResult -eq 0) {
    Write-Host "Basic Tests: PASSED" -ForegroundColor Green
} else {
    Write-Host "Basic Tests: FAILED" -ForegroundColor Red
}

if ($comprehensiveResult -eq 0) {
    Write-Host "Comprehensive Tests: PASSED" -ForegroundColor Green
} else {
    Write-Host "Comprehensive Tests: FAILED" -ForegroundColor Red
}

Write-Host ""

if ($basicResult -eq 0 -and $comprehensiveResult -eq 0) {
    Write-Host "All tests passed on $platform!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some tests failed on $platform" -ForegroundColor Red
    exit 1
}

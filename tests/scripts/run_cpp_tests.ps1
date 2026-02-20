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

# Test executables organized by functionality
$testExecutables = @(
    "test_audio_initialization",
    "test_audio_logging",
    "test_audio_volume",
    "test_audio_groups",
    "test_audio_sounds",
    "test_audio_tracks",
    "test_audio_validation",
    "test_audio_threading",
    "test_audio_resources",
    "test_audio_spatial"
)

$testResults = @{}
$allPassed = $true

foreach ($testName in $testExecutables) {
    Write-Host "Running $testName..." -ForegroundColor Cyan
    $testExe = Join-Path $buildDir "$testName$exe"
    if (Test-Path $testExe) {
        & $testExe
        $result = $LASTEXITCODE
        $testResults[$testName] = $result
        if ($result -ne 0) {
            $allPassed = $false
        }
    } else {
        Write-Host "ERROR: $testName executable not found at $testExe" -ForegroundColor Red
        $testResults[$testName] = 1
        $allPassed = $false
    }
    Write-Host ""
}

Pop-Location

Write-Host "==================================="
Write-Host "C++ Test Summary"
Write-Host "==================================="

foreach ($testName in $testExecutables) {
    if ($testResults[$testName] -eq 0) {
        Write-Host "$testName : PASSED" -ForegroundColor Green
    } else {
        Write-Host "$testName : FAILED" -ForegroundColor Red
    }
}

Write-Host ""

if ($allPassed) {
    Write-Host "All C++ tests passed on $platform!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some C++ tests failed on $platform" -ForegroundColor Red
    exit 1
}

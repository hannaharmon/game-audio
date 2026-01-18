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
    $buildDir = "build\Release"
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
$repoRoot = Join-Path (Join-Path $PSScriptRoot "..") ".."
$buildPath = Join-Path $repoRoot $buildDir
$fallbackBuildPath = Join-Path $repoRoot "build"
$debugBuildPath = Join-Path $repoRoot "build\Debug"
if (-not (Test-Path $buildPath) -and -not (Test-Path $debugBuildPath) -and -not (Test-Path $fallbackBuildPath)) {
    Write-Host "ERROR: Build directory not found at $buildPath" -ForegroundColor Red
    Write-Host "Please build the project first with:" -ForegroundColor Yellow
    if ($IsWindows -or $env:OS -match "Windows") {
        Write-Host "  cd build && cmake --build . --config Release" -ForegroundColor Yellow
    } else {
        Write-Host "  cmake -B build && cmake --build build" -ForegroundColor Yellow
    }
    exit 1
}

# Add build outputs to PYTHONPATH so Python can find audio_py module
$env:PYTHONPATH = $buildPath
if (Test-Path $debugBuildPath) {
    $env:PYTHONPATH = "$env:PYTHONPATH;$debugBuildPath"
}
if ((Test-Path $fallbackBuildPath) -and ($fallbackBuildPath -ne $buildPath)) {
    $env:PYTHONPATH = "$env:PYTHONPATH;$fallbackBuildPath"
}
Write-Host "Working directory: $buildPath"
Write-Host ""

# Test files organized by functionality
$testFiles = @(
    "test_audio_initialization",
    "test_audio_volume",
    "test_audio_groups",
    "test_audio_sounds",
    "test_audio_tracks",
    "test_audio_validation",
    "test_audio_threading",
    "test_audio_resources"
)

$testResults = @{}
$allPassed = $true

foreach ($testName in $testFiles) {
    Write-Host "Running $testName..." -ForegroundColor Cyan
    $testPath = Join-Path $repoRoot "tests\python\$testName.py"
    if (Test-Path $testPath) {
        python $testPath
        $result = $LASTEXITCODE
        $testResults[$testName] = $result
        if ($result -ne 0) {
            $allPassed = $false
        }
    } else {
        Write-Host "ERROR: $testName.py not found at $testPath" -ForegroundColor Red
        $testResults[$testName] = 1
        $allPassed = $false
    }
    Write-Host ""
}

Write-Host "==================================="
Write-Host "Test Summary"
Write-Host "==================================="

foreach ($testName in $testFiles) {
    if ($testResults[$testName] -eq 0) {
        Write-Host "$testName : PASSED" -ForegroundColor Green
    } else {
        Write-Host "$testName : FAILED" -ForegroundColor Red
    }
}

Write-Host ""

if ($allPassed) {
    Write-Host "All tests passed on $platform!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some tests failed on $platform" -ForegroundColor Red
    exit 1
}

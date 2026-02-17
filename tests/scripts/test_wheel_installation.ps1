#!/usr/bin/env pwsh
# Test Python bindings using installed wheel
# This script builds a wheel, installs it, and runs all Python tests against it

Write-Host "===================================" -ForegroundColor Cyan
Write-Host "Testing with Installed Wheel" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Stop"

# Get repository root (assuming script is in tests/scripts/)
$repoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
Push-Location $repoRoot

try {
    # Step 1: Build the wheel
    Write-Host "Step 1: Building wheel..." -ForegroundColor Yellow
    if (-not (Test-Path "pyproject.toml")) {
        Write-Host "ERROR: pyproject.toml not found. Run this from the repository root." -ForegroundColor Red
        exit 1
    }

    # Clean previous builds
    if (Test-Path "dist") {
        Remove-Item -Recurse -Force dist
    }
    if (Test-Path "build") {
        Remove-Item -Recurse -Force build
    }

    python -m build --wheel
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Wheel build failed" -ForegroundColor Red
        exit 1
    }
    Write-Host "Wheel built successfully" -ForegroundColor Green
    Write-Host ""

    # Step 2: Find the built wheel
    $wheelFile = (Get-ChildItem dist/*.whl | Select-Object -First 1).FullName
    if (-not $wheelFile) {
        Write-Host "ERROR: No wheel file found in dist/" -ForegroundColor Red
        exit 1
    }
    Write-Host "Found wheel: $(Split-Path $wheelFile -Leaf)" -ForegroundColor Cyan
    Write-Host ""

    # Step 3: Create a clean virtual environment
    Write-Host "Step 2: Creating clean virtual environment..." -ForegroundColor Yellow
    $testEnv = Join-Path $repoRoot "test_wheel_env"
    if (Test-Path $testEnv) {
        Remove-Item -Recurse -Force $testEnv
    }
    python -m venv $testEnv
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Failed to create virtual environment" -ForegroundColor Red
        exit 1
    }

    # Activate virtual environment
    $activateScript = Join-Path $testEnv "Scripts\Activate.ps1"
    if (-not (Test-Path $activateScript)) {
        Write-Host "ERROR: Virtual environment activation script not found" -ForegroundColor Red
        exit 1
    }
    & $activateScript
    Write-Host "Virtual environment activated" -ForegroundColor Green
    Write-Host ""

    # Step 4: Install the wheel
    Write-Host "Step 3: Installing wheel..." -ForegroundColor Yellow
    pip install --upgrade pip
    pip install $wheelFile
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Wheel installation failed" -ForegroundColor Red
        exit 1
    }
    Write-Host "Wheel installed successfully" -ForegroundColor Green
    Write-Host ""

    # Step 5: Verify installation
    Write-Host "Step 4: Verifying installation..." -ForegroundColor Yellow
    python -c "import game_audio; print(f'Successfully imported game_audio version {game_audio.__version__}')"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Import verification failed" -ForegroundColor Red
        exit 1
    }
    Write-Host ""

    # Step 6: Run all Python tests
    Write-Host "Step 5: Running Python tests against installed wheel..." -ForegroundColor Yellow
    Write-Host ""

    $testFiles = @(
        "test_audio_initialization",
        "test_audio_logging",
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
            # Change to a temp directory to avoid importing local source
            $tempDir = [System.IO.Path]::GetTempPath()
            Push-Location $tempDir
            python $testPath
            $result = $LASTEXITCODE
            Pop-Location
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

    # Step 7: Summary
    Write-Host "===================================" -ForegroundColor Cyan
    Write-Host "Test Summary" -ForegroundColor Cyan
    Write-Host "===================================" -ForegroundColor Cyan
    Write-Host ""

    foreach ($testName in $testFiles) {
        if ($testResults[$testName] -eq 0) {
            Write-Host "$testName : PASSED" -ForegroundColor Green
        } else {
            Write-Host "$testName : FAILED" -ForegroundColor Red
        }
    }

    Write-Host ""

    # Cleanup
    Write-Host "Cleaning up..." -ForegroundColor Yellow
    deactivate
    Remove-Item -Recurse -Force $testEnv -ErrorAction SilentlyContinue

    if ($allPassed) {
        Write-Host "All tests passed with installed wheel!" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Some tests failed" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
} finally {
    Pop-Location
}

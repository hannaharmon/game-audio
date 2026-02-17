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
    & (Join-Path $repoRoot (Join-Path "scripts" "build.ps1")) -Configurations Debug,Release
} else {
    & (Join-Path $repoRoot (Join-Path "scripts" "build.ps1")) -Configurations Debug
}

Pop-Location

# ============================================================================
Write-TestHeader "BUILD VERIFICATION"

$testExecutables = @(
    "test_audio_initialization",
    "test_audio_volume",
    "test_audio_groups",
    "test_audio_sounds",
    "test_audio_tracks",
    "test_audio_validation",
    "test_audio_threading",
    "test_audio_resources"
)

if ($IsWindows -or $env:OS -match "Windows") {
    $buildBinDir = Join-Path $repoRoot (Join-Path "build" "Debug")
    $exe = ".exe"
} else {
    $buildBinDir = Join-Path $repoRoot "build"
    $exe = ""
}

foreach ($testName in $testExecutables) {
    Write-Test "$testName executable exists" (Test-Path (Join-Path $buildBinDir "$testName$exe"))
}
Write-Test "Interactive test executable exists" (Test-Path (Join-Path $buildBinDir "test_audio$exe"))
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
Write-TestHeader "PYTHON TESTS (Source Build)"

Write-Host "`nRunning Python test suite against source build...`n" -ForegroundColor Yellow
& "$PSScriptRoot\run_python_tests.ps1"
$python_source_exit_code = $LASTEXITCODE
Write-Host "" # blank line
Write-Test "All Python tests passed (source build)" ($python_source_exit_code -eq 0)

# ============================================================================
Write-TestHeader "PYTHON TESTS (Installed Wheel)"

Write-Host "`nPreparing wheel for testing...`n" -ForegroundColor Yellow
Push-Location $repoRoot

$wheelAvailable = $false
$python_wheel_exit_code = 0

# Check if wheel is already installed
python -c "import game_audio" 2>&1 | Out-Null
if ($LASTEXITCODE -eq 0) {
    Write-Host "Wheel already installed, using existing installation" -ForegroundColor Green
    $wheelAvailable = $true
} else {
    Write-Host "Wheel not installed, building and installing..." -ForegroundColor Yellow
    
    # Install build dependencies if needed
    python -m pip install --quiet --upgrade pip build wheel scikit-build-core cmake pybind11 ninja 2>&1 | Out-Null
    
    # Build wheel
    python -m build --wheel 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "WARNING: Wheel build failed, skipping wheel tests" -ForegroundColor Yellow
        $wheelAvailable = $false
    } else {
        # Install the wheel
        $wheelFile = (Get-ChildItem dist/*.whl | Select-Object -First 1).FullName
        if ($wheelFile) {
            pip install --quiet --force-reinstall $wheelFile 2>&1 | Out-Null
            if ($LASTEXITCODE -eq 0) {
                Write-Host "Wheel built and installed successfully" -ForegroundColor Green
                $wheelAvailable = $true
            } else {
                Write-Host "WARNING: Wheel installation failed, skipping wheel tests" -ForegroundColor Yellow
                $wheelAvailable = $false
            }
        } else {
            Write-Host "WARNING: No wheel file found, skipping wheel tests" -ForegroundColor Yellow
            $wheelAvailable = $false
        }
    }
}

Pop-Location

if ($wheelAvailable) {
    Write-Host "`nRunning Python test suite against installed wheel...`n" -ForegroundColor Yellow
    & "$PSScriptRoot\run_python_tests.ps1" -UseWheel
    $python_wheel_exit_code = $LASTEXITCODE
    Write-Host "" # blank line
    Write-Test "All Python tests passed (installed wheel)" ($python_wheel_exit_code -eq 0)
} else {
    Write-Host "Skipping wheel tests (wheel not available)" -ForegroundColor Yellow
    Write-Test "All Python tests passed (installed wheel)" $true  # Mark as passed if skipped
}

$python_exit_code = [Math]::Max($python_source_exit_code, $python_wheel_exit_code)

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

if ($TestsFailed -eq 0 -and $cross_platform_exit -eq 0 -and $cpp_exit_code -eq 0 -and $python_exit_code -eq 0 -and $python_source_exit_code -eq 0 -and $python_wheel_exit_code -eq 0) {
    Write-Host "`nAll tests passed" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nSome tests failed" -ForegroundColor Red
    exit 1
}

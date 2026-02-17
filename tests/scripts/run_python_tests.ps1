#!/usr/bin/env pwsh
# Audio System Test Runner - Cross-Platform
# Runs both basic and comprehensive test suites
# 
# Usage:
#   .\run_python_tests.ps1              # Test against source build (default)
#   .\run_python_tests.ps1 -UseWheel     # Test against installed wheel

param(
    [switch]$UseWheel = $false
)

Write-Host "==================================="
Write-Host "Audio System Python Tests"
if ($UseWheel) {
    Write-Host "Testing against: Installed Wheel"
} else {
    Write-Host "Testing against: Source Build"
}
Write-Host "==================================="
Write-Host ""

# Detect platform
if ($IsWindows -or $env:OS -match "Windows") {
    $platform = "Windows"
    $buildDir = Join-Path "build" "Release"
    $pathSeparator = ";"
} elseif ($IsLinux) {
    $platform = "Linux"
    $buildDir = "build"
    $pathSeparator = ":"
} elseif ($IsMacOS) {
    $platform = "macOS"
    $buildDir = "build"
    $pathSeparator = ":"
} else {
    $platform = "Unknown"
    $buildDir = "build"
    $pathSeparator = ":"
}

Write-Host "Platform: $platform"
Write-Host "Python: $(python --version)"
Write-Host ""

$repoRoot = Join-Path (Join-Path $PSScriptRoot "..") ".."

if ($UseWheel) {
    # Test against installed wheel - verify it's installed
    Write-Host "Checking for installed wheel package..." -ForegroundColor Yellow
    
    # Clear PYTHONPATH and change to temp directory to avoid local source
    $originalPYTHONPATH = $env:PYTHONPATH
    $env:PYTHONPATH = ""
    $tempCheckDir = [System.IO.Path]::GetTempPath()
    Push-Location $tempCheckDir
    
    # Verify the installed package can be imported and is not from local source
    $checkScript = [System.IO.Path]::GetTempFileName() + ".py"
    # Use forward slashes for Python paths (works on all platforms)
    $repoRootEscaped = $repoRoot.Replace('\', '/').Replace('\\', '/')
    @'
import sys
import os
# Remove current directory from path
if os.getcwd() in sys.path:
    sys.path.remove(os.getcwd())
if '' in sys.path:
    sys.path.remove('')

# Remove repo root if it's in path
repo_root = r'$repoRootEscaped'
if repo_root in sys.path:
    sys.path.remove(repo_root)

try:
    import audio_py
    package_path = os.path.dirname(audio_py.__file__)
    print(f'Found audio_py version {audio_py.__version__}')
    print(f'Package location: {package_path}')
    
    # Verify it's the installed package, not local source
    # The installed package should be in site-packages, not in the repo
    if 'site-packages' in package_path or 'local-packages' in package_path:
        # Try to import the compiled extension
        try:
            import audio_py.audio_py  # This will fail if it's local source without compiled extension
            print('Verified: Using installed wheel (compiled extension found)')
            sys.exit(0)
        except ImportError:
            print('WARNING: Package found but compiled extension missing')
            sys.exit(1)
    else:
        print(f'ERROR: Package appears to be from local source: {package_path}')
        print('This is likely the local audio_py directory, not the installed wheel')
        sys.exit(1)
except ImportError as e:
    print(f'ERROR: {e}')
    print('audio_py package not found in installed packages')
    sys.exit(1)
'@ | Out-File -FilePath $checkScript -Encoding utf8
    
    python $checkScript 2>&1 | Out-Null
    $checkResult = $LASTEXITCODE
    Remove-Item $checkScript -ErrorAction SilentlyContinue
    Pop-Location
    $env:PYTHONPATH = $originalPYTHONPATH
    
    if ($checkResult -ne 0) {
        Write-Host "ERROR: audio_py package not found or not properly installed." -ForegroundColor Red
        Write-Host "Install it with: pip install game-audio-py" -ForegroundColor Yellow
        Write-Host "Or build and install a wheel locally: pip install dist/*.whl" -ForegroundColor Yellow
        exit 1
    }
    Write-Host "Using installed wheel package" -ForegroundColor Green
    Write-Host ""
    
    # Clear PYTHONPATH to ensure we use installed package, not local source
    $env:PYTHONPATH = ""
} else {
    # Test against source build
    $buildPath = Join-Path $repoRoot $buildDir
    $fallbackBuildPath = Join-Path $repoRoot "build"
    if ($IsWindows -or $env:OS -match "Windows") {
        $debugBuildPath = Join-Path $repoRoot (Join-Path "build" "Debug")
    } else {
        $debugBuildPath = $fallbackBuildPath  # Debug and Release are same on Unix
    }
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
        $env:PYTHONPATH = "$env:PYTHONPATH$pathSeparator$debugBuildPath"
    }
    if ((Test-Path $fallbackBuildPath) -and ($fallbackBuildPath -ne $buildPath)) {
        $env:PYTHONPATH = "$env:PYTHONPATH$pathSeparator$fallbackBuildPath"
    }
    Write-Host "Working directory: $buildPath"
    Write-Host ""
}

# Test files organized by functionality
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

# When testing against wheel, run all tests from a temp directory
if ($UseWheel) {
    $tempTestDir = [System.IO.Path]::GetTempPath() + [System.IO.Path]::GetRandomFileName()
    New-Item -ItemType Directory -Path $tempTestDir -Force | Out-Null
    Push-Location $tempTestDir
    Write-Host "Running tests from temporary directory to ensure we use installed wheel" -ForegroundColor Gray
    Write-Host ""
}

foreach ($testName in $testFiles) {
    Write-Host "Running $testName..." -ForegroundColor Cyan
    $testPath = Join-Path $repoRoot (Join-Path "tests" (Join-Path "python" "$testName.py"))
    if (Test-Path $testPath) {
        # When using wheel, run from temp directory with sys.path cleaned
        if ($UseWheel) {
            $wrapperScript = Join-Path $tempTestDir "${testName}_wrapper.py"
            # Use forward slashes for Python paths (works on all platforms)
            $repoRootEscaped = $repoRoot.Replace('\', '/').Replace('\\', '/')
            $testPathEscaped = $testPath.Replace('\', '/').Replace('\\', '/')
            $testsPythonDir = (Join-Path $repoRoot (Join-Path "tests" "python")).Replace('\', '/').Replace('\\', '/')
            @"
import sys
import os

# Remove current directory and repo root from path to ensure we use installed package
current_dir = os.getcwd()
if current_dir in sys.path:
    sys.path.remove(current_dir)
if '' in sys.path:
    sys.path.remove('')

# Remove repo root if it's in path
repo_root = r'$repoRootEscaped'
if repo_root in sys.path:
    sys.path.remove(repo_root)

# Add tests/python directory to path so test_common can be found
# But only if it's not already there (to avoid importing local audio_py)
tests_dir = r'$testsPythonDir'
if tests_dir not in sys.path:
    sys.path.insert(0, tests_dir)

# Verify we're using installed package
try:
    import audio_py
    package_path = os.path.dirname(audio_py.__file__)
    if 'site-packages' not in package_path and 'local-packages' not in package_path:
        print(f'ERROR: Not using installed package! Found at: {package_path}')
        sys.exit(1)
except ImportError as e:
    print(f'ERROR: Cannot import audio_py: {e}')
    sys.exit(1)

# Import and run the test
import importlib.util
spec = importlib.util.spec_from_file_location('$testName', r'$testPathEscaped')
test_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(test_module)
"@ | Out-File -FilePath $wrapperScript -Encoding utf8
            python $wrapperScript
            $result = $LASTEXITCODE
            Remove-Item $wrapperScript -ErrorAction SilentlyContinue
        } else {
            python $testPath
            $result = $LASTEXITCODE
        }
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

if ($UseWheel) {
    Pop-Location
    Remove-Item $tempTestDir -Recurse -Force -ErrorAction SilentlyContinue
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

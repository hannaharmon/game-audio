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
Write-TestHeader "BUILD VERIFICATION"

Write-Test "Automated test executable exists" (Test-Path "..\build\Debug\test_automated.exe")
Write-Test "Interactive test executable exists" (Test-Path "..\build\Debug\test_audio.exe")
Write-Test "CMakeLists.txt exists" (Test-Path "..\CMakeLists.txt")
Write-Test "Sound files directory exists" (Test-Path "..\sound_files")

# ============================================================================
Write-TestHeader "AUTOMATED FUNCTIONAL TESTS"

if (Test-Path "..\build\Debug\test_automated.exe") {
    Write-Host "`nRunning comprehensive automated test suite...`n" -ForegroundColor Yellow
    
    $test_output = & "..\build\Debug\test_automated.exe" 2>&1
    $test_exit_code = $LASTEXITCODE
    
    # Display test output
    $test_output | ForEach-Object { Write-Host $_ }
    
    # Check results
    Write-Host "" # blank line
    Write-Test "All automated tests passed" ($test_exit_code -eq 0)
    
    # Parse test results from output
    $passed_match = $test_output | Select-String "Passed: (\d+)"
    $failed_match = $test_output | Select-String "Failed: (\d+)"
    
    if ($passed_match) {
        $tests_run = $passed_match.Matches[0].Groups[1].Value
        Write-Host "Automated suite: $tests_run tests completed" -ForegroundColor Cyan
    }
} else {
    Write-Host "Automated test executable not found!" -ForegroundColor Red
    $TestsFailed++
}

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

if ($TestsFailed -eq 0 -and $cross_platform_exit -eq 0) {
    Write-Host "`nAll tests passed" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nSome tests failed" -ForegroundColor Red
    exit 1
}

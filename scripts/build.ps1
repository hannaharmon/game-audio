#!/usr/bin/env pwsh
# Cross-platform build helper for the audio module.

param(
    [string[]]$Configurations
)

$repoRoot = Join-Path (Join-Path $PSScriptRoot "..") ""

# Use the Python from PATH to ensure build and tests share the same version.
$pythonExe = $null
$pythonCmd = Get-Command python -ErrorAction SilentlyContinue
if ($pythonCmd) {
    $pythonExe = $pythonCmd.Source
}

$pythonArgs = @()
if ($pythonExe) {
    $pythonArgs += "-DPython_EXECUTABLE=$pythonExe"
    $pythonArgs += "-DPython3_EXECUTABLE=$pythonExe"
}

if (-not $Configurations -or $Configurations.Count -eq 0) {
    $Configurations = @("Debug")
}

if ($IsWindows -or $env:OS -match "Windows") {
    Push-Location $repoRoot
    cmake -S . -B build @pythonArgs | Out-Host
    foreach ($config in $Configurations) {
        cmake --build build --config $config | Out-Host
        if ($LASTEXITCODE -ne 0) { Pop-Location; exit $LASTEXITCODE }
    }
    Pop-Location
    exit 0
}

if ($Configurations.Count -gt 1) {
    Write-Host "Non-Windows builds support a single configuration. Using $($Configurations[0])." -ForegroundColor Yellow
}

$buildType = $Configurations[0]
Push-Location $repoRoot
cmake -S . -B build -DCMAKE_BUILD_TYPE=$buildType @pythonArgs | Out-Host
cmake --build build | Out-Host
$exitCode = $LASTEXITCODE
Pop-Location
exit $exitCode

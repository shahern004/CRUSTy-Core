# ZEPHYR ENVIRONMENT DIAGNOSTIC TOOL
# This script helps diagnose issues with the Zephyr development environment
# Last Updated: 2025-03-09
#
# Usage: .\zephyr_diagnostic.ps1
#
# This script will:
# 1. Check Zephyr environment variables
# 2. Verify the Zephyr workspace structure
# 3. Check project directory paths
# 4. Report any issues found
# PowerShell diagnostic script for Zephyr environment

# Output header
Write-Host "=== Zephyr Environment Diagnostic ===" -ForegroundColor Cyan
Write-Host "Current directory: $(Get-Location)" -ForegroundColor Yellow

# Check if west is installed and get its version
try {
    $westVersion = west --version 2>&1
    Write-Host "West version: $westVersion" -ForegroundColor Green
} catch {
    Write-Host "Error: West command not found or not accessible" -ForegroundColor Red
}

# Check environment variables related to Zephyr
Write-Host "`nChecking Zephyr environment variables:" -ForegroundColor Yellow
$envVars = @("ZEPHYR_BASE", "ZEPHYR_SDK_INSTALL_DIR")
foreach ($var in $envVars) {
    $value = [Environment]::GetEnvironmentVariable($var)
    if ($value) {
        Write-Host "$var = $value" -ForegroundColor Green
        
        # Check if the directory exists
        if (Test-Path $value) {
            Write-Host "  Directory exists: Yes" -ForegroundColor Green
        } else {
            Write-Host "  Directory exists: No" -ForegroundColor Red
        }
    } else {
        Write-Host "$var is not set" -ForegroundColor Red
    }
}

# Try to locate .west directory to find the workspace
Write-Host "`nSearching for Zephyr workspace..." -ForegroundColor Yellow
$currentPath = Get-Location
$foundWorkspace = $false

while ($currentPath -ne "") {
    $westDir = Join-Path -Path $currentPath -ChildPath ".west"
    if (Test-Path $westDir) {
        Write-Host "Found Zephyr workspace at: $currentPath" -ForegroundColor Green
        $foundWorkspace = $true
        break
    }
    
    # Move up one directory
    $parentPath = Split-Path -Path $currentPath -Parent
    if ($parentPath -eq $currentPath) {
        # We've reached the root
        break
    }
    $currentPath = $parentPath
}

if (-not $foundWorkspace) {
    Write-Host "Could not find a Zephyr workspace (no .west directory found in parent directories)" -ForegroundColor Red
}

# Check for tools/Zephyr directory structure
Write-Host "`nChecking project structure:" -ForegroundColor Yellow
$zephyrDir = Join-Path -Path (Get-Location) -ChildPath "tools\Zephyr"
if (Test-Path $zephyrDir) {
    Write-Host "tools\Zephyr directory exists" -ForegroundColor Green
    
    # Check for zephyr directory inside tools/Zephyr
    $zephyrCodeDir = Join-Path -Path $zephyrDir -ChildPath "zephyr"
    if (Test-Path $zephyrCodeDir) {
        Write-Host "tools\Zephyr\zephyr directory exists (Zephyr RTOS code)" -ForegroundColor Green
    } else {
        Write-Host "tools\Zephyr\zephyr directory not found" -ForegroundColor Red
    }
    
    # Check for app directory 
    $appDir = Join-Path -Path $zephyrDir -ChildPath "stm32h573i_dk_app"
    if (Test-Path $appDir) {
        Write-Host "tools\Zephyr\stm32h573i_dk_app directory exists (Application code)" -ForegroundColor Green
    } else {
        Write-Host "tools\Zephyr\stm32h573i_dk_app directory not found" -ForegroundColor Red
    }
} else {
    Write-Host "tools\Zephyr directory not found" -ForegroundColor Red
}

# Look for any .west directories in the current path or below
Write-Host "`nSearching for any .west directories in current directory or subdirectories:" -ForegroundColor Yellow
$westDirs = Get-ChildItem -Path (Get-Location) -Filter ".west" -Directory -Recurse -ErrorAction SilentlyContinue
if ($westDirs.Count -gt 0) {
    foreach ($dir in $westDirs) {
        Write-Host "Found .west directory at: $($dir.FullName)" -ForegroundColor Green
    }
} else {
    Write-Host "No .west directories found in current directory or subdirectories" -ForegroundColor Red
}

Write-Host "`n=== End of Diagnostic ===" -ForegroundColor Cyan


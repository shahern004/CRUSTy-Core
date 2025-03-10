# Cleanup script for after renaming from "Shahern004 Github" to "shahern004_github"
# Run this script from the CRUSTy-Core directory after completing the rename

# Print current working directory for verification
Write-Output "Current working directory: $(Get-Location)"

# Verify the rename was successful
if (-not (Test-Path "C:\shahern004_github")) {
    Write-Error "The directory 'C:\shahern004_github' does not exist. Please rename the directory first."
    exit 1
}

# 1. Clean up Zephyr cache
Write-Output "Cleaning Zephyr cache..."
$zephyrCachePath = "tools\Zephyr\zephyr\.cache"
if (Test-Path $zephyrCachePath) {
    Remove-Item -Recurse -Force $zephyrCachePath
    Write-Output "Zephyr cache deleted."
} else {
    Write-Output "Zephyr cache not found. Skipping."
}

# 2. Clean up build directories
Write-Output "Cleaning build directories..."
$buildDirs = @(
    "tools\Zephyr\stm32h573i_dk_app\build"
)

foreach ($dir in $buildDirs) {
    if (Test-Path $dir) {
        Remove-Item -Recurse -Force $dir
        Write-Output "Removed build directory: $dir"
    } else {
        Write-Output "Build directory not found: $dir. Skipping."
    }
}

# 3. Verify that new path structure is recognized
Write-Output "Verifying path structure..."
if (Get-Location -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path | Where-Object { $_ -like "*Shahern004 Github*" }) {
    Write-Warning "Path still contains 'Shahern004 Github'. This may indicate the rename wasn't fully successful."
} else {
    Write-Output "Path structure looks good. Directory rename was successful."
}

Write-Output "Cleanup complete. You should now be able to rebuild the project with the new path structure."
Write-Output "To test building the Zephyr app, run: cd tools\Zephyr\stm32h573i_dk_app; west build -b stm32h573i_dk"

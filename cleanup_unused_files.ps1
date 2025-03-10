 # Powershell script to clean up unused files in the repository
# Run this script from the root of the repository

# First, let's remove unused build directories and modules
$unusedDirs = @(
    "tools/Zephyr/build_absolute_path",
    "tools/Zephyr/build_minimal_cli", 
    "tools/Zephyr/build_minimal_working",
    "tools/Zephyr/build_qemu_cli",
    "tools/Zephyr/build_qemu_final",
    "tools/Zephyr/gpio_qemu_test",
    "tools/Zephyr/QEMU Reference"
)

Write-Host "Removing unused directories..." -ForegroundColor Yellow

foreach ($dir in $unusedDirs) {
    if (Test-Path $dir) {
        Write-Host "Removing $dir"
        Remove-Item -Path $dir -Recurse -Force -ErrorAction SilentlyContinue
    } else {
        Write-Host "Directory $dir already removed or does not exist"
    }
}

# Remove any build directories in STM32 app except the main ones
$buildDirs = Get-ChildItem -Path "tools/Zephyr/stm32h573i_dk_app" -Directory | Where-Object { $_.Name -like "build_*" }
foreach ($dir in $buildDirs) {
    Write-Host "Removing $($dir.FullName)"
    Remove-Item -Path $dir.FullName -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "Cleanup complete!" -ForegroundColor Green
Write-Host "Note: Modules like zephyr/ and modules/ will be excluded by .gitignore" -ForegroundColor Cyan

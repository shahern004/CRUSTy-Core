# Simple MSVC-specific build script for CRUSTy-Core
# This works around linking issues with the Rust libraries

$ErrorActionPreference = "Stop"

# Set build directory
$BUILD_DIR = "out/build/msvc-direct"

# Create the build directory
New-Item -ItemType Directory -Path $BUILD_DIR -Force | Out-Null

# Get the full paths
$BUILD_PATH = Join-Path $PWD $BUILD_DIR
$INSTALL_PATH = Join-Path $PWD "out/install/msvc-direct"

try {
    # Generate build files
    Write-Host "Generating build files with CMake..." -ForegroundColor Cyan
    cmake -G "Visual Studio 17 2022" -A x64 `
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" `
        -B "$BUILD_PATH" .

    # Build the project
    Write-Host "Building project..." -ForegroundColor Cyan
    cmake --build "$BUILD_PATH" --config Debug

    # Display build result
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build completed successfully!" -ForegroundColor Green
    } else {
        Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    }
} catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
}

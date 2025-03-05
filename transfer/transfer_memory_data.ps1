# Memory Knowledge Graph Transfer Script
# This script helps transfer Claude's memory knowledge graph between computers

# Define paths
$backupPath = "$PSScriptRoot\claude_memory_backup.tar"

Write-Host "Claude Memory Knowledge Graph Transfer Tool" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host ""

# Function to check if Docker is running
function Test-DockerRunning {
    try {
        $dockerInfo = docker info 2>&1
        return $true
    }
    catch {
        return $false
    }
}

# Check if Docker is running
if (-not (Test-DockerRunning)) {
    Write-Host "Error: Docker is not running. Please start Docker and try again." -ForegroundColor Red
    exit 1
}

# Menu for operations
function Show-Menu {
    Write-Host "Select an operation:" -ForegroundColor Yellow
    Write-Host "1. Export memory data from this computer" -ForegroundColor Yellow
    Write-Host "2. Import memory data to this computer" -ForegroundColor Yellow
    Write-Host "3. Exit" -ForegroundColor Yellow
    
    $choice = Read-Host "Enter your choice (1-3)"
    return $choice
}

# Export memory data
function Export-MemoryData {
    Write-Host "`nExporting memory data from Docker volume 'claude-memory'..." -ForegroundColor Cyan
    
    # Check if the volume exists
    $volumeExists = docker volume ls | Select-String -Pattern "claude-memory"
    if (-not $volumeExists) {
        Write-Host "Error: Docker volume 'claude-memory' does not exist." -ForegroundColor Red
        return
    }
    
    # Create a temporary container to access the volume
    Write-Host "Creating temporary container to access the volume..." -ForegroundColor Cyan
    docker run --rm -v claude-memory:/data -v ${PWD}:/backup alpine sh -c "tar -cf /backup/claude_memory_backup.tar -C /data ."
    
    if (Test-Path $backupPath) {
        Write-Host "Memory data exported successfully to $backupPath" -ForegroundColor Green
        Write-Host "`nInstructions:" -ForegroundColor Yellow
        Write-Host "1. Copy $backupPath to your target computer" -ForegroundColor Yellow
        Write-Host "2. Run this script on the target computer and select option 2 to import" -ForegroundColor Yellow
    }
    else {
        Write-Host "Error: Failed to export memory data." -ForegroundColor Red
    }
}

# Import memory data
function Import-MemoryData {
    if (-not (Test-Path $backupPath)) {
        Write-Host "Error: Backup file $backupPath not found." -ForegroundColor Red
        Write-Host "Please make sure to copy the backup file to this directory first." -ForegroundColor Yellow
        return
    }
    
    Write-Host "`nImporting memory data to Docker volume 'claude-memory'..." -ForegroundColor Cyan
    
    # Check if the volume exists, create it if not
    $volumeExists = docker volume ls | Select-String -Pattern "claude-memory"
    if (-not $volumeExists) {
        Write-Host "Creating Docker volume 'claude-memory'..." -ForegroundColor Cyan
        docker volume create claude-memory
    }
    
    # Create a temporary container to access the volume and import data
    Write-Host "Creating temporary container to access the volume and import data..." -ForegroundColor Cyan
    docker run --rm -v claude-memory:/data -v ${PWD}:/backup alpine sh -c "rm -rf /data/* && tar -xf /backup/claude_memory_backup.tar -C /data"
    
    Write-Host "Memory data imported successfully!" -ForegroundColor Green
    Write-Host "`nNotes:" -ForegroundColor Yellow
    Write-Host "- Restart VSCode to ensure the memory data is recognized" -ForegroundColor Yellow
    Write-Host "- You may need to restart the memory MCP server if it was already running" -ForegroundColor Yellow
}

# Main script logic
$choice = Show-Menu

switch ($choice) {
    "1" { Export-MemoryData }
    "2" { Import-MemoryData }
    "3" { Write-Host "Exiting..." -ForegroundColor Cyan; exit 0 }
    default { Write-Host "Invalid choice. Please enter a number between 1 and 3." -ForegroundColor Red }
}

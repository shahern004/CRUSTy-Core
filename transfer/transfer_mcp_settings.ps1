# MCP Settings Transfer Script
# This script helps transfer MCP server settings between computers

# Define paths
$sourceSettingsPath = "$env:APPDATA\Code\User\globalStorage\saoudrizwan.claude-dev\settings\cline_mcp_settings.json"
$backupSettingsPath = "$PSScriptRoot\mcp_settings_backup.json"

# Function to replace paths in the settings
function Replace-Paths {
    param (
        [string]$settingsContent,
        [string]$oldProjectPath,
        [string]$newProjectPath,
        [string]$oldUserPath,
        [string]$newUserPath
    )
    
    $content = $settingsContent
    $content = $content -replace [regex]::Escape($oldProjectPath), $newProjectPath
    $content = $content -replace [regex]::Escape($oldUserPath), $newUserPath
    
    return $content
}

# Step 1: Backup current settings
Write-Host "Backing up current MCP settings to $backupSettingsPath" -ForegroundColor Green
Copy-Item -Path $sourceSettingsPath -Destination $backupSettingsPath -Force

# Step 2: Create a modified version with placeholders for the target computer
$settingsContent = Get-Content -Path $sourceSettingsPath -Raw
$modifiedSettingsPath = "$PSScriptRoot\mcp_settings_modified.json"

# Current paths that need to be replaced
$currentProjectPath = "c:/Software Projects/CRUSTy-Core"
$currentUserPath = "C:/Users/Shawn_Work/Documents/Cline/MCP"

Write-Host "Creating modified settings file with placeholders at $modifiedSettingsPath" -ForegroundColor Green
$settingsContent | Out-File -FilePath $modifiedSettingsPath -Force

Write-Host "`nInstructions for transferring settings:" -ForegroundColor Yellow
Write-Host "1. Copy both files to your target computer:" -ForegroundColor Yellow
Write-Host "   - $backupSettingsPath (Original backup)" -ForegroundColor Yellow
Write-Host "   - $modifiedSettingsPath (Template for modification)" -ForegroundColor Yellow
Write-Host "`n2. On the target computer, run the following PowerShell commands:" -ForegroundColor Yellow
Write-Host "   # Define your paths on the target computer" -ForegroundColor Cyan
Write-Host "   `$targetProjectPath = 'c:/path/to/CRUSTy-Core'  # Update this path" -ForegroundColor Cyan
Write-Host "   `$targetUserPath = 'C:/Users/YourUsername/Documents/Cline/MCP'  # Update this path" -ForegroundColor Cyan
Write-Host "   `$settingsContent = Get-Content -Path 'mcp_settings_modified.json' -Raw" -ForegroundColor Cyan
Write-Host "   `$settingsContent = `$settingsContent -replace 'c:/Software Projects/CRUSTy-Core', `$targetProjectPath" -ForegroundColor Cyan
Write-Host "   `$settingsContent = `$settingsContent -replace 'C:/Users/Shawn_Work/Documents/Cline/MCP', `$targetUserPath" -ForegroundColor Cyan
Write-Host "   `$targetSettingsPath = '`$env:APPDATA\Code\User\globalStorage\saoudrizwan.claude-dev\settings\cline_mcp_settings.json'" -ForegroundColor Cyan
Write-Host "   New-Item -Path (Split-Path `$targetSettingsPath -Parent) -ItemType Directory -Force" -ForegroundColor Cyan
Write-Host "   `$settingsContent | Out-File -FilePath `$targetSettingsPath -Force" -ForegroundColor Cyan
Write-Host "   Write-Host 'MCP settings installed successfully!' -ForegroundColor Green" -ForegroundColor Cyan

Write-Host "`nNotes:" -ForegroundColor Yellow
Write-Host "- Make sure Docker is running on the target computer" -ForegroundColor Yellow
Write-Host "- Ensure all required Docker images (mcp/git, mcp/filesystem, etc.) are available" -ForegroundColor Yellow
Write-Host "- Restart VSCode after applying the settings" -ForegroundColor Yellow
Write-Host "- The claude-memory volume will need to be created separately if you want to transfer memory data" -ForegroundColor Yellow

# Create a separate script for the target computer
$targetScriptPath = "$PSScriptRoot\install_mcp_settings.ps1"
@"
# MCP Settings Installation Script
# Run this on the target computer to install the MCP settings

# Define your paths on the target computer
`$targetProjectPath = "c:/path/to/CRUSTy-Core"  # Update this path
`$targetUserPath = "C:/Users/YourUsername/Documents/Cline/MCP"  # Update this path

# Source settings file (should be in the same directory as this script)
`$sourceSettingsPath = "`$PSScriptRoot\mcp_settings_modified.json"
`$targetSettingsPath = "`$env:APPDATA\Code\User\globalStorage\saoudrizwan.claude-dev\settings\cline_mcp_settings.json"

# Read the settings content
`$settingsContent = Get-Content -Path `$sourceSettingsPath -Raw

# Replace paths
`$settingsContent = `$settingsContent -replace 'c:/Software Projects/CRUSTy-Core', `$targetProjectPath
`$settingsContent = `$settingsContent -replace 'C:/Users/Shawn_Work/Documents/Cline/MCP', `$targetUserPath

# Ensure the target directory exists
New-Item -Path (Split-Path `$targetSettingsPath -Parent) -ItemType Directory -Force -ErrorAction SilentlyContinue

# Write the modified settings to the target path
`$settingsContent | Out-File -FilePath `$targetSettingsPath -Force

Write-Host "MCP settings installed successfully!" -ForegroundColor Green
Write-Host "Please restart VSCode to apply the new settings." -ForegroundColor Yellow
"@ | Out-File -FilePath $targetScriptPath -Force

Write-Host "`nCreated installation script for the target computer at $targetScriptPath" -ForegroundColor Green
Write-Host "Copy both the modified settings file and this script to your target computer." -ForegroundColor Green

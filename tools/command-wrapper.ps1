<#
.SYNOPSIS
    Wrapper script to execute commands and capture their output to files.

.DESCRIPTION
    This script executes a command and captures its output to a file, then displays the output.
    It helps work around issues where terminal output might not be properly captured by Cline.

.PARAMETER Command
    The command to execute.

.PARAMETER OutputFile
    The file to save the command output to. If not specified, a timestamped file in the temp directory will be used.

.PARAMETER ShowOutput
    If specified, the command output will be displayed in the console as well as saved to the file.

.EXAMPLE
    .\command-wrapper.ps1 -Command "west build -b qemu_cortex_m3"
    
.EXAMPLE
    .\command-wrapper.ps1 -Command "west build -b qemu_cortex_m3" -OutputFile "build-output.log" -ShowOutput
#>

param (
    [Parameter(Mandatory=$true)]
    [string]$Command,
    
    [Parameter(Mandatory=$false)]
    [string]$OutputFile,
    
    [Parameter(Mandatory=$false)]
    [switch]$ShowOutput
)

# Script configuration
$ErrorActionPreference = "Stop"

# If no output file is specified, create a timestamped one in the temp directory
if (-not $OutputFile) {
    $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $OutputFile = Join-Path $env:TEMP "command-output-$timestamp.log"
}

# Ensure the output file path exists
$outputDir = Split-Path -Parent $OutputFile
if (-not (Test-Path $outputDir)) {
    New-Item -Path $outputDir -ItemType Directory -Force | Out-Null
}

Write-Host "Executing command: $Command" -ForegroundColor Cyan
Write-Host "Output will be saved to: $OutputFile" -ForegroundColor Cyan

try {
    # Execute the command and capture output to the file
    $output = Invoke-Expression $Command 2>&1 | Tee-Object -FilePath $OutputFile
    
    # Display the output if requested
    if ($ShowOutput) {
        Write-Host "`nCommand Output:" -ForegroundColor Green
        $output | ForEach-Object { Write-Host $_ }
    }
    
    Write-Host "`nCommand executed successfully." -ForegroundColor Green
    Write-Host "Output saved to: $OutputFile" -ForegroundColor Green
    
    # Return the path to the output file
    return $OutputFile
}
catch {
    Write-Host "Error executing command: $_" -ForegroundColor Red
    Write-Host "Error details saved to: $OutputFile" -ForegroundColor Red
    $_ | Out-File -Append -FilePath $OutputFile
    throw $_
}

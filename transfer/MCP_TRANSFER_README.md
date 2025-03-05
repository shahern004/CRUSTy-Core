# MCP Server Settings and Memory Transfer Guide

This guide explains how to transfer your MCP server settings and memory knowledge graph between Windows 11 computers.

## Files Included

1. `transfer_mcp_settings.ps1` - Script to transfer MCP server configurations
2. `transfer_memory_data.ps1` - Script to transfer memory knowledge graph data
3. `MCP_TRANSFER_README.md` - This guide

## Prerequisites

On both source and target computers:

- Windows 11
- PowerShell
- Docker Desktop installed and running
- VSCode with Claude extension installed
- Required Docker images (mcp/git, mcp/filesystem, mcp/memory, etc.)

## Transferring MCP Server Settings

### On the Source Computer

1. Run the `transfer_mcp_settings.ps1` script in PowerShell:
   ```powershell
   .\transfer_mcp_settings.ps1
   ```

2. The script will:
   - Back up your current MCP settings to `mcp_settings_backup.json`
   - Create a modified settings file at `mcp_settings_modified.json`
   - Create an installation script for the target computer at `install_mcp_settings.ps1`

3. Copy the following files to your target computer (via USB drive, network share, or cloud storage):
   - `mcp_settings_modified.json`
   - `install_mcp_settings.ps1`

### On the Target Computer

1. Place the copied files in a directory of your choice

2. Edit the `install_mcp_settings.ps1` script to update the paths:
   - Set `$targetProjectPath` to the path of your CRUSTy-Core project on the target computer
   - Set `$targetUserPath` to the path where your MCP servers are located on the target computer

3. Run the installation script in PowerShell:
   ```powershell
   .\install_mcp_settings.ps1
   ```

4. Restart VSCode to apply the new settings

## Transferring Memory Knowledge Graph

### On the Source Computer

1. Run the `transfer_memory_data.ps1` script in PowerShell:
   ```powershell
   .\transfer_memory_data.ps1
   ```

2. Select option 1 to export memory data
   - The script will create a backup file named `claude_memory_backup.tar`

3. Copy the `claude_memory_backup.tar` file to your target computer

### On the Target Computer

1. Place the `claude_memory_backup.tar` file in the same directory as the `transfer_memory_data.ps1` script

2. Run the script in PowerShell:
   ```powershell
   .\transfer_memory_data.ps1
   ```

3. Select option 2 to import memory data

4. Restart VSCode to ensure the memory data is recognized

## Troubleshooting

### Docker Issues

- Ensure Docker Desktop is running on both computers
- Verify that all required Docker images are available (run `docker images` to check)
- If you encounter permission issues, try running PowerShell as Administrator

### Path Issues

- Double-check that all paths in the installation script match your actual directory structure
- Windows paths can use either forward slashes (/) or backslashes (\\)
- Ensure the target directories exist before running the scripts

### VSCode Issues

- If settings aren't applied after restart, check the VSCode logs for errors
- Verify that the Claude extension is installed and activated
- Check that the settings file was correctly created in the appropriate directory

## Additional Notes

- The memory knowledge graph is stored in a Docker volume named `claude-memory`
- MCP server configurations are stored in the VSCode Claude extension settings
- You may need to adjust Docker volume permissions on the target computer
- If you have custom MCP servers, ensure they are properly configured on the target computer

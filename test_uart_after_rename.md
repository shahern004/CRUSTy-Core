# Testing UART Integration After Directory Rename

This document provides instructions for testing the UART integration model once the directory has been renamed from `C:\Shahern004 Github` to `C:\shahern004_github`.

## Prerequisites

1. Complete the directory rename as instructed
2. Run the `cleanup_after_rename.ps1` script to clean up cached paths
3. Ensure that the VSCode environment has been reloaded

## Building the Project

```powershell
# Navigate to the Zephyr application directory
cd tools\Zephyr\stm32h573i_dk_app

# Clean build the project with the STM32H573I-DK board target
west build -b stm32h573i_dk -p
```

## Testing the UART Integration

### Hardware Setup

1. Connect the STM32H573I-DK board via USB to your computer
2. Ensure the correct USB port is detected for the serial communication
3. Connect any additional hardware if needed (LEDs, buttons, etc.)

### Using a Serial Terminal

To test the UART communication:

1. Open a serial terminal program (like PuTTY, TeraTerm, or the built-in serial monitor)
2. Configure the connection:

   - Port: Select the COM port for the STM32H573I-DK board
   - Baud rate: 115200
   - Data bits: 8
   - Parity: None
   - Stop bits: 1
   - Flow control: None

3. Press the reset button on the board to start the application
4. You should see a welcome message:
   ```
   ==================================
   CRUSTy-Core UART Demo
   Type HELP for available commands
   ==================================
   ```

### Testing Commands

Try the following commands to test the UART integration:

1. Type `HELP` to see available commands
2. LED Control: `LED <led_num> <0|1>` (e.g., `LED 0 1` to turn on LED 0)
3. Echo Test: `ECHO Hello World`
4. Press the user button on the board to trigger a status message

### Expected Behavior

- The board should respond to commands via the UART interface
- The LEDs should toggle when commanded
- The device should echo back text as requested
- The user button should trigger status messages

## Troubleshooting

If you encounter issues:

1. Verify the board is properly connected and powered
2. Check if the correct COM port is selected
3. Ensure the baud rate settings match (115200)
4. Try rebuilding the project with `-p` flag to ensure a clean build
5. Check the console output during the build for any errors

This testing confirms that the UART integration model works correctly after the directory rename, validating both the communication interface and the command processing functionality.

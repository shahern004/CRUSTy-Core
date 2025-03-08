/**
 * Minimal test for ARM toolchain
 */

// Simple function that adds two integers
int add(int a, int b) {
    return a + b;
}

// Entry point for bare-metal application
// Note: This won't actually run on the hardware without proper startup code
extern "C" void _start() {
    // Just a simple calculation to verify compilation
    volatile int result = add(2, 3);
    
    // Infinite loop to prevent returning
    while(1) {
        // Do nothing
    }
}

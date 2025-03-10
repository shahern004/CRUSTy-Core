/*
 * Minimal test application for QEMU CLI testing
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(minimal_app, CONFIG_LOG_DEFAULT_LEVEL);

/* CLI Test commands */
static int cmd_hello(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    shell_print(sh, "Hello from QEMU test app!");
    return 0;
}

static int cmd_crypto_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    shell_print(sh, "Crypto hardware capabilities (QEMU simulation):");
    shell_print(sh, "  AES hardware acceleration: Not available");
    shell_print(sh, "  Random number generator:   Software");
    shell_print(sh, "  SHA hardware acceleration: Not available");
    shell_print(sh, "  Public key accelerator:    Not available");
    
    return 0;
}

static int cmd_crypto_random(const struct shell *sh, size_t argc, char **argv)
{
    size_t len = 16;  /* Default to 16 bytes */
    uint8_t buffer[64];
    char hex_str[129];  /* 64 bytes * 2 chars per byte + null terminator */
    
    if (argc > 1) {
        len = atoi(argv[1]);
        if (len == 0 || len > sizeof(buffer)) {
            shell_error(sh, "Invalid length. Must be between 1 and %u", sizeof(buffer));
            return -1;
        }
    }
    
    /* Simple random number generation for demo */
    for (size_t i = 0; i < len; i++) {
        buffer[i] = (uint8_t)(k_uptime_get_32() & 0xFF);
        k_busy_wait(1000);  /* Small delay to change the value */
    }
    
    /* Convert to hex string */
    for (size_t i = 0, j = 0; i < len && j < sizeof(hex_str) - 1; i++) {
        static const char hex_chars[] = "0123456789ABCDEF";
        hex_str[j++] = hex_chars[(buffer[i] >> 4) & 0xF];
        hex_str[j++] = hex_chars[buffer[i] & 0xF];
    }
    hex_str[len * 2] = '\0';
    
    shell_print(sh, "Random bytes (%u): %s", (unsigned int)len, hex_str);
    
    return 0;
}

/* Create subcommand sets */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_crypto,
    SHELL_CMD_ARG(status, NULL, "Show crypto hardware status", cmd_crypto_status, 1, 0),
    SHELL_CMD_ARG(random, NULL, "Generate random bytes: random [length]", cmd_crypto_random, 1, 1),
    SHELL_SUBCMD_SET_END
);

/* Register the main "crypto" command */
SHELL_CMD_REGISTER(crypto, &sub_crypto, "Crypto operations", NULL);

/* Also register a simple "hello" command */
SHELL_CMD_REGISTER(hello, NULL, "Say hello from test app", cmd_hello);

void main(void)
{
    LOG_INF("Minimal QEMU Test Application");
    LOG_INF("Board: %s", CONFIG_BOARD);
    LOG_INF("Type 'help' to see available commands");
    LOG_INF("Try 'hello' and 'crypto' commands to test the CLI");
}

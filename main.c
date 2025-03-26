#include <stdio.h>
#include <stdint.h>
#include "include/my_rust_lib.h"

int main() {
    uint32_t result = rust_function_blank();
    printf("Result from rust_function_blank() -> u32: %u\n", result);
    return 0;
}
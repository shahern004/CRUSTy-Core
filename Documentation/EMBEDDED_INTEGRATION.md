# Embedded Architecture: Combining Rust and C++ in a Single Binary

CRUSTy-Core's embedded implementation demonstrates a powerful approach to combining Rust and C++ code within a single cortex processor. This section details the architecture from the perspective of an embedded system providing encryption services to other hosts.

## Integration Architecture

The embedded implementation runs on STM32H573I-DK hardware, leveraging the strengths of both languages:

- **Rust** handles all input validation, sanitization, and memory-sensitive operations
- **C++** manages core processing, hardware acceleration, and system integration

Both languages are compiled into a single binary through a sophisticated build pipeline that ensures secure and efficient interoperation.

```
┌─────────────────────────────────────────────────────────┐
│                  Single Binary Image                    │
│                                                         │
│  ┌─────────────────┐           ┌─────────────────────┐  │
│  │                 │           │                     │  │
│  │   Rust Code     │◄─────────►│     C++ Code        │  │
│  │  (Input Safety) │    FFI    │  (Core Processing)  │  │
│  │                 │ Boundary  │                     │  │
│  └────────┬────────┘           └──────────┬──────────┘  │
│           │                               │             │
│  ┌────────▼────────┐           ┌──────────▼──────────┐  │
│  │  Memory-Safe    │           │   Hardware Access   │  │
│  │  Input Handling │           │   & Acceleration    │  │
│  └─────────────────┘           └─────────────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

## Build Process and Tools

The integration of Rust and C++ code into a single binary involves several specialized tools:

1. **CMake** - Primary build orchestrator that manages the overall build process
2. **Corrosion** - CMake module that integrates Rust's Cargo build system with CMake
3. **cbindgen** - Automatically generates C/C++ headers from Rust code
4. **Cargo** - Rust's package manager and build system
5. **ARM GCC** - C/C++ compiler for the ARM Cortex-M7 target
6. **LLVM** - Backend for Rust compilation to ARM target

The build process follows these steps:

1. Rust code is analyzed by cbindgen to generate C-compatible header files
2. Cargo compiles Rust code to a static library (.a) for the ARM target
3. C++ code is compiled with the generated headers
4. The linker combines both into a single binary
5. The binary is flashed to the STM32H573I-DK hardware

## Memory Management and FFI

Memory management between Rust and C++ is handled through a carefully designed Foreign Function Interface (FFI):

- **Zero-copy data transfer** - Data is passed by reference across the language boundary
- **Explicit ownership** - Clear rules for which language owns memory at each point
- **Safe abstractions** - Unsafe Rust code is contained in small, well-tested modules
- **Error propagation** - Errors are safely translated between languages
- **Resource cleanup** - RAII patterns in both languages ensure proper resource management

Example of the FFI boundary:

```rust
// Rust side
#[no_mangle]
pub unsafe extern "C" fn process_input(
    data: *const u8, 
    len: usize, 
    output: *mut u8, 
    output_len: *mut usize
) -> i32 {
    // Convert raw pointers to Rust slices
    let input = std::slice::from_raw_parts(data, len);
    
    // Process with Rust's memory safety
    match validate_and_process(input) {
        Ok(result) => {
            // Copy result to output buffer
            if result.len() > unsafe { *output_len } {
                unsafe { *output_len = result.len(); }
                return ERROR_BUFFER_TOO_SMALL;
            }
            
            let out_slice = unsafe { std::slice::from_raw_parts_mut(output, *output_len) };
            out_slice[..result.len()].copy_from_slice(&result);
            unsafe { *output_len = result.len(); }
            SUCCESS
        },
        Err(e) => e.to_error_code()
    }
}
```

```cpp
// C++ side
extern "C" {
    int32_t process_input(
        const uint8_t* data, 
        size_t len, 
        uint8_t* output, 
        size_t* output_len
    );
}

bool CppProcessor::processData(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> output(MAX_OUTPUT_SIZE);
    size_t output_len = output.size();
    
    int32_t result = process_input(
        input.data(), 
        input.size(), 
        output.data(), 
        &output_len
    );
    
    if (result == SUCCESS) {
        output.resize(output_len);
        // Use the processed data
        return true;
    } else {
        // Handle error
        return false;
    }
}
```

## Hardware Acceleration

The embedded implementation leverages STM32H5's hardware cryptographic accelerators:

- **AES-GCM Hardware Engine** - Accelerates encryption/decryption operations
- **SHA Hardware Accelerator** - Speeds up hashing operations
- **True Random Number Generator** - Provides high-quality entropy for key generation
- **PKA (Public Key Accelerator)** - Accelerates asymmetric cryptography operations

C++ code interfaces with these hardware accelerators through the STM32CubeH5 HAL:

```cpp
bool initHardwareAES(AES_HandleTypeDef* haes, const uint8_t* key) {
    haes->Instance = AES;
    haes->Init.DataType = AES_DATATYPE_8B;
    haes->Init.KeySize = AES_KEYSIZE_256;
    haes->Init.OperatingMode = AES_MODE_GCM;
    haes->Init.ChainingMode = AES_CHAINMODE_AES_GCM;
    haes->Init.KeyWriteFlag = AES_KEY_WRITE_ENABLE;
    
    if (HAL_AES_Init(haes) != HAL_OK) {
        return false;
    }
    
    return HAL_AES_SetKey(haes, (uint32_t*)key, AES_KEYSIZE_256) == HAL_OK;
}
```

## Conditional Compilation

The codebase uses conditional compilation to support both PC and embedded targets:

```rust
// Rust conditional compilation
#[cfg(feature = "embedded")]
pub fn get_random_bytes(output: &mut [u8]) -> Result<(), Error> {
    // Use hardware RNG on embedded target
    extern "C" {
        fn hw_get_random_bytes(buffer: *mut u8, len: usize) -> i32;
    }
    
    let result = unsafe { hw_get_random_bytes(output.as_mut_ptr(), output.len()) };
    if result == 0 {
        Ok(())
    } else {
        Err(Error::RngFailure)
    }
}

#[cfg(not(feature = "embedded"))]
pub fn get_random_bytes(output: &mut [u8]) -> Result<(), Error> {
    // Use software RNG on PC target
    use rand::RngCore;
    rand::thread_rng().fill_bytes(output);
    Ok(())
}
```

```cpp
// C++ conditional compilation
#ifdef EMBEDDED_TARGET
void initCrypto() {
    // Initialize hardware crypto
    MX_AES_Init();
    MX_HASH_Init();
    MX_RNG_Init();
}
#else
void initCrypto() {
    // Initialize software crypto
    initOpenSSL();
}
#endif
```

## Developer Considerations for Embedded Adaptation

For developers adapting this architecture to their own embedded systems:

### Memory Constraints

- **Stack Usage**: Carefully monitor stack usage in both languages
- **Heap Allocation**: Minimize or eliminate heap allocations in critical paths
- **Static Allocation**: Use static buffers where possible for predictable memory usage
- **Memory Pools**: Consider memory pools for dynamic allocations with bounded size

```cpp
// Example of static allocation for buffers
static uint8_t inputBuffer[MAX_INPUT_SIZE];
static uint8_t outputBuffer[MAX_OUTPUT_SIZE];

// Instead of:
// std::vector<uint8_t> inputBuffer(size);
// std::vector<uint8_t> outputBuffer(size);
```

### Performance Optimization

- **Zero-Copy**: Design interfaces to avoid unnecessary copying
- **DMA Integration**: Use DMA for efficient data transfer to/from hardware accelerators
- **Interrupt Handling**: Process data in chunks to maintain responsiveness
- **Power Management**: Implement sleep modes between processing tasks

```cpp
// Example of DMA usage with hardware crypto
void encryptWithDMA(const uint8_t* input, size_t len, uint8_t* output) {
    // Configure DMA for input
    HAL_DMA_Start(&hdma_aes_in, (uint32_t)input, (uint32_t)&AES->DINR, len/4);
    
    // Configure DMA for output
    HAL_DMA_Start(&hdma_aes_out, (uint32_t)&AES->DOUTR, (uint32_t)output, len/4);
    
    // Enable DMA mode
    SET_BIT(AES->CR, AES_CR_DMAOUTEN | AES_CR_DMAINEN);
    
    // Start encryption
    SET_BIT(AES->CR, AES_CR_EN);
    
    // Wait for completion
    while (HAL_DMA_GetState(&hdma_aes_out) != HAL_DMA_STATE_READY) {
        // Could yield to other tasks here
    }
}
```

### Security Considerations

- **Secure Boot**: Implement secure boot to verify firmware integrity
- **Memory Protection**: Use MPU to isolate sensitive operations
- **Side-Channel Protection**: Implement countermeasures against timing and power analysis
- **Key Management**: Store keys in secure memory regions or secure elements
- **Debug Protection**: Disable debug interfaces in production

```cpp
// Example of MPU configuration for secure memory regions
void configureMPU() {
    MPU_Region_InitTypeDef MPU_InitStruct;
    
    // Disable MPU
    HAL_MPU_Disable();
    
    // Configure secure memory region
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = SECURE_MEMORY_BASE;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    // Enable MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
```

### Debugging and Testing

- **SEGGER J-Link**: Use advanced debugging tools for both C++ and Rust code
- **Unit Testing**: Implement unit tests that can run on both host and target
- **Fault Handlers**: Implement comprehensive fault handlers for diagnostics
- **Logging**: Use a lightweight logging system with configurable levels

```cpp
// Example of a fault handler
void HardFault_Handler(void) {
    // Capture fault status registers
    volatile uint32_t stacked_r0;
    volatile uint32_t stacked_r1;
    volatile uint32_t stacked_r2;
    volatile uint32_t stacked_r3;
    volatile uint32_t stacked_r12;
    volatile uint32_t stacked_lr;
    volatile uint32_t stacked_pc;
    volatile uint32_t stacked_psr;
    volatile uint32_t _CFSR;
    volatile uint32_t _HFSR;
    volatile uint32_t _DFSR;
    volatile uint32_t _AFSR;
    volatile uint32_t _BFAR;
    volatile uint32_t _MMAR;
    
    _CFSR = SCB->CFSR;
    _HFSR = SCB->HFSR;
    _DFSR = SCB->DFSR;
    _AFSR = SCB->AFSR;
    _BFAR = SCB->BFAR;
    _MMAR = SCB->MMFAR;
    
    __asm volatile (
        "TST LR, #4\n"
        "ITE EQ\n"
        "MRSEQ R0, MSP\n"
        "MRSNE R0, PSP\n"
        "MOV R1, R0\n"
        "LDR R0, [R1, #0x00]\n"
        "LDR R1, [R1, #0x04]\n"
        "LDR R2, [R1, #0x08]\n"
        "LDR R3, [R1, #0x0C]\n"
        "LDR R12, [R1, #0x10]\n"
        "LDR LR, [R1, #0x14]\n"
        "LDR PC, [R1, #0x18]\n"
        "LDR PSR, [R1, #0x1C]\n"
        "BX LR\n"
    );
    
    // Log fault information or store in non-volatile memory
    // Then trigger system reset
    NVIC_SystemReset();
}
```

### Firmware Updates

- **Dual Bank Flash**: Implement A/B firmware updates for reliability
- **Integrity Verification**: Verify firmware integrity before applying updates
- **Rollback Protection**: Prevent downgrade attacks
- **Update Protocol**: Design a secure protocol for receiving updates

```cpp
// Example of firmware update verification
bool verifyFirmwareUpdate(const uint8_t* firmware, size_t size, const uint8_t* signature) {
    // Hash the firmware
    uint8_t hash[32];
    HAL_HASH_SHA256_Start(&hhash, (uint8_t*)firmware, size, hash, HAL_MAX_DELAY);
    
    // Verify signature using PKA
    return verifySignature(hash, sizeof(hash), signature);
}
```

By following these guidelines, developers can successfully adapt the CRUSTy-Core architecture to their own embedded systems, leveraging the security benefits of Rust with the performance and hardware integration capabilities of C++.

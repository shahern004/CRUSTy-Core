# CRUSTY-Core System Design Document

**Author: Shawn Ahern**  
**Date: March 4, 2025**

## 1. Introduction

This document describes the system design for CRUSTY-Core, a secure file encryption application with dual functionality: a PC application with Qt UI and an embedded deployment on STM32H573I-DK hardware. The architecture leverages Rust for all input handling while maintaining the cryptographic core in C++, combining the memory safety benefits of Rust with the performance and compatibility of C++.

## 2. Architectural Overview

### 2.1 Design Philosophy

The CRUSTY-Core architecture is built on two key principles:

1. **Rust for Input Handling**: All functions that take input from users or other devices (network, input fields, etc.) are implemented in Rust to leverage its memory safety and security features.

2. **C++ for Cryptographic Core**: The core cryptographic operations are implemented in C++ for performance and compatibility with existing libraries and hardware acceleration.

### 2.2 High-Level Architecture

```mermaid
graph TD
    subgraph "CRUSTY-Core Architecture"
        subgraph "Rust Layer"
            A[User Input Handling]
            B[Network Input]
            C[Device Commands]
            D[Input Validation]
        end
        
        subgraph "C++ Layer"
            E[Cryptographic Core]
            F[File Operations]
            G[Application Logic]
        end
        
        subgraph "FFI Boundary"
            H[Rust-C++ Interface]
        end
        
        A --> D
        B --> D
        C --> D
        D --> H
        H --> E
        E --> F
        E --> G
    end
```

### 2.3 Dual Functionality

CRUSTY-Core supports two deployment targets:

```mermaid
graph TD
    subgraph "PC Deployment"
        A1[Qt GUI]
        B1[Local File System]
        C1[Local Encryption]
        D1[Embedded Device Management]
    end
    
    subgraph "Embedded Deployment"
        A2[STM32H573I-DK Hardware]
        B2[Hardware Acceleration]
        C2[Host Communication]
        D2[Embedded Encryption Service]
    end
    
    subgraph "Shared Components"
        E1[Rust Input Handling]
        F1[C++ Crypto Core]
        G1[Communication Protocol]
    end
    
    A1 --> E1
    E1 --> F1
    F1 --> C1
    D1 --> G1
    
    C2 --> E1
    F1 --> B2
    G1 --> C2
    B2 --> D2
```

## 3. Component Details

### 3.1 Rust Input Handling Layer

The Rust layer is responsible for processing all inputs from users, networks, and devices. This includes:

- GUI input validation
- Command parsing and validation
- Network protocol handling
- File data validation
- Error handling and reporting

**Example: Input Validation in Rust**

```rust
pub fn validate_user_input(input: &str) -> Result<ValidatedInput, ValidationError> {
    // Check for empty input
    if input.is_empty() {
        return Err(ValidationError::EmptyInput);
    }
    
    // Check for maximum length
    if input.len() > MAX_INPUT_LENGTH {
        return Err(ValidationError::InputTooLong);
    }
    
    // Validate content
    if !input.chars().all(|c| c.is_alphanumeric() || c == '_') {
        return Err(ValidationError::InvalidCharacters);
    }
    
    Ok(ValidatedInput::new(input))
}
```

### 3.2 C++ Cryptographic Core

The C++ layer implements the core cryptographic operations, including:

- AES-256-GCM encryption and decryption
- Key derivation using Argon2
- Key management
- File operations
- Hardware acceleration interface (for embedded target)

**Example: C++ Encryption Function**

```cpp
bool Encryptor::encryptData(const uint8_t* data, size_t dataLen, 
                           const uint8_t* key, size_t keyLen,
                           uint8_t* output, size_t* outputLen) {
    // Initialize encryption context
    AES_GCM_Context ctx;
    if (!initContext(&ctx, key, keyLen)) {
        return false;
    }
    
    // Process data
    return processData(&ctx, data, dataLen, output, outputLen);
}
```

### 3.3 FFI Boundary

The FFI (Foreign Function Interface) boundary defines how Rust and C++ components communicate. It includes:

- Type-safe function signatures
- Memory ownership transfer rules
- Error propagation mechanisms
- Data validation at the boundary

**Example: FFI Interface Definition**

```rust
// Rust side of FFI
#[no_mangle]
pub extern "C" fn validate_and_encrypt(
    input_ptr: *const c_char,
    input_len: usize,
    key_ptr: *const c_char,
    key_len: usize,
    output_ptr: *mut u8,
    output_len: *mut usize
) -> c_int {
    // Convert C inputs to Rust
    let input = unsafe { slice::from_raw_parts(input_ptr as *const u8, input_len) };
    let key = unsafe { slice::from_raw_parts(key_ptr as *const u8, key_len) };
    
    // Validate input
    if let Err(_) = validate_input(input) {
        return ERROR_INVALID_INPUT;
    }
    
    // Call C++ encryption function
    unsafe {
        cpp_encrypt_data(input.as_ptr(), input.len(), 
                        key.as_ptr(), key.len(),
                        output_ptr, output_len)
    }
}
```

## 4. PC Deployment

### 4.1 Qt User Interface

The PC deployment includes a Qt-based user interface that provides:

- File selection for encryption/decryption
- Password input
- Progress reporting
- Embedded device management

All user inputs from the Qt UI are passed to the Rust layer for validation before being processed.

### 4.2 Local Encryption

The PC deployment supports local encryption using the C++ cryptographic core. This provides:

- File encryption/decryption on the local file system
- Password-based key derivation
- Progress reporting and error handling

### 4.3 Embedded Device Management

The PC application includes features for managing connected embedded devices:

- Device detection and connection
- Firmware installation and configuration
- Encryption service requests
- Key management

**Example: Embedded Device Detection**

```cpp
std::vector<DeviceInfo> EmbeddedDeviceManager::detectDevices() {
    std::vector<DeviceInfo> devices;
    
    // Scan for USB devices
    auto usbDevices = scanUsbDevices();
    for (const auto& dev : usbDevices) {
        if (isStm32h5Device(dev)) {
            devices.push_back(createDeviceInfo(dev));
        }
    }
    
    return devices;
}
```

## 5. Embedded Deployment

### 5.1 STM32H573I-DK Hardware

The embedded deployment runs on STM32H573I-DK hardware, which provides:

- ARM Cortex-M7 processor
- Hardware cryptographic accelerators
- Secure storage
- Communication interfaces (USB, UART)

### 5.2 Hardware Acceleration

The embedded deployment leverages the STM32H5's hardware cryptographic accelerators for improved performance:

- AES-GCM hardware acceleration
- SHA hardware acceleration
- True random number generation

**Example: Hardware Acceleration Interface**

```cpp
bool initHardwareAccelerator(EncryptionContext* context) {
    // Configure AES-GCM hardware
    HAL_CRYP_Init(&context->hcryp);
    
    // Set key
    if (HAL_CRYP_SetKey(&context->hcryp, context->key, context->keyLen) != HAL_OK) {
        return false;
    }
    
    // Set IV
    return HAL_CRYP_SetInitVector(&context->hcryp, context->iv, 12) == HAL_OK;
}
```

### 5.3 Host Communication

The embedded deployment communicates with the host PC through a secure protocol implemented in Rust:

- Command reception and parsing
- Response formatting
- Error handling
- Flow control

### 5.4 Embedded Encryption Service

The embedded device provides encryption services to the connected host:

- File encryption/decryption
- Key management
- Secure operations

## 6. Communication Protocol

### 6.1 Protocol Overview

The communication protocol between PC and embedded device is implemented in Rust and follows a command-response pattern:

```mermaid
sequenceDiagram
    participant PC
    participant Embedded
    
    PC->>Embedded: Command Message
    Note over PC,Embedded: Command ID, Parameters
    Embedded->>Embedded: Process Command
    Embedded->>PC: Response Message
    Note over PC,Embedded: Status, Result Data
```

### 6.2 Message Format

Messages follow a binary format with headers, payloads, and checksums:

```
Command Message:
[2 bytes: Magic] [1 byte: Type] [4 bytes: Length] [4 bytes: Command ID] [Variable: Payload] [2 bytes: CRC]

Response Message:
[2 bytes: Magic] [1 byte: Type] [4 bytes: Length] [4 bytes: Command ID] [1 byte: Status] [Variable: Payload] [2 bytes: CRC]
```

### 6.3 Command Processing

Commands are processed through a pipeline that ensures security at each step:

```mermaid
graph LR
    A[Receive Message] --> B[Validate Format]
    B --> C[Parse Command]
    C --> D[Validate Parameters]
    D --> E[Execute Command]
    E --> F[Format Response]
    F --> G[Send Response]
```

## 7. Build System

### 7.1 Unified Build System

The build system supports both PC and embedded targets from a single codebase:

- CMake as the primary build orchestrator
- Corrosion for Rust integration
- Conditional compilation for target-specific code

**Example: CMake Configuration for Dual Targets**

```cmake
# Common components
add_library(cpp_crypto_core
    src/cpp/crypto/encryptor.cpp
    src/cpp/crypto/key_management.cpp
)

# Conditional compilation for PC target
if(BUILD_PC_TARGET)
    add_executable(crusty_qt src/cpp/main.cpp)
    target_link_libraries(crusty_qt PRIVATE 
        cpp_crypto_core
        qt_ui
        rust_input_pc
    )
endif()

# Conditional compilation for embedded target
if(BUILD_EMBEDDED_TARGET)
    add_executable(crusty_embedded src/cpp/embedded_main.cpp)
    target_link_libraries(crusty_embedded PRIVATE 
        cpp_crypto_core
        rust_input_embedded
    )
endif()
```

### 7.2 Rust Conditional Compilation

Rust code uses feature flags for conditional compilation:

**Example: Rust Conditional Compilation**

```rust
#[cfg(feature = "std")]
pub fn pc_specific_function() {
    // PC-specific implementation using std library
    std::fs::File::open("config.txt").unwrap();
}

#[cfg(not(feature = "std"))]
pub fn embedded_specific_function() {
    // Embedded-specific implementation without std
    // Uses no_std compatible alternatives
}
```

## 8. Security Considerations

### 8.1 Memory Safety

Rust's ownership model ensures memory safety for all input handling:

- No buffer overflows
- No use-after-free vulnerabilities
- No data races
- Explicit error handling

### 8.2 Input Validation

All inputs are validated in Rust before processing:

- Type checking
- Range validation
- Format validation
- Sanitization

### 8.3 Secure Communication

Communication between PC and embedded device is secured:

- Message authentication
- Encryption for sensitive data
- Protection against replay attacks
- Error detection

### 8.4 Hardware Security

The STM32H5 provides hardware security features:

- Secure boot
- Secure storage
- Hardware isolation
- Side-channel attack countermeasures

## 9. Performance Considerations

### 9.1 Chunked Processing

Large files are processed in chunks to manage memory usage:

```mermaid
graph TD
    A[Input File] --> B[Split into Chunks]
    B --> C[Process Chunk 1]
    B --> D[Process Chunk 2]
    B --> E[Process Chunk N]
    C --> F[Combine Results]
    D --> F
    E --> F
    F --> G[Output File]
```

### 9.2 Hardware Acceleration

The embedded target leverages hardware acceleration for improved performance:

- AES-GCM hardware accelerator for encryption/decryption
- SHA hardware accelerator for hashing
- DMA for efficient data transfer

### 9.3 Parallel Processing

The PC target uses parallel processing for large files:

- Multiple threads for chunk processing
- Progress reporting across threads
- Synchronization for result combination

## 10. User Experience

### 10.1 PC Application

The PC application provides a seamless user experience:

- Intuitive file selection and operation
- Clear progress reporting
- Embedded device management
- Error reporting and recovery

### 10.2 Encryption Modes

Users can choose between local and embedded encryption:

```mermaid
graph TD
    A[User Selects File] --> B[Choose Encryption Mode]
    B --> C[Local Encryption]
    B --> D[Embedded Encryption]
    C --> E[Process Locally]
    D --> F[Select Device]
    F --> G[Send to Device]
    G --> H[Process on Device]
    E --> I[Save Encrypted File]
    H --> I
```

### 10.3 Embedded Installation

The PC application provides a simple interface for installing CRUSTY-Core on embedded devices:

- Device detection
- Firmware installation
- Configuration
- Verification

## 11. Conclusion

The CRUSTY-Core system design leverages the strengths of both Rust and C++ to create a secure, high-performance file encryption application with dual functionality. By using Rust for all input handling and C++ for the cryptographic core, the system achieves both memory safety and performance.

The dual-target architecture supports both PC and embedded deployments, providing flexibility and enhanced security options. The ability to offload encryption to a dedicated STM32H5 device provides additional security benefits for sensitive operations, while the local encryption option ensures convenience for everyday use.

## 12. Development Plan

### 12.1 Current Status Overview

The CRUSTy-Core project has established its core architecture and implemented basic encryption/decryption functionality. The hybrid C++/Rust approach is working well, with Rust handling cryptographic operations and C++ providing the application framework and UI. However, many UI features are defined but not fully implemented, and the embedded target implementation has not been started.

Key accomplishments:
- Core architecture with C++/Rust integration is implemented
- AES-256-GCM encryption with Argon2 key derivation is functional
- Basic Qt UI framework is in place
- File operations and encryption/decryption are working

Current challenges:
- Qt DLL dependency issue: Qt DLLs are not automatically copied to the build directory
- Many UI features are defined but show "not yet implemented" messages
- Embedded target implementation has not been started

### 12.2 Revised Development Phases

#### Phase 1: Project Setup and Core Architecture

**Current Status:** ✅ COMPLETED
- Project structure, build system, and interface definitions are in place
- FFI boundary between C++ and Rust is defined and functional
- CMake build system with Corrosion integration is working
- Basic Qt application shell is implemented

**Objectives:**
- Fork the CRUSTy repository and establish the new project structure
- Define the interface between C++ and Rust components
- Set up the dual-target build system for PC and embedded deployments
- Set up the basic Qt application framework

**Key Deliverables:**
- Project repository with initial structure and build system
- Interface definitions for cross-language communication
- Build configuration for both PC and STM32H5 targets
- Basic Qt application shell with placeholder UI

**Technical Approach:**
The FFI boundary will be carefully designed to ensure type safety and memory safety across language boundaries. The interface will include functions for data encryption/decryption, password hashing, and key derivation. A C++ wrapper class will provide a clean, object-oriented interface to the Rust input handling functions. The build system will support conditional compilation for both PC and embedded targets.

#### Phase 2A: Complete Basic UI Implementation

**Current Status:** ⚠️ PARTIALLY COMPLETED
- Basic encryption/decryption UI is implemented
- File operations are implemented but file browser functionality is incomplete
- Many UI features show "not yet implemented" messages
- Qt DLL dependency issue exists

**Objectives:**
- Complete the file browser functionality
- Implement batch processing for multiple files
- Implement settings dialog
- Implement key management
- Resolve Qt DLL dependency issue
- Remove unnecessary UI elements

**Key Deliverables:**
- Fully functional file browser with proper directory navigation
- Batch processing for encrypting/decrypting multiple files
- Settings dialog for configuring application preferences
- Key management interface for generating, saving, and loading encryption keys
- Deployment configuration to handle Qt DLL dependencies
- Clean UI without unnecessary elements

**Technical Approach:**
The file browser will be enhanced to support proper directory navigation and file selection. Batch processing will be implemented to handle multiple files with a single operation. The settings dialog will allow users to configure application preferences such as default directories and UI options. Key management will provide functionality for generating, saving, and loading encryption keys. A post-build step will be added to the CMake configuration to copy required Qt DLLs to the build directory. Unnecessary UI elements will be removed to simplify the interface.

#### Phase 2B: Implement Enhanced Security Features

**Current Status:** ⚠️ PARTIALLY IMPLEMENTED
- Basic security features are implemented
- Password strength meter is implemented but could be enhanced
- Key management is not implemented

**Objectives:**
- Enhance password strength requirements
- Implement secure key storage
- Add encryption metadata and verification

**Key Deliverables:**
- Enhanced password strength validation and feedback
- Secure key storage with proper encryption
- File metadata for verification and integrity checking

**Technical Approach:**
Password strength requirements will be enhanced with better validation and feedback. Secure key storage will use proper encryption for storing keys on disk. Encryption metadata will be added to encrypted files to support verification and integrity checking.

#### Phase 3: Dual-Target Implementation

**Current Status:** ❌ NOT STARTED
- Embedded target functionality is defined in the UI but not implemented
- Communication protocol between PC and embedded device is not implemented
- Hardware acceleration for cryptographic operations is not implemented

**Objectives:**
- Implement the embedded firmware for STM32H573I-DK
- Develop the communication protocol between PC and embedded device
- Implement hardware acceleration for cryptographic operations
- Add "Install CRUSTy-Core on Embedded System" feature to PC application

**Key Deliverables:**
- Functional embedded firmware for STM32H573I-DK
- Secure communication protocol between PC and embedded device
- Hardware-accelerated encryption on embedded device
- PC application feature for installing and configuring embedded firmware

**Technical Approach:**
The embedded implementation will leverage the STM32H5 cryptographic hardware accelerators for improved performance. The communication protocol will be implemented in Rust with a focus on security and reliability. The PC application will include features for detecting, installing, and configuring the embedded firmware.

#### Phase 4: Polish and Optimization

**Current Status:** ❌ NOT STARTED
- Some UI theming is implemented with a stylesheet
- Performance optimization for large files is implemented with chunking
- Comprehensive testing and documentation are not complete

**Objectives:**
- Complete UI theming and usability enhancements
- Optimize performance for large files on both PC and embedded targets
- Conduct comprehensive testing on both platforms
- Create user and developer documentation

**Key Deliverables:**
- Polished user interface with theming support
- Optimized file handling for large files
- Comprehensive test suite for both PC and embedded targets
- Complete user and developer documentation

**Technical Approach:**
Performance optimization will include file chunking for large files and parallel processing where appropriate. The UI will be enhanced with theming support and improved layouts. A comprehensive testing strategy will ensure both the C++ and Rust components function correctly and securely across both deployment targets.

### 12.3 Technical Highlights

#### Cross-Language Integration

The integration between C++ and Rust will be handled through a carefully designed Foreign Function Interface (FFI). This interface will:

- Define clear boundaries between language domains
- Ensure type safety across language boundaries
- Handle memory management safely between languages
- Provide error propagation mechanisms
- Support complex data structures through serialization
- Maintain consistent behavior across PC and embedded targets

#### Dual-Target Architecture

The dual-target architecture supports both PC and embedded deployments:

1. **PC Deployment**:
   - Full Qt GUI for user interaction
   - Local file encryption/decryption
   - Option to offload encryption to connected embedded device
   - Ability to install and configure embedded firmware

2. **Embedded Deployment (STM32H573I-DK)**:
   - Runs on STM32H573I-DK hardware
   - Provides encryption services to connected host
   - Leverages hardware acceleration for cryptographic operations
   - Communicates via secure protocol with host

#### Rust Input Handling

All user inputs and external communications are handled in Rust to leverage its memory safety features:

- User input from GUI components
- Network commands and data
- Device communications
- File data validation
- Command parsing and validation

This approach significantly enhances security by ensuring that all untrusted input is processed through Rust's memory-safe environment before reaching the C++ core.

### 12.4 FFI, CMake, and Corrosion Integration

The following diagram illustrates in detail how FFI, CMake, and Corrosion work together to integrate C++ and Rust code into a single binary:

```mermaid
graph TD
    subgraph "Build Process"
        subgraph "Source Code"
            CPP[C++ Source Files]
            Rust[Rust Source Files]
            Headers[C/C++ Headers]
        end
        
        subgraph "Build System"
            CMake[CMake Build System]
            Corrosion[Corrosion CMake Module]
            Cargo[Rust's Cargo]
            CXXCompiler[C++ Compiler]
        end
        
        subgraph "Intermediate Artifacts"
            RustLib[Rust Static Library]
            CPPObj[C++ Object Files]
            FFIHeaders[FFI Header Files]
        end
        
        subgraph "Final Output"
            Binary[Single Executable Binary]
        end
        
        subgraph "Runtime"
            CPPRuntime[C++ Runtime Components]
            RustRuntime[Rust Runtime Components]
            FFIBoundary[FFI Boundary]
        end
    end
    
    %% Source relationships
    CPP --> Headers
    Rust --> Headers
    
    %% Build system flow
    CMake --> Corrosion
    CMake --> CXXCompiler
    Corrosion --> Cargo
    
    %% Compilation flow
    Cargo --> RustLib
    CXXCompiler --> CPPObj
    Headers --> FFIHeaders
    
    %% Linking flow
    RustLib --> Binary
    CPPObj --> Binary
    
    %% Runtime relationships
    Binary --> CPPRuntime
    Binary --> RustRuntime
    CPPRuntime <--> FFIBoundary
    RustRuntime <--> FFIBoundary
    
    %% Detailed process steps
    Rust -- "1. cbindgen generates C headers" --> FFIHeaders
    FFIHeaders -- "2. C++ includes headers" --> CPP
    Cargo -- "3. Compiles Rust to static lib" --> RustLib
    CXXCompiler -- "4. Compiles C++ with FFI headers" --> CPPObj
    CMake -- "5. Links everything into one binary" --> Binary
    
    %% Memory model
    subgraph "Memory Model"
        Stack[Stack Memory]
        HeapCPP[C++ Heap Memory]
        HeapRust[Rust Heap Memory]
    end
    
    CPPRuntime --> Stack
    CPPRuntime --> HeapCPP
    RustRuntime --> Stack
    RustRuntime --> HeapRust
    FFIBoundary -- "Memory ownership transfer" --> HeapCPP
    FFIBoundary -- "Memory ownership transfer" --> HeapRust
```

#### Key Integration Components

1. **FFI (Foreign Function Interface)**:
   - Defines the boundary between C++ and Rust code
   - Specifies function signatures that are compatible across languages
   - Handles type conversions between Rust and C++ types
   - Manages memory ownership transfer between language runtimes
   - Provides error propagation mechanisms

2. **CMake Build System**:
   - Serves as the primary build orchestrator
   - Configures the build environment for both C++ and Rust
   - Manages dependencies and build order
   - Handles platform-specific build settings
   - Invokes the appropriate compilers and linkers

3. **Corrosion CMake Module**:
   - Integrates Rust's Cargo build system with CMake
   - Automatically generates CMake targets for Rust crates
   - Handles Rust compilation flags and feature configuration
   - Ensures Rust libraries are built with the correct settings
   - Makes Rust artifacts available to the CMake linking process

4. **cbindgen**:
   - Analyzes Rust code to generate C/C++ compatible headers
   - Ensures type definitions are consistent across language boundaries
   - Handles complex type mappings between Rust and C++
   - Generates documentation for FFI functions
   - Supports attributes for customizing the generated headers

5. **Static Linking Process**:
   - Rust code is compiled to a static library (.a/.lib)
   - C++ code is compiled to object files
   - The linker combines all objects into a single executable
   - Symbol resolution happens across language boundaries
   - Results in a single binary with no external dependencies on Rust

6. **Runtime Integration**:
   - Both language runtimes coexist in the same process
   - Memory is shared through carefully defined interfaces
   - Stack frames can interleave between languages
   - Heap allocations are managed by their respective language
   - Ownership transfer is explicit at FFI boundaries

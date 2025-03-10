# CRUSTy-Core Data Flow Between C++ and Rust

This document illustrates how data flows between C++ and Rust components during cryptographic operations in the CRUSTy-Core project.

## Encryption Data Flow

The following diagram shows the complete journey of data during an encryption operation, from the C++ application through to the Rust crypto implementation and back:

```mermaid
sequenceDiagram
    participant App as C++ Application
    participant Wrapper as C++ Crypto Wrapper
    participant FFI as FFI Boundary
    participant Rust as Rust FFI Function
    participant Crypto as Rust Crypto Implementation

    Note over App,Crypto: Encryption Operation Flow

    App->>Wrapper: encrypt(plaintext, password)

    Note over Wrapper: Create output buffer
    Note over Wrapper: Size = plaintext + overhead

    Wrapper->>FFI: encrypt_data(...)

    FFI->>Rust: encrypt_data(data_ptr, password_ptr, ...)

    Note over Rust: Validate input parameters
    Note over Rust: Convert raw pointers to slices

    Rust->>Crypto: encrypt_aes_gcm(data, password)

    Note over Crypto: Derive key from password
    Note over Crypto: Generate random nonce
    Note over Crypto: Perform AES-GCM encryption

    Crypto-->>Rust: Return ciphertext

    Note over Rust: Copy ciphertext to output buffer
    Note over Rust: Set output_len parameter

    Rust-->>FFI: Return status code

    FFI-->>Wrapper: Return from FFI call

    Note over Wrapper: Check status code
    Note over Wrapper: Resize buffer to actual size
    Note over Wrapper: Create C++ vector from buffer

    Wrapper-->>App: Return encrypted data
```

## Memory Management During Cross-Language Operations

```mermaid
graph LR
    subgraph "C++ Memory Space"
        A[Input Data Buffer] --> |"Pass pointer,\nretain ownership"| C
        B[Output Buffer] --> |"Preallocate,\npass pointer"| C
    end

    subgraph "FFI Boundary"
        C[Raw Pointers + Sizes]
    end

    subgraph "Rust Memory Space"
        C --> |"Convert to\nRust slices"| D[Safe Rust Slices]
        D --> |"Process data"| E[Processed Data]
        E --> |"Copy to\noutput buffer"| F[Output in C++ Buffer]
    end

    F --> |"Set size via\noutput_len param"| B

    style C fill:#f9f,stroke:#333,stroke-width:2px
```

## Error Handling Across Language Boundary

```mermaid
flowchart TD
    %% C++ Domain
    A[C++ Function Call] --> B[C++ Error Handler]

    %% Error Flow
    B --> C{Error?}
    C -->|Yes| D[C++ Exception]
    C -->|No| E[Continue Processing]

    %% FFI Crossing
    B --> F[FFI Call]
    F --> G[Rust Function]

    %% Rust Domain
    G --> H{Error?}
    H -->|Yes| I[Map Rust Error\nto Error Code]
    H -->|No| J[Process and\nReturn 0]

    I --> K[Return Non-Zero\nError Code]

    %% Back to C++
    K --> L[Check Return Code]
    J --> L

    L --> M{Error Code != 0?}
    M -->|Yes| N[Map to C++\nException]
    M -->|No| O[Process Result]

    N --> P[Throw Exception]

    style F fill:#f9f,stroke:#333,stroke-width:2px
    style G fill:#deb887,stroke:#8b4513,color:#000
    style I fill:#deb887,stroke:#8b4513,color:#000
    style J fill:#deb887,stroke:#8b4513,color:#000
    style K fill:#deb887,stroke:#8b4513,color:#000
```

## Key Components and Their Interactions

The table below summarizes how different components interact across the language boundary:

| Component                  | Language | Role in Integration                      | Interfaces With       |
| -------------------------- | -------- | ---------------------------------------- | --------------------- |
| `Encryptor`                | C++      | High-level API for encryption operations | `Crypto` C++ wrapper  |
| `Crypto`                   | C++      | Wraps Rust FFI functions in C++ API      | FFI functions         |
| `crypto_interface.h`       | C/C++    | Defines FFI boundary                     | Both C++ and Rust     |
| Rust FFI Functions         | Rust     | Expose Rust functionality to C++         | C++ via FFI           |
| Rust Crypto Implementation | Rust     | Core cryptographic operations            | Rust crypto libraries |

## Implementation Benefits

This integration approach offers several key benefits:

1. **Memory Safety**: Rust handles sensitive cryptographic operations with its memory safety guarantees
2. **Performance**: Minimal overhead for crossing the language boundary
3. **Type Safety**: C++ wrappers provide type-safe interfaces to Rust functions
4. **Error Handling**: Clear mapping between Rust results and C++ exceptions
5. **Modularity**: Each language handles the tasks it's best suited for

## Real-World Example: Handling a File Encryption Operation

```mermaid
sequenceDiagram
    participant UI as C++ UI
    participant Enc as C++ Encryptor
    participant Wrapper as C++ Crypto Wrapper
    participant FFI as FFI Boundary
    participant Rust as Rust Crypto

    UI->>Enc: encryptFile(source, dest, password)

    loop For each file chunk
        Enc->>Enc: Read chunk from source file
        Enc->>Wrapper: encrypt(chunk, password)

        Wrapper->>FFI: encrypt_data(...)
        FFI->>Rust: Process with Rust crypto
        Rust-->>FFI: Return encrypted chunk
        FFI-->>Wrapper: Return result

        Wrapper-->>Enc: Return encrypted chunk
        Enc->>Enc: Write chunk to destination file
        Enc->>UI: Update progress
    end

    Enc-->>UI: File encryption complete
```

This diagram shows how the system handles a real file encryption operation, breaking the file into chunks to process efficiently while maintaining a responsive UI.

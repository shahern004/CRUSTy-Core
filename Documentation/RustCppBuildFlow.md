# CRUSTy-Core Build System Flow Diagram

This diagram illustrates the complete build process for the CRUSTy-Core project, showing how Rust and C++ components are compiled and linked together.

```mermaid
flowchart TD
    %% Main build process
    Start([Build Start]) --> CMake[CMake Configuration]
    CMake --> FindCorrosion[Find Corrosion Package]
    FindCorrosion --> ImportCrate[Import Rust Crate]

    %% Rust compilation branch
    ImportCrate --> RustCompile[Compile Rust Library]
    RustCompile --> GenerateBindings[Generate C++ Bindings]

    %% C++ compilation branch
    CMake --> CompileCpp[Compile C++ Sources]

    %% Binding generation decision
    GenerateBindings --> BindingCheck{cbindgen\nSuccessful?}
    BindingCheck -->|Yes| UseGenHeaders[Use Generated Headers]
    BindingCheck -->|No| UseManualHeaders[Use Manual Header Fallback]

    UseGenHeaders --> HeaderDone[Final crypto_interface.h]
    UseManualHeaders --> HeaderDone

    %% Platform-specific linking
    HeaderDone --> PlatformCheck{MSVC Build?}

    %% MSVC branch
    PlatformCheck -->|Yes| MSVCLink[Special MSVC Linking]
    MSVCLink --> LinkSystemLibs[Link Windows System Libraries]

    %% Standard branch
    PlatformCheck -->|No| StandardLink[Standard Linking]

    %% Final steps
    LinkSystemLibs --> LinkRustLib[Link with Rust Static Library]
    StandardLink --> LinkRustLib

    CompileCpp --> LinkCppComponents[Link C++ Components]

    LinkRustLib --> FinalLink[Link Final Executable]
    LinkCppComponents --> FinalLink

    FinalLink --> BuildEnd([Build Complete])

    %% Styles
    classDef rustNode fill:#deb887,stroke:#8b4513,color:#000
    classDef cppNode fill:#add8e6,stroke:#4682b4,color:#000
    classDef headerNode fill:#ffa07a,stroke:#ff6347,color:#000
    classDef linkNode fill:#90ee90,stroke:#3cb371,color:#000
    classDef decisionNode fill:#ffd700,stroke:#daa520,color:#000

    class RustCompile,ImportCrate rustNode
    class CompileCpp,LinkCppComponents cppNode
    class GenerateBindings,HeaderDone,UseGenHeaders,UseManualHeaders headerNode
    class LinkRustLib,MSVCLink,StandardLink,LinkSystemLibs,FinalLink linkNode
    class BindingCheck,PlatformCheck decisionNode
```

## Build Process Explained

1. **CMake Configuration**: The build process starts with CMake configuring the build based on CMakeLists.txt
2. **Corrosion Integration**: CMake finds and integrates the Corrosion package for Rust support
3. **Rust Compilation**:
   - The Rust crate is imported and compiled
   - cbindgen attempts to generate C++ bindings for the Rust functions
   - If cbindgen fails, a manual header fallback is used
4. **C++ Compilation**: C++ source files are compiled independently
5. **Platform-Specific Linking**:
   - For MSVC builds, special linking logic is applied
   - Windows system libraries are explicitly linked
   - For other platforms, standard linking is used
6. **Final Linking**:
   - The Rust static library is linked with the C++ components
   - The final executable is generated

This multi-stage process ensures that both Rust and C++ components are properly integrated regardless of platform or build tools.

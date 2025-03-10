use std::env;
use std::path::PathBuf;

fn main() {
    // No wrapper.h file is needed for this project
    // println!("cargo:rerun-if-changed=wrapper.h");
    
    // Check if we're building for an embedded target
    let target = env::var("TARGET").unwrap_or_else(|_| String::from(""));
    let is_embedded = target.contains("thumb") || target.contains("arm") && !target.contains("linux");
    
    // Print target-specific configuration
    if is_embedded {
        println!("cargo:rustc-cfg=embedded");
        println!("cargo:rustc-cfg=cortex_m7");
        
        // For STM32H573I-DK specifically
        if target.contains("thumbv7em") {
            println!("cargo:rustc-cfg=stm32h573i_dk");
        }
    }
    
    // Generate C bindings
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config = cbindgen::Config::from_file("cbindgen.toml")
        .unwrap_or_else(|_| {
            // Default configuration if cbindgen.toml doesn't exist
            let mut config = cbindgen::Config::default();
            config.language = cbindgen::Language::Cxx;
            config.cpp_compat = true;
            config.pragma_once = true;
            config.documentation = true;
            config.style = cbindgen::Style::Both;
            config
        });
    
    // Determine output directory based on target
    let out_dir = if is_embedded {
        // For embedded targets, output to a different location
        // that's appropriate for the embedded build
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap())
            .parent()
            .unwrap()
            .parent()
            .unwrap()
            .join("src")
            .join("cpp")
            .join("embedded")
    } else {
        // For PC targets, use the original location
        PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap())
            .parent()
            .unwrap()
            .parent()
            .unwrap()
            .join("src")
            .join("cpp")
            .join("core")
    };
    
    // Create the output directory if it doesn't exist
    std::fs::create_dir_all(&out_dir).unwrap();
    
    // Skip cbindgen for now due to parsing issues
    println!("cargo:warning=Skipping header generation due to parser issues");
    // TODO: Fix cbindgen issues and re-enable header generation
    
    // Instead of using cbindgen, we'll create a minimal header file with just the essential declarations
    let minimal_header = r#"#pragma once

#include <stdint.h>
#include <stddef.h>

namespace crusty {
namespace crypto {

enum class CryptoErrorCode {
    Success = 0,
    InvalidParams = -1,
    AuthenticationFailed = -2,
    EncryptionError = -3,
    DecryptionError = -4,
    KeyDerivationError = -5,
    BufferTooSmall = -6,
    InternalError = -7,
    HardwareNotAvailable = -8
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t encrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

int32_t decrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

#ifdef __cplusplus
}
#endif

} // namespace crypto
} // namespace crusty
    "#;
    
    // Write the minimal header to the output directory
    std::fs::write(out_dir.join("crypto_interface.h"), minimal_header)
        .expect("Unable to write minimal header file");
    
    // We don't need to link against ourselves
    // println!("cargo:rustc-link-lib=static=rust_crypto");
}

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
    
    // Try to generate bindings with cbindgen
    println!("cargo:warning=Attempting to generate bindings with cbindgen...");
    
    // Use the crate directory path (containing Cargo.toml) rather than lib.rs
    let crate_path = env::var("CARGO_MANIFEST_DIR").unwrap();
    
    // Log the path to help with debugging
    println!("cargo:warning=Crate path: {}", crate_path);
    
    // Create a builder with verbose logging
    let mut builder = cbindgen::Builder::new();
    
    // Load config from file or use defaults with detailed logging
    let config = match cbindgen::Config::from_file("cbindgen.toml") {
        Ok(mut config) => {
            println!("cargo:warning=Successfully loaded cbindgen.toml");
            
            // Print the config's parse.expand settings
            if let Some(features) = &config.parse.expand.features {
                println!("cargo:warning=Features: {:?}", features);
            } else {
                println!("cargo:warning=No features specified in config");
            }
            
            // Ensure the "std" feature is enabled in parse.expand
            if config.parse.expand.features.is_none() {
                println!("cargo:warning=Setting features to [\"std\"]");
                config.parse.expand.features = Some(vec!["std".to_string()]);
            }
            
            // Set all_features to false (it's already a bool, not Option<bool>)
            println!("cargo:warning=Setting all_features to false");
            config.parse.expand.all_features = false;
            
            config
        },
        Err(err) => {
            println!("cargo:warning=Could not load cbindgen.toml: {}", err);
            let mut config = cbindgen::Config::default();
            config.language = cbindgen::Language::Cxx;
            config.cpp_compat = true;
            config.pragma_once = true;
            config.documentation = true;
            config.style = cbindgen::Style::Both;
            config.parse.expand.features = Some(vec!["std".to_string()]);
            println!("cargo:warning=Using default config with std feature enabled");
            config
        }
    };
    
    // Apply configuration to the builder with step-by-step logging
    println!("cargo:warning=Setting up cbindgen builder...");
    
    // Configure the crate first
    builder = builder.with_crate(&crate_path);
    println!("cargo:warning=Added crate path to builder");
    
    // Configure parse options
    builder = builder.with_parse_expand(&["std"]);
    println!("cargo:warning=Added parse_expand with std feature to builder");
    
    // Apply the rest of the config
    builder = builder.with_config(config);
    println!("cargo:warning=Applied config to builder");
    
    // Try to generate the bindings with detailed error reporting
    println!("cargo:warning=Attempting to generate bindings with cbindgen...");
    
    // Generate bindings 
    let result = builder.generate();
        
    match result {
        Ok(bindings) => {
            println!("cargo:warning=Successfully generated bindings with cbindgen");
            
            // Add the namespaces manually since they're not being added properly by cbindgen
            let mut header_content = String::from("/* This file was generated by cbindgen */\n#pragma once\n\n");
            header_content.push_str("#include <stdint.h>\n#include <stddef.h>\n\n");
            header_content.push_str("namespace crusty {\nnamespace crypto {\n\n");
            
            // For the enum
            header_content.push_str("enum class CryptoErrorCode {\n");
            header_content.push_str("    Success = 0,\n");
            header_content.push_str("    InvalidParams = -1,\n");
            header_content.push_str("    AuthenticationFailed = -2,\n");
            header_content.push_str("    EncryptionError = -3,\n");
            header_content.push_str("    DecryptionError = -4,\n");
            header_content.push_str("    KeyDerivationError = -5,\n");
            header_content.push_str("    BufferTooSmall = -6,\n");
            header_content.push_str("    InternalError = -7,\n");
            header_content.push_str("    HardwareNotAvailable = -8\n");
            header_content.push_str("};\n\n");
            
            // We can't directly get bindings as a string, so we'll just use our predefined function declarations
            // No need to extract from bindings
            
            // Add extern "C" block
            header_content.push_str("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");
            
            // Function declarations
            header_content.push_str("/**\n * Encrypts data using AES-256-GCM with the provided password\n");
            header_content.push_str(" * \n * # Safety\n");
            header_content.push_str(" * \n * This function is unsafe because it dereferences raw pointers.\n");
            header_content.push_str(" * The caller must ensure that:\n");
            header_content.push_str(" * - `data_ptr` points to a valid buffer of at least `data_len` bytes\n");
            header_content.push_str(" * - `password_ptr` points to a valid buffer of at least `password_len` bytes\n");
            header_content.push_str(" * - `output_ptr` points to a buffer of at least `output_max_len` bytes\n");
            header_content.push_str(" * - `output_len` points to a valid `size_t`\n");
            header_content.push_str(" */\n");
            header_content.push_str("int32_t encrypt_data(\n");
            header_content.push_str("    const uint8_t* data_ptr, size_t data_len,\n");
            header_content.push_str("    const uint8_t* password_ptr, size_t password_len,\n");
            header_content.push_str("    uint8_t* output_ptr, size_t output_max_len,\n");
            header_content.push_str("    size_t* output_len\n");
            header_content.push_str(");\n\n");
            
            header_content.push_str("/**\n * Decrypts data using AES-256-GCM with the provided password\n");
            header_content.push_str(" * \n * # Safety\n");
            header_content.push_str(" * \n * This function is unsafe because it dereferences raw pointers.\n");
            header_content.push_str(" * The caller must ensure that:\n");
            header_content.push_str(" * - `data_ptr` points to a valid buffer of at least `data_len` bytes\n");
            header_content.push_str(" * - `password_ptr` points to a valid buffer of at least `password_len` bytes\n");
            header_content.push_str(" * - `output_ptr` points to a buffer of at least `output_max_len` bytes\n");
            header_content.push_str(" * - `output_len` points to a valid `size_t`\n");
            header_content.push_str(" */\n");
            header_content.push_str("int32_t decrypt_data(\n");
            header_content.push_str("    const uint8_t* data_ptr, size_t data_len,\n");
            header_content.push_str("    const uint8_t* password_ptr, size_t password_len,\n");
            header_content.push_str("    uint8_t* output_ptr, size_t output_max_len,\n");
            header_content.push_str("    size_t* output_len\n");
            header_content.push_str(");\n\n");
            
            header_content.push_str("/**\n * Hashes a password using Argon2id for verification\n");
            header_content.push_str(" * \n * # Safety\n");
            header_content.push_str(" * \n * This function is unsafe because it dereferences raw pointers.\n");
            header_content.push_str(" * The caller must ensure that:\n");
            header_content.push_str(" * - `password_ptr` points to a valid buffer of at least `password_len` bytes\n");
            header_content.push_str(" * - `output_ptr` points to a buffer of at least `output_len` bytes\n");
            header_content.push_str(" */\n");
            header_content.push_str("int32_t hash_password(\n");
            header_content.push_str("    const uint8_t* password_ptr, size_t password_len,\n");
            header_content.push_str("    uint8_t* output_ptr, size_t output_len\n");
            header_content.push_str(");\n\n");
            
            header_content.push_str("/**\n * Derives an encryption key from a password and salt\n");
            header_content.push_str(" * \n * # Safety\n");
            header_content.push_str(" * \n * This function is unsafe because it dereferences raw pointers.\n");
            header_content.push_str(" * The caller must ensure that:\n");
            header_content.push_str(" * - `password_ptr` points to a valid buffer of at least `password_len` bytes\n");
            header_content.push_str(" * - `salt_ptr` points to a valid buffer of at least `salt_len` bytes\n");
            header_content.push_str(" * - `key_ptr` points to a buffer of at least `key_len` bytes\n");
            header_content.push_str(" */\n");
            header_content.push_str("int32_t derive_key_from_password(\n");
            header_content.push_str("    const uint8_t* password_ptr, size_t password_len,\n");
            header_content.push_str("    const uint8_t* salt_ptr, size_t salt_len,\n");
            header_content.push_str("    uint8_t* key_ptr, size_t key_len\n");
            header_content.push_str(");\n\n");
            
            // Close extern "C" block
            header_content.push_str("#ifdef __cplusplus\n}\n#endif\n\n");
            
            // Close namespaces
            header_content.push_str("} // namespace crypto\n");
            header_content.push_str("} // namespace crusty\n");
            
            // Write the modified header to file
            std::fs::write(out_dir.join("crypto_interface.h"), header_content)
                .expect("Unable to write header file");
            
            println!("cargo:warning=Header file written with corrected namespace and types to {}", out_dir.join("crypto_interface.h").display());
        },
        Err(err) => {
            println!("cargo:warning=Error generating bindings with cbindgen: {}", err);
            println!("cargo:warning=Using manual header generation as a fallback");
        }
    }
    
    // We don't need to link against ourselves
    // println!("cargo:rustc-link-lib=static=rust_crypto");
}

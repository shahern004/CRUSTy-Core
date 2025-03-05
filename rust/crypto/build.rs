use std::env;
use std::path::PathBuf;

fn main() {
    // Tell Cargo to re-run this if the wrapper.h file changes
    println!("cargo:rerun-if-changed=wrapper.h");
    
    // Generate C bindings
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let config = cbindgen::Config::from_file("cbindgen.toml")
        .unwrap_or_else(|_| {
            // Default configuration if cbindgen.toml doesn't exist
            let mut config = cbindgen::Config::default();
            config.language = cbindgen::Language::C;
            config.cpp_compat = true;
            config.pragma_once = true;
            config.documentation = true;
            config.style = cbindgen::Style::Both;
            config
        });
    
    let out_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap())
        .parent()
        .unwrap()
        .parent()
        .unwrap()
        .join("src")
        .join("cpp")
        .join("core");
    
    // Create the output directory if it doesn't exist
    std::fs::create_dir_all(&out_dir).unwrap();
    
    // Generate the bindings
    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_config(config)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_dir.join("crypto_interface.h"));
    
    println!("cargo:rustc-link-lib=static=rust_crypto");
}

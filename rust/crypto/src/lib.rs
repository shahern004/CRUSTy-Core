/// Cryptographic core for CRUSTy-Qt
/// 
/// This module provides the core cryptographic functionality for CRUSTy-Qt,
/// including AES-256-GCM encryption/decryption and password-based key derivation.
/// It exposes a C-compatible FFI interface for integration with C++ code.
use aes_gcm::{
    aead::{Aead, KeyInit, OsRng},
    Aes256Gcm, Key, Nonce
};
use argon2::{
    password_hash::{
        rand_core::OsRng as Argon2OsRng,
        PasswordHasher, SaltString
    },
    Argon2
};
use rand::RngCore;
use std::slice;
use std::ptr;

/// Error codes for cryptographic operations
#[repr(C)]
pub enum CryptoErrorCode {
    /// Operation completed successfully
    Success = 0,
    /// Invalid parameters provided
    InvalidParams = -1,
    /// Authentication failed during decryption
    AuthenticationFailed = -2,
    /// Error during encryption
    EncryptionError = -3,
    /// Error during decryption
    DecryptionError = -4,
    /// Error with key derivation
    KeyDerivationError = -5,
    /// Buffer too small for output
    BufferTooSmall = -6,
    /// Internal error
    InternalError = -7,
}

/// Encrypts data using AES-256-GCM with the provided password
/// 
/// # Safety
/// 
/// This function is unsafe because it dereferences raw pointers.
/// The caller must ensure that:
/// - `data_ptr` points to a valid buffer of at least `data_len` bytes
/// - `password_ptr` points to a valid buffer of at least `password_len` bytes
/// - `output_ptr` points to a buffer of at least `output_max_len` bytes
/// - `output_len` points to a valid `usize`
#[no_mangle]
pub unsafe extern "C" fn encrypt_data(
    data_ptr: *const u8, data_len: usize,
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_max_len: usize,
    output_len: *mut usize
) -> i32 {
    // Validate parameters
    if data_ptr.is_null() || password_ptr.is_null() || output_ptr.is_null() || output_len.is_null() {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Convert raw pointers to slices
    let data = slice::from_raw_parts(data_ptr, data_len);
    let password = slice::from_raw_parts(password_ptr, password_len);
    
    // Derive key from password
    let key = match derive_key_from_password_internal(password) {
        Ok(k) => k,
        Err(_) => return CryptoErrorCode::KeyDerivationError as i32,
    };
    
    // Generate a random nonce
    let mut nonce_bytes = [0u8; 12];
    OsRng.fill_bytes(&mut nonce_bytes);
    let nonce = Nonce::from_slice(&nonce_bytes);
    
    // Create the cipher
    let cipher = match Aes256Gcm::new(Key::<Aes256Gcm>::from_slice(&key)) {
        Ok(c) => c,
        Err(_) => return CryptoErrorCode::EncryptionError as i32,
    };
    
    // Encrypt the data
    let ciphertext = match cipher.encrypt(nonce, data) {
        Ok(c) => c,
        Err(_) => return CryptoErrorCode::EncryptionError as i32,
    };
    
    // Calculate required output size
    let required_size = 12 + 4 + ciphertext.len(); // nonce + ciphertext length + ciphertext
    
    // Check if output buffer is large enough
    if output_max_len < required_size {
        *output_len = required_size;
        return CryptoErrorCode::BufferTooSmall as i32;
    }
    
    // Write nonce to output
    let output_slice = slice::from_raw_parts_mut(output_ptr, output_max_len);
    output_slice[0..12].copy_from_slice(&nonce_bytes);
    
    // Write ciphertext length to output
    let ciphertext_len_bytes = (ciphertext.len() as u32).to_be_bytes();
    output_slice[12..16].copy_from_slice(&ciphertext_len_bytes);
    
    // Write ciphertext to output
    output_slice[16..16 + ciphertext.len()].copy_from_slice(&ciphertext);
    
    // Set output length
    *output_len = required_size;
    
    CryptoErrorCode::Success as i32
}

/// Decrypts data using AES-256-GCM with the provided password
/// 
/// # Safety
/// 
/// This function is unsafe because it dereferences raw pointers.
/// The caller must ensure that:
/// - `data_ptr` points to a valid buffer of at least `data_len` bytes
/// - `password_ptr` points to a valid buffer of at least `password_len` bytes
/// - `output_ptr` points to a buffer of at least `output_max_len` bytes
/// - `output_len` points to a valid `usize`
#[no_mangle]
pub unsafe extern "C" fn decrypt_data(
    data_ptr: *const u8, data_len: usize,
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_max_len: usize,
    output_len: *mut usize
) -> i32 {
    // Validate parameters
    if data_ptr.is_null() || password_ptr.is_null() || output_ptr.is_null() || output_len.is_null() {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Check if data is long enough to contain nonce and length
    if data_len < 16 {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Convert raw pointers to slices
    let data = slice::from_raw_parts(data_ptr, data_len);
    let password = slice::from_raw_parts(password_ptr, password_len);
    
    // Extract the nonce
    let nonce = Nonce::from_slice(&data[0..12]);
    
    // Extract the ciphertext length
    let ciphertext_len = u32::from_be_bytes([data[12], data[13], data[14], data[15]]) as usize;
    
    // Verify the data length
    if data_len < 16 + ciphertext_len {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Extract the ciphertext
    let ciphertext = &data[16..16 + ciphertext_len];
    
    // Derive key from password
    let key = match derive_key_from_password_internal(password) {
        Ok(k) => k,
        Err(_) => return CryptoErrorCode::KeyDerivationError as i32,
    };
    
    // Create the cipher
    let cipher = match Aes256Gcm::new(Key::<Aes256Gcm>::from_slice(&key)) {
        Ok(c) => c,
        Err(_) => return CryptoErrorCode::DecryptionError as i32,
    };
    
    // Decrypt the data
    let plaintext = match cipher.decrypt(nonce, ciphertext) {
        Ok(p) => p,
        Err(_) => return CryptoErrorCode::AuthenticationFailed as i32,
    };
    
    // Check if output buffer is large enough
    if output_max_len < plaintext.len() {
        *output_len = plaintext.len();
        return CryptoErrorCode::BufferTooSmall as i32;
    }
    
    // Write plaintext to output
    let output_slice = slice::from_raw_parts_mut(output_ptr, output_max_len);
    output_slice[0..plaintext.len()].copy_from_slice(&plaintext);
    
    // Set output length
    *output_len = plaintext.len();
    
    CryptoErrorCode::Success as i32
}

/// Hashes a password using Argon2id for verification
/// 
/// # Safety
/// 
/// This function is unsafe because it dereferences raw pointers.
/// The caller must ensure that:
/// - `password_ptr` points to a valid buffer of at least `password_len` bytes
/// - `output_ptr` points to a buffer of at least `output_len` bytes
#[no_mangle]
pub unsafe extern "C" fn hash_password(
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_len: usize
) -> i32 {
    // Validate parameters
    if password_ptr.is_null() || output_ptr.is_null() {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Convert raw pointers to slices
    let password = slice::from_raw_parts(password_ptr, password_len);
    
    // Generate a salt
    let salt = SaltString::generate(&mut Argon2OsRng);
    
    // Create Argon2id instance
    let argon2 = Argon2::default();
    
    // Hash the password
    let password_hash = match argon2.hash_password(password, &salt) {
        Ok(hash) => hash.to_string(),
        Err(_) => return CryptoErrorCode::KeyDerivationError as i32,
    };
    
    // Check if output buffer is large enough
    if output_len < password_hash.len() {
        return CryptoErrorCode::BufferTooSmall as i32;
    }
    
    // Write hash to output
    let output_slice = slice::from_raw_parts_mut(output_ptr, output_len);
    output_slice[0..password_hash.len()].copy_from_slice(password_hash.as_bytes());
    
    // Null-terminate the string
    if output_len > password_hash.len() {
        output_slice[password_hash.len()] = 0;
    }
    
    CryptoErrorCode::Success as i32
}

/// Derives an encryption key from a password and salt
/// 
/// # Safety
/// 
/// This function is unsafe because it dereferences raw pointers.
/// The caller must ensure that:
/// - `password_ptr` points to a valid buffer of at least `password_len` bytes
/// - `salt_ptr` points to a valid buffer of at least `salt_len` bytes
/// - `key_ptr` points to a buffer of at least `key_len` bytes
#[no_mangle]
pub unsafe extern "C" fn derive_key_from_password(
    password_ptr: *const u8, password_len: usize,
    salt_ptr: *const u8, salt_len: usize,
    key_ptr: *mut u8, key_len: usize
) -> i32 {
    // Validate parameters
    if password_ptr.is_null() || salt_ptr.is_null() || key_ptr.is_null() {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Check if key length is valid
    if key_len != 32 {
        return CryptoErrorCode::InvalidParams as i32;
    }
    
    // Convert raw pointers to slices
    let password = slice::from_raw_parts(password_ptr, password_len);
    let salt = slice::from_raw_parts(salt_ptr, salt_len);
    
    // Create Argon2id instance
    let argon2 = Argon2::default();
    
    // Derive key
    let mut key = [0u8; 32];
    if let Err(_) = argon2.hash_password_into(password, salt, &mut key) {
        return CryptoErrorCode::KeyDerivationError as i32;
    }
    
    // Write key to output
    let key_slice = slice::from_raw_parts_mut(key_ptr, key_len);
    key_slice.copy_from_slice(&key);
    
    CryptoErrorCode::Success as i32
}

// Internal function to derive a key from a password
fn derive_key_from_password_internal(password: &[u8]) -> Result<[u8; 32], ()> {
    // Generate a salt
    let salt = SaltString::generate(&mut Argon2OsRng);
    
    // Create Argon2id instance
    let argon2 = Argon2::default();
    
    // Derive key
    let mut key = [0u8; 32];
    argon2.hash_password_into(password, salt.as_str().as_bytes(), &mut key)
        .map_err(|_| ())?;
    
    Ok(key)
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_encrypt_decrypt_roundtrip() {
        let data = b"Hello, CRUSTy-Qt!";
        let password = b"secure_password";
        
        // Buffers for encryption
        let mut encrypted = vec![0u8; 1024];
        let mut encrypted_len = 0;
        
        // Encrypt
        let result = unsafe {
            encrypt_data(
                data.as_ptr(), data.len(),
                password.as_ptr(), password.len(),
                encrypted.as_mut_ptr(), encrypted.len(),
                &mut encrypted_len
            )
        };
        
        assert_eq!(result, CryptoErrorCode::Success as i32);
        
        // Resize encrypted buffer to actual length
        encrypted.truncate(encrypted_len);
        
        // Buffers for decryption
        let mut decrypted = vec![0u8; 1024];
        let mut decrypted_len = 0;
        
        // Decrypt
        let result = unsafe {
            decrypt_data(
                encrypted.as_ptr(), encrypted.len(),
                password.as_ptr(), password.len(),
                decrypted.as_mut_ptr(), decrypted.len(),
                &mut decrypted_len
            )
        };
        
        assert_eq!(result, CryptoErrorCode::Success as i32);
        
        // Resize decrypted buffer to actual length
        decrypted.truncate(decrypted_len);
        
        // Verify decrypted data matches original
        assert_eq!(decrypted, data);
    }
    
    #[test]
    fn test_wrong_password() {
        let data = b"Hello, CRUSTy-Qt!";
        let password = b"secure_password";
        let wrong_password = b"wrong_password";
        
        // Buffers for encryption
        let mut encrypted = vec![0u8; 1024];
        let mut encrypted_len = 0;
        
        // Encrypt
        let result = unsafe {
            encrypt_data(
                data.as_ptr(), data.len(),
                password.as_ptr(), password.len(),
                encrypted.as_mut_ptr(), encrypted.len(),
                &mut encrypted_len
            )
        };
        
        assert_eq!(result, CryptoErrorCode::Success as i32);
        
        // Resize encrypted buffer to actual length
        encrypted.truncate(encrypted_len);
        
        // Buffers for decryption
        let mut decrypted = vec![0u8; 1024];
        let mut decrypted_len = 0;
        
        // Decrypt with wrong password
        let result = unsafe {
            decrypt_data(
                encrypted.as_ptr(), encrypted.len(),
                wrong_password.as_ptr(), wrong_password.len(),
                decrypted.as_mut_ptr(), decrypted.len(),
                &mut decrypted_len
            )
        };
        
        // Should fail with authentication error
        assert_eq!(result, CryptoErrorCode::AuthenticationFailed as i32);
    }
}

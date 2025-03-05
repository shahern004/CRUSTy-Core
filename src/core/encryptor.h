#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <functional>

namespace crusty {

/**
 * @brief Exception class for encryption errors
 */
class EncryptionException : public std::runtime_error {
public:
    explicit EncryptionException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Class for file encryption and decryption operations
 * 
 * This class provides a C++ wrapper around the Rust cryptographic functions,
 * offering a clean, object-oriented interface for encryption operations.
 */
class Encryptor {
public:
    /**
     * @brief Default constructor
     */
    Encryptor() = default;
    
    /**
     * @brief Encrypt a file with a password
     * 
     * @param sourcePath Path to the source file
     * @param destPath Path to the destination file
     * @param password Password for encryption
     * @param progressCallback Callback function for progress updates (0.0 to 1.0)
     * @throws EncryptionException if encryption fails
     */
    void encryptFile(
        const std::string& sourcePath,
        const std::string& destPath,
        const std::string& password,
        std::function<void(float)> progressCallback = nullptr
    );
    
    /**
     * @brief Decrypt a file with a password
     * 
     * @param sourcePath Path to the source file
     * @param destPath Path to the destination file
     * @param password Password for decryption
     * @param secondFactor Second factor for authentication (optional)
     * @param progressCallback Callback function for progress updates (0.0 to 1.0)
     * @throws EncryptionException if decryption fails
     */
    void decryptFile(
        const std::string& sourcePath,
        const std::string& destPath,
        const std::string& password,
        const std::string& secondFactor = "",
        std::function<void(float)> progressCallback = nullptr
    );
    
    /**
     * @brief Encrypt raw data with a password
     * 
     * @param data Data to encrypt
     * @param password Password for encryption
     * @return Encrypted data
     * @throws EncryptionException if encryption fails
     */
    std::vector<uint8_t> encryptData(
        const std::vector<uint8_t>& data,
        const std::string& password
    );
    
    /**
     * @brief Decrypt raw data with a password
     * 
     * @param data Data to decrypt
     * @param password Password for decryption
     * @return Decrypted data
     * @throws EncryptionException if decryption fails
     */
    std::vector<uint8_t> decryptData(
        const std::vector<uint8_t>& data,
        const std::string& password
    );
    
    /**
     * @brief Hash a password for verification
     * 
     * @param password Password to hash
     * @return Hashed password
     * @throws EncryptionException if hashing fails
     */
    std::string hashPassword(const std::string& password);
    
    /**
     * @brief Verify a password against a hash
     * 
     * @param password Password to verify
     * @param hash Hash to verify against
     * @return True if password matches hash, false otherwise
     */
    bool verifyPassword(const std::string& password, const std::string& hash);
    
private:
    // Helper methods
    std::vector<uint8_t> readFile(const std::string& path);
    void writeFile(const std::string& path, const std::vector<uint8_t>& data);
};

} // namespace crusty

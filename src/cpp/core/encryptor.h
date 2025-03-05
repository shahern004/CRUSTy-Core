#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace crusty {

/**
 * Progress callback type for encryption/decryption operations
 * Reports progress as a value from 0.0 (started) to 1.0 (completed)
 */
using ProgressCallback = std::function<void(float progress)>;

/**
 * Detailed error codes for encryption operations
 */
enum class CryptoErrorCode {
    InvalidPassword,
    DataCorrupted,
    AuthenticationFailed, 
    IoError,
    InternalError
};

/**
 * @brief Exception class for encryption errors with detailed error information
 */
class EncryptionException : public std::runtime_error {
private:
    CryptoErrorCode error_code_;

public:
    EncryptionException(const std::string& message, CryptoErrorCode code)
        : std::runtime_error(message), error_code_(code) {}
    
    /**
     * @return The specific error code associated with this exception
     */
    CryptoErrorCode getErrorCode() const { return error_code_; }
};

/**
 * @brief Core cryptographic operations
 * 
 * Provides pure cryptographic functionality without file I/O dependencies
 */
class Crypto {
public:
    Crypto() = default;
    virtual ~Crypto() = default;
    
    /**
     * @brief Encrypt raw data with a password
     * 
     * @param plaintext Data to encrypt
     * @param password Password for encryption
     * @return Encrypted data
     * @throws EncryptionException if encryption fails
     */
    virtual std::vector<uint8_t> encrypt(
        const std::vector<uint8_t>& plaintext,
        const std::string& password
    ) const;
    
    /**
     * @brief Decrypt raw data with a password
     * 
     * @param ciphertext Data to decrypt
     * @param password Password for decryption
     * @return Decrypted data
     * @throws EncryptionException if decryption fails
     */
    virtual std::vector<uint8_t> decrypt(
        const std::vector<uint8_t>& ciphertext,
        const std::string& password
    ) const;
    
    /**
     * @brief Hash a password for storage and verification
     * 
     * @param password Password to hash
     * @return Hashed password
     * @throws EncryptionException if hashing fails
     */
    virtual std::string hashPassword(const std::string& password) const;
    
    /**
     * @brief Verify a password against a hash
     * 
     * @param password Password to verify
     * @param hash Hash to verify against
     * @return True if password matches hash, false otherwise
     */
    virtual bool verifyPassword(
        const std::string& password, 
        const std::string& hash
    ) const;
};

/**
 * @brief File encryption and decryption operations
 */
class Encryptor {
public:
    /**
     * @brief Default constructor
     */
    Encryptor();
    
    /**
     * @brief Constructor with custom crypto implementation
     * 
     * @param crypto Custom crypto implementation (for testing/mocking)
     */
    explicit Encryptor(std::unique_ptr<Crypto> crypto);
    
    /**
     * @brief Encrypt a file with a password
     * 
     * @param sourcePath Path to the source file
     * @param destPath Path to the destination file
     * @param password Password for encryption
     * @param progressCallback Optional callback for progress updates
     * @throws EncryptionException if encryption fails
     */
    void encryptFile(
        const std::string& sourcePath,
        const std::string& destPath,
        const std::string& password,
        ProgressCallback progressCallback = nullptr
    );
    
    /**
     * @brief Decrypt a file with a password
     * 
     * @param sourcePath Path to the source file
     * @param destPath Path to the destination file
     * @param password Password for decryption
     * @param progressCallback Optional callback for progress updates
     * @throws EncryptionException if decryption fails
     */
    void decryptFile(
        const std::string& sourcePath,
        const std::string& destPath,
        const std::string& password,
        ProgressCallback progressCallback = nullptr
    );
    
    /**
     * @brief Set the chunk size for processing large files
     * 
     * Adjusts how much data is processed at once during file operations
     * 
     * @param bytes Chunk size in bytes
     */
    void setChunkSize(size_t bytes);
    
private:
    // Implementation detail: the crypto provider
    std::unique_ptr<Crypto> crypto_;
    
    // Default chunk size for processing files (8 MB)
    size_t chunk_size_ = 8 * 1024 * 1024;
    
    // Helper methods
    std::vector<uint8_t> readFileChunk(std::istream& file, size_t size);
    void writeFileChunk(std::ostream& file, const std::vector<uint8_t>& data);
    void processFileInChunks(
        const std::string& sourcePath,
        const std::string& destPath,
        const std::string& password,
        bool encrypting,
        ProgressCallback progressCallback
    );
};

} // namespace crusty

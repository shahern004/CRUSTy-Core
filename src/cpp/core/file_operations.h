#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>
#include <stdexcept>

namespace crusty {

/**
 * @brief Exception class for file operations
 */
class FileOperationException : public std::runtime_error {
public:
    explicit FileOperationException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Class for file operations
 * 
 * This class provides functionality for file operations like selecting files,
 * creating output directories, and managing file paths.
 */
class FileOperations {
public:
    /**
     * @brief Default constructor
     */
    FileOperations() = default;
    
    /**
     * @brief Select a file for encryption/decryption
     * 
     * @param title Dialog title
     * @param filter File filter (e.g., "All Files (*.*)")
     * @param isOpen True for open dialog, false for save dialog
     * @return Selected file path, or empty string if canceled
     */
    static std::string selectFile(
        const std::string& title,
        const std::string& filter,
        bool isOpen = true
    );
    
    /**
     * @brief Select multiple files for encryption/decryption
     * 
     * @param title Dialog title
     * @param filter File filter (e.g., "All Files (*.*)")
     * @return Vector of selected file paths, or empty vector if canceled
     */
    static std::vector<std::string> selectFiles(
        const std::string& title,
        const std::string& filter
    );
    
    /**
     * @brief Select a directory
     * 
     * @param title Dialog title
     * @return Selected directory path, or empty string if canceled
     */
    static std::string selectDirectory(const std::string& title);
    
    /**
     * @brief Get the default output path for an encrypted/decrypted file
     * 
     * @param sourcePath Source file path
     * @param isEncrypting True if encrypting, false if decrypting
     * @return Default output path
     */
    static std::string getDefaultOutputPath(
        const std::string& sourcePath,
        bool isEncrypting
    );
    
    /**
     * @brief Create a directory if it doesn't exist
     * 
     * @param path Directory path
     * @throws FileOperationException if directory creation fails
     */
    static void createDirectory(const std::string& path);
    
    /**
     * @brief Check if a file exists
     * 
     * @param path File path
     * @return True if file exists, false otherwise
     */
    static bool fileExists(const std::string& path);
    
    /**
     * @brief Get the file size
     * 
     * @param path File path
     * @return File size in bytes
     * @throws FileOperationException if file doesn't exist or can't be accessed
     */
    static std::uintmax_t getFileSize(const std::string& path);
    
    /**
     * @brief Get the file name from a path
     * 
     * @param path File path
     * @return File name
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * @brief Get the file extension from a path
     * 
     * @param path File path
     * @return File extension
     */
    static std::string getFileExtension(const std::string& path);
    
    /**
     * @brief Get the directory path from a file path
     * 
     * @param path File path
     * @return Directory path
     */
    static std::string getDirectoryPath(const std::string& path);
};

} // namespace crusty

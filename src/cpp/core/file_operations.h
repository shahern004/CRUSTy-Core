#pragma once

#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace crusty {

/**
 * @brief Exception for file operation errors with specific error information
 */
class FileOperationException : public std::runtime_error {
public:
    enum class ErrorCode {
        FileNotFound,
        AccessDenied,
        DiskFull,
        InvalidPath,
        DirectoryCreationFailed,
        IoError
    };

private:
    ErrorCode error_code_;
    std::string file_path_;

public:
    FileOperationException(
        const std::string& message, 
        ErrorCode code,
        const std::string& path
    ) : std::runtime_error(message), error_code_(code), file_path_(path) {}
    
    ErrorCode getErrorCode() const { return error_code_; }
    const std::string& getFilePath() const { return file_path_; }
};

/**
 * @brief File path utilities
 * 
 * Handles operations related to file paths and extensions
 */
class PathUtil {
public:
    /**
     * @brief Get the file name without directory path
     * 
     * @param path Full file path
     * @return File name component
     */
    static std::string getFileName(std::string_view path);
    
    /**
     * @brief Get the file extension (without the dot)
     * 
     * @param path File path
     * @return File extension or empty string if none
     */
    static std::string getExtension(std::string_view path);
    
    /**
     * @brief Get the directory path from a file path
     * 
     * @param path File path
     * @return Directory path
     */
    static std::string getDirectory(std::string_view path);
    
    /**
     * @brief Create the default output path for encryption/decryption
     * 
     * @param sourcePath Source file path
     * @param isEncrypting True if encrypting, false if decrypting
     * @return Generated output path
     */
    static std::string createOutputPath(
        std::string_view sourcePath,
        bool isEncrypting
    );
    
    /**
     * @brief Generate a unique filename if the original already exists
     * 
     * @param basePath Base path to check
     * @return Unique path with numbered suffix if needed
     */
    static std::string ensureUniqueFilePath(std::string_view basePath);
};

/**
 * @brief File system operations
 * 
 * Handles direct file and directory operations
 */
class FileSystem {
public:
    FileSystem() = default;
    virtual ~FileSystem() = default;
    
    /**
     * @brief Check if a file exists
     * 
     * @param path File path
     * @return True if file exists and is accessible
     */
    virtual bool fileExists(std::string_view path) const;
    
    /**
     * @brief Get file size in bytes
     * 
     * @param path File path
     * @return File size
     * @throws FileOperationException if file doesn't exist or can't be accessed
     */
    virtual std::uintmax_t getFileSize(std::string_view path) const;
    
    /**
     * @brief Create a directory and any needed parent directories
     * 
     * @param path Directory path
     * @throws FileOperationException if creation fails
     */
    virtual void createDirectories(std::string_view path) const;
    
    /**
     * @brief Read a file's contents entirely into memory
     * 
     * @param path File path
     * @return File contents as byte vector
     * @throws FileOperationException on IO errors
     */
    virtual std::vector<uint8_t> readFile(std::string_view path) const;
    
    /**
     * @brief Write data to a file, creating or overwriting
     * 
     * @param path File path
     * @param data Data to write
     * @throws FileOperationException on IO errors
     */
    virtual void writeFile(
        std::string_view path, 
        const std::vector<uint8_t>& data
    ) const;
    
    /**
     * @brief Remove a file
     * 
     * @param path File path
     * @return True if file was removed, false if it didn't exist
     * @throws FileOperationException if file exists but couldn't be removed
     */
    virtual bool removeFile(std::string_view path) const;
};

/**
 * @brief File selection dialog interface
 * 
 * Abstract interface for file selection to separate UI concerns
 */
class FileDialogInterface {
public:
    virtual ~FileDialogInterface() = default;
    
    /**
     * @brief Display dialog to select a file
     * 
     * @param title Dialog title
     * @param filter File filter (format depends on implementation)
     * @param forSaving True for save dialog, false for open dialog
     * @return Selected file path, or empty string if canceled
     */
    virtual std::string selectFile(
        const std::string& title,
        const std::string& filter,
        bool forSaving
    ) const = 0;
    
    /**
     * @brief Display dialog to select multiple files
     * 
     * @param title Dialog title
     * @param filter File filter
     * @return Vector of selected file paths, or empty if canceled
     */
    virtual std::vector<std::string> selectMultipleFiles(
        const std::string& title,
        const std::string& filter
    ) const = 0;
    
    /**
     * @brief Display dialog to select a directory
     * 
     * @param title Dialog title
     * @return Selected directory path, or empty if canceled
     */
    virtual std::string selectDirectory(const std::string& title) const = 0;
};

/**
 * @brief High-level file operations for the application
 */
class FileOperations {
public:
    /**
     * @brief Constructor with default system implementations
     */
    FileOperations();
    
    /**
     * @brief Constructor with custom implementations for testing
     * 
     * @param fileSystem Custom file system implementation
     * @param fileDialog Custom file dialog implementation
     */
    FileOperations(
        std::unique_ptr<FileSystem> fileSystem,
        std::unique_ptr<FileDialogInterface> fileDialog
    );
    
    /**
     * @brief Select a file for encryption/decryption
     * 
     * @param title Dialog title
     * @param filter File filter
     * @param forSaving True for save dialog, false for open dialog
     * @return Selected file path, or empty if canceled
     */
    std::string selectFile(
        const std::string& title,
        const std::string& filter,
        bool forSaving = false
    ) const;
    
    /**
     * @brief Select multiple files for batch processing
     * 
     * @param title Dialog title
     * @param filter File filter
     * @return Selected file paths, or empty if canceled
     */
    std::vector<std::string> selectMultipleFiles(
        const std::string& title,
        const std::string& filter
    ) const;
    
    /**
     * @brief Select a directory for output
     * 
     * @param title Dialog title
     * @return Selected directory, or empty if canceled
     */
    std::string selectDirectory(const std::string& title) const;
    
    /**
     * @brief Ensure a valid output file exists for the operation
     * 
     * Checks if the output path exists and creates parent directories
     * if needed. Generates a default path if none provided.
     * 
     * @param sourcePath Source file path
     * @param outputPath Suggested output path (or empty for default)
     * @param isEncrypting True if encrypting, false if decrypting
     * @param overwrite True to allow overwriting existing files
     * @return Final validated output path
     * @throws FileOperationException if output path cannot be created
     */
    std::string prepareOutputFile(
        const std::string& sourcePath,
        const std::string& outputPath,
        bool isEncrypting,
        bool overwrite = false
    );
    
    /**
     * @brief Create a directory and any needed parent directories
     * 
     * @param path Directory path
     * @throws FileOperationException if creation fails
     */
    void createDirectory(const std::string& path);
    
    /**
     * @brief Check if a file exists
     * 
     * @param path File path
     * @return True if file exists and is accessible
     */
    bool fileExists(const std::string& path);
    
    /**
     * @brief Get file size in bytes
     * 
     * @param path File path
     * @return File size
     * @throws FileOperationException if file doesn't exist or can't be accessed
     */
    std::uintmax_t getFileSize(const std::string& path);
    
    /**
     * @brief Get the file name without directory path
     * 
     * @param path Full file path
     * @return File name component
     */
    std::string getFileName(const std::string& path);
    
    /**
     * @brief Get the file extension (without the dot)
     * 
     * @param path File path
     * @return File extension or empty string if none
     */
    std::string getFileExtension(const std::string& path);
    
    /**
     * @brief Get the directory path from a file path
     * 
     * @param path File path
     * @return Directory path
     */
    std::string getDirectoryPath(const std::string& path);
    
private:
    std::unique_ptr<FileSystem> file_system_;
    std::unique_ptr<FileDialogInterface> file_dialog_;
};

} // namespace crusty

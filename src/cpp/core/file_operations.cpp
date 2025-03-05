#include "file_operations.h"
#include "path_utils.h"
#include "audit_log.h"

#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <fstream>

namespace crusty {

namespace {

QStringList convertToQStringList(const std::vector<std::string>& stringVector) {
    QStringList result;
    for (const auto& str : stringVector) {
        result.append(QString::fromStdString(str));
    }
    return result;
}

std::vector<std::string> convertToStdVector(const QStringList& stringList) {
    std::vector<std::string> result;
    result.reserve(stringList.size());
    for (const auto& str : stringList) {
        result.push_back(str.toStdString());
    }
    return result;
}

QString toQString(const std::string& str) {
    return QString::fromStdString(str);
}

} // anonymous namespace

std::string FileOperations::selectFile(
    const std::string& title,
    const std::string& filter,
    bool forSaving
) const {
    QString selectedFile;
    
    if (!forSaving) {
        selectedFile = QFileDialog::getOpenFileName(
            nullptr,
            toQString(title),
            QDir::homePath(),
            toQString(filter)
        );
    } else {
        selectedFile = QFileDialog::getSaveFileName(
            nullptr,
            toQString(title),
            QDir::homePath(),
            toQString(filter)
        );
    }
    
    std::string result = selectedFile.toStdString();
    
    if (!result.empty()) {
        try {
            // Sanitize the path to prevent directory traversal attacks
            result = PathUtils::sanitizePath(result);
            LOG_INFO("File selected: " + result);
        } catch (const std::exception& e) {
            LOG_WARNING("Invalid file path selected: " + std::string(e.what()));
            return "";
        }
    }
    
    return result;
}

std::vector<std::string> FileOperations::selectMultipleFiles(
    const std::string& title,
    const std::string& filter
) const {
    QStringList selectedFiles = QFileDialog::getOpenFileNames(
        nullptr,
        toQString(title),
        QDir::homePath(),
        toQString(filter)
    );
    
    std::vector<std::string> result = convertToStdVector(selectedFiles);
    
    // Sanitize all paths
    std::vector<std::string> sanitizedPaths;
    sanitizedPaths.reserve(result.size());
    
    for (const auto& path : result) {
        try {
            sanitizedPaths.push_back(PathUtils::sanitizePath(path));
        } catch (const std::exception& e) {
            LOG_WARNING("Invalid file path skipped: " + path + " - " + e.what());
        }
    }
    
    if (!sanitizedPaths.empty()) {
        LOG_INFO("Multiple files selected: " + std::to_string(sanitizedPaths.size()) + " files");
    }
    
    return sanitizedPaths;
}

std::string FileOperations::selectDirectory(const std::string& title) const {
    QString selectedDir = QFileDialog::getExistingDirectory(
        nullptr,
        toQString(title),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    std::string result = selectedDir.toStdString();
    
    if (!result.empty()) {
        try {
            // Sanitize the path to prevent directory traversal attacks
            result = PathUtils::sanitizePath(result);
            LOG_INFO("Directory selected: " + result);
        } catch (const std::exception& e) {
            LOG_WARNING("Invalid directory path selected: " + std::string(e.what()));
            return "";
        }
    }
    
    return result;
}

std::string FileOperations::prepareOutputFile(
    const std::string& sourcePath,
    const std::string& outputPath,
    bool isEncrypting,
    bool overwrite
) {
    std::string result;
    
    if (outputPath.empty()) {
        // Generate default output path
    try {
        // Sanitize the input path
        std::string sanitizedPath = PathUtils::sanitizePath(sourcePath);
        std::filesystem::path path(sanitizedPath);
        std::filesystem::path outputPath;
        
        constexpr auto ENCRYPTED_EXTENSION = ".encrypted";
        constexpr auto DECRYPTED_EXTENSION = ".decrypted";
        
        if (isEncrypting) {
            outputPath = path;
            outputPath += ENCRYPTED_EXTENSION;
            return outputPath.string();
        }
        
        // Decrypting case
        std::string filename = path.filename().string();
        
        if (filename.length() > std::string(ENCRYPTED_EXTENSION).length() &&
            filename.substr(filename.length() - std::string(ENCRYPTED_EXTENSION).length()) == ENCRYPTED_EXTENSION) {
            // Remove .encrypted extension
            size_t extensionPos = filename.length() - std::string(ENCRYPTED_EXTENSION).length();
            outputPath = path.parent_path() / filename.substr(0, extensionPos);
        } else {
            // Add .decrypted extension
            outputPath = path;
            outputPath += DECRYPTED_EXTENSION;
        }
        
        std::string result = outputPath.string();
        LOG_INFO("Generated output path: " + result + " for source: " + sanitizedPath);
        return result;
    } catch (const std::exception& e) {
        LOG_WARNING("Error generating output path: " + std::string(e.what()));
        // Fallback to a safe default in case of error
        return sourcePath + (isEncrypting ? ".encrypted" : ".decrypted");
    }
    }
    
    // If outputPath is provided, use it
    return outputPath;
}

void FileOperations::createDirectory(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        std::filesystem::path dirPath(sanitizedPath);
        
        LOG_INFO("Creating directory: " + sanitizedPath);
        
        if (!std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) {
                std::string errorMsg = "Failed to create directory: " + sanitizedPath;
                LOG_ERROR(errorMsg);
                throw FileOperationException(errorMsg, FileOperationException::ErrorCode::DirectoryCreationFailed, sanitizedPath);
            }
            LOG_INFO("Directory created successfully: " + sanitizedPath);
        } else {
            LOG_INFO("Directory already exists: " + sanitizedPath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::string errorMsg = "Failed to create directory: " + path + " (" + e.what() + ")";
        LOG_ERROR(errorMsg);
        throw FileOperationException(errorMsg, FileOperationException::ErrorCode::DirectoryCreationFailed, path);
    } catch (const std::runtime_error& e) {
        // Path sanitization error
        std::string errorMsg = "Invalid directory path: " + path + " (" + e.what() + ")";
        LOG_ERROR(errorMsg);
        throw FileOperationException(errorMsg, FileOperationException::ErrorCode::InvalidPath, path);
    }
}

bool FileOperations::fileExists(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        bool exists = std::filesystem::exists(sanitizedPath) && 
                      std::filesystem::is_regular_file(sanitizedPath);
        
        if (exists) {
            LOG_INFO("File exists: " + sanitizedPath);
        } else {
            LOG_INFO("File does not exist: " + sanitizedPath);
        }
        
        return exists;
    } catch (const std::exception& e) {
        LOG_WARNING("Error checking if file exists: " + path + " - " + e.what());
        return false;
    }
}

std::uintmax_t FileOperations::getFileSize(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        
        if (!std::filesystem::exists(sanitizedPath)) {
            std::string errorMsg = "File does not exist: " + sanitizedPath;
            LOG_ERROR(errorMsg);
            throw FileOperationException(errorMsg, FileOperationException::ErrorCode::FileNotFound, sanitizedPath);
        }
        
        if (!std::filesystem::is_regular_file(sanitizedPath)) {
            std::string errorMsg = "Path is not a regular file: " + sanitizedPath;
            LOG_ERROR(errorMsg);
            throw FileOperationException(errorMsg, FileOperationException::ErrorCode::InvalidPath, sanitizedPath);
        }
        
        std::uintmax_t size = std::filesystem::file_size(sanitizedPath);
        LOG_INFO("File size for " + sanitizedPath + ": " + std::to_string(size) + " bytes");
        return size;
    } catch (const std::filesystem::filesystem_error& e) {
        std::string errorMsg = "Failed to get file size: " + path + " (" + e.what() + ")";
        LOG_ERROR(errorMsg);
        throw FileOperationException(errorMsg, FileOperationException::ErrorCode::IoError, path);
    } catch (const std::runtime_error& e) {
        // Path sanitization error
        std::string errorMsg = "Invalid file path: " + path + " (" + e.what() + ")";
        LOG_ERROR(errorMsg);
        throw FileOperationException(errorMsg, FileOperationException::ErrorCode::InvalidPath, path);
    }
}

std::string FileOperations::getFileName(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        std::string filename = std::filesystem::path(sanitizedPath).filename().string();
        return filename;
    } catch (const std::exception& e) {
        LOG_WARNING("Error getting file name: " + path + " - " + e.what());
        // Fallback to basic implementation for invalid paths
        return std::filesystem::path(path).filename().string();
    }
}

std::string FileOperations::getFileExtension(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        std::string extension = std::filesystem::path(sanitizedPath).extension().string();
        
        if (!extension.empty() && extension[0] == '.') {
            extension.erase(0, 1);  // Remove the leading dot
        }
        
        return extension;
    } catch (const std::exception& e) {
        LOG_WARNING("Error getting file extension: " + path + " - " + e.what());
        // Fallback to basic implementation for invalid paths
        std::string extension = std::filesystem::path(path).extension().string();
        if (!extension.empty() && extension[0] == '.') {
            extension.erase(0, 1);
        }
        return extension;
    }
}

std::string FileOperations::getDirectoryPath(const std::string& path) {
    try {
        // Sanitize the path
        std::string sanitizedPath = PathUtils::sanitizePath(path);
        std::string dirPath = std::filesystem::path(sanitizedPath).parent_path().string();
        return dirPath;
    } catch (const std::exception& e) {
        LOG_WARNING("Error getting directory path: " + path + " - " + e.what());
        // Fallback to basic implementation for invalid paths
        return std::filesystem::path(path).parent_path().string();
    }
}

} // namespace crusty

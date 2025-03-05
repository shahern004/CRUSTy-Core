#include "file_operations.h"

#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

namespace crusty {

// Select a file for encryption/decryption
std::string FileOperations::selectFile(
    const std::string& title,
    const std::string& filter,
    bool isOpen
) {
    QString selectedFile;
    
    if (isOpen) {
        selectedFile = QFileDialog::getOpenFileName(
            nullptr,
            QString::fromStdString(title),
            QDir::homePath(),
            QString::fromStdString(filter)
        );
    } else {
        selectedFile = QFileDialog::getSaveFileName(
            nullptr,
            QString::fromStdString(title),
            QDir::homePath(),
            QString::fromStdString(filter)
        );
    }
    
    return selectedFile.toStdString();
}

// Select multiple files for encryption/decryption
std::vector<std::string> FileOperations::selectFiles(
    const std::string& title,
    const std::string& filter
) {
    QStringList selectedFiles = QFileDialog::getOpenFileNames(
        nullptr,
        QString::fromStdString(title),
        QDir::homePath(),
        QString::fromStdString(filter)
    );
    
    std::vector<std::string> result;
    for (const QString& file : selectedFiles) {
        result.push_back(file.toStdString());
    }
    
    return result;
}

// Select a directory
std::string FileOperations::selectDirectory(const std::string& title) {
    QString selectedDir = QFileDialog::getExistingDirectory(
        nullptr,
        QString::fromStdString(title),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    return selectedDir.toStdString();
}

// Get the default output path for an encrypted/decrypted file
std::string FileOperations::getDefaultOutputPath(
    const std::string& sourcePath,
    bool isEncrypting
) {
    std::filesystem::path path(sourcePath);
    std::filesystem::path outputPath;
    
    if (isEncrypting) {
        // Add .encrypted extension
        outputPath = path;
        outputPath += ".encrypted";
    } else {
        // Remove .encrypted extension if present, otherwise add .decrypted
        std::string filename = path.filename().string();
        std::string extension = ".encrypted";
        
        if (filename.length() > extension.length() &&
            filename.substr(filename.length() - extension.length()) == extension) {
            // Remove .encrypted extension
            outputPath = path.parent_path() / filename.substr(0, filename.length() - extension.length());
        } else {
            // Add .decrypted extension
            outputPath = path;
            outputPath += ".decrypted";
        }
    }
    
    return outputPath.string();
}

// Create a directory if it doesn't exist
void FileOperations::createDirectory(const std::string& path) {
    std::filesystem::path dirPath(path);
    
    try {
        if (!std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) {
                throw FileOperationException("Failed to create directory: " + path);
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw FileOperationException("Failed to create directory: " + path + " (" + e.what() + ")");
    }
}

// Check if a file exists
bool FileOperations::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

// Get the file size
std::uintmax_t FileOperations::getFileSize(const std::string& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error& e) {
        throw FileOperationException("Failed to get file size: " + path + " (" + e.what() + ")");
    }
}

// Get the file name from a path
std::string FileOperations::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

// Get the file extension from a path
std::string FileOperations::getFileExtension(const std::string& path) {
    std::string extension = std::filesystem::path(path).extension().string();
    
    // Remove the leading dot
    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }
    
    return extension;
}

// Get the directory path from a file path
std::string FileOperations::getDirectoryPath(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

} // namespace crusty

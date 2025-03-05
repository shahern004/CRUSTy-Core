#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace crusty {

/**
 * @brief Utilities for secure path handling
 */
class PathUtils {
public:
    /**
     * @brief Sanitize a file path to prevent directory traversal attacks
     * 
     * Converts the path to its canonical form and ensures it doesn't escape
     * from the allowed base directory.
     * 
     * @param path Path to sanitize
     * @param baseDir Optional base directory that the path must be within
     * @return Sanitized path
     * @throws std::runtime_error if path is invalid or escapes from base directory
     */
    static std::string sanitizePath(std::string_view path, std::string_view baseDir = "") {
        try {
            // Convert to canonical form (resolves "..", ".", and symlinks)
            std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
            
            // If base directory is specified, ensure path is within it
            if (!baseDir.empty()) {
                std::filesystem::path canonicalBaseDir = std::filesystem::weakly_canonical(baseDir);
                
                // Check if the path is within the base directory
                auto baseDirStr = canonicalBaseDir.string();
                auto pathStr = canonicalPath.string();
                
                // Path must start with the base directory
                if (pathStr.length() < baseDirStr.length() || 
                    pathStr.substr(0, baseDirStr.length()) != baseDirStr) {
                    throw std::runtime_error("Path escapes from the allowed directory");
                }
            }
            
            return canonicalPath.string();
        } catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error(std::string("Invalid path: ") + e.what());
        }
    }
    
    /**
     * @brief Check if a file path has a safe extension
     * 
     * @param path Path to check
     * @param allowedExtensions List of allowed extensions (without the dot)
     * @return true if extension is in the allowed list, false otherwise
     */
    static bool hasSafeExtension(std::string_view path, 
                                const std::vector<std::string>& allowedExtensions) {
        std::filesystem::path filePath(path);
        std::string extension = filePath.extension().string();
        
        // Remove the leading dot
        if (!extension.empty() && extension[0] == '.') {
            extension.erase(0, 1);
        }
        
        // Convert to lowercase for case-insensitive comparison
        std::transform(extension.begin(), extension.end(), extension.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Check if extension is in the allowed list
        return std::find(allowedExtensions.begin(), allowedExtensions.end(), extension) 
               != allowedExtensions.end();
    }
    
    /**
     * @brief Create a unique file path if the original already exists
     * 
     * @param basePath Base path to check
     * @return Unique path with numbered suffix if needed
     */
    static std::string ensureUniqueFilePath(std::string_view basePath) {
        std::filesystem::path path(basePath);
        
        if (!std::filesystem::exists(path)) {
            return std::string(basePath);
        }
        
        std::filesystem::path parentPath = path.parent_path();
        std::filesystem::path stem = path.stem();
        std::filesystem::path extension = path.extension();
        
        int counter = 1;
        std::filesystem::path newPath;
        
        do {
            std::string newName = stem.string() + "_" + std::to_string(counter) + extension.string();
            newPath = parentPath / newName;
            counter++;
        } while (std::filesystem::exists(newPath));
        
        return newPath.string();
    }
    
    /**
     * @brief Validate a file path for security
     * 
     * Checks if the path is valid, within allowed directory, and has a safe extension
     * 
     * @param path Path to validate
     * @param baseDir Optional base directory that the path must be within
     * @param allowedExtensions Optional list of allowed extensions
     * @return Sanitized path if valid
     * @throws std::runtime_error if path is invalid
     */
    static std::string validateFilePath(
        std::string_view path, 
        std::string_view baseDir = "",
        const std::vector<std::string>& allowedExtensions = {}
    ) {
        // First sanitize the path
        std::string sanitized = sanitizePath(path, baseDir);
        
        // If allowed extensions are specified, check the extension
        if (!allowedExtensions.empty() && !hasSafeExtension(sanitized, allowedExtensions)) {
            throw std::runtime_error("File has an unsafe extension");
        }
        
        return sanitized;
    }
};

} // namespace crusty

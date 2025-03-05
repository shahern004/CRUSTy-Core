#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace crusty {

/**
 * @brief Secure audit logging system for security-critical operations
 * 
 * Thread-safe singleton that logs security events to a file
 */
class AuditLog {
public:
    /**
     * @brief Types of events that can be logged
     */
    enum class EventType {
        Info,
        Warning,
        Error,
        SecurityEvent
    };
    
    /**
     * @brief Get the singleton instance
     * 
     * @return Reference to the singleton instance
     */
    static AuditLog& getInstance() {
        static AuditLog instance;
        return instance;
    }
    
    /**
     * @brief Log an event
     * 
     * @param type Type of event
     * @param message Message to log
     */
    void log(EventType type, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!logFile_.is_open() && !logPath_.empty()) {
            openLogFile();
        }
        
        if (logFile_.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            
            logFile_ << "[" << ss.str() << "] [" << eventTypeToString(type) << "] " 
                     << message << std::endl;
            logFile_.flush();
        }
    }
    
    /**
     * @brief Set the log file path
     * 
     * @param path Path to the log file
     */
    void setLogFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (logFile_.is_open()) {
            logFile_.close();
        }
        
        logPath_ = path;
        openLogFile();
    }
    
private:
    /**
     * @brief Private constructor for singleton
     */
    AuditLog() {
        // Default log file in user's home directory
        auto homeDir = std::filesystem::path(std::getenv("USERPROFILE") ? std::getenv("USERPROFILE") : 
                                           (std::getenv("HOME") ? std::getenv("HOME") : "."));
        logPath_ = (homeDir / "crusty_audit.log").string();
        openLogFile();
    }
    
    /**
     * @brief Private destructor for singleton
     */
    ~AuditLog() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    /**
     * @brief Open the log file
     */
    void openLogFile() {
        try {
            // Create parent directories if they don't exist
            std::filesystem::path logFilePath(logPath_);
            std::filesystem::create_directories(logFilePath.parent_path());
            
            // Open log file in append mode
            logFile_.open(logPath_, std::ios::app);
            
            if (!logFile_.is_open()) {
                // Failed to open log file
                std::cerr << "Failed to open audit log file: " << logPath_ << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error opening audit log file: " << e.what() << std::endl;
        }
    }
    
    /**
     * @brief Convert event type to string
     * 
     * @param type Event type
     * @return String representation of event type
     */
    std::string eventTypeToString(EventType type) {
        switch (type) {
            case EventType::Info:
                return "INFO";
            case EventType::Warning:
                return "WARNING";
            case EventType::Error:
                return "ERROR";
            case EventType::SecurityEvent:
                return "SECURITY";
            default:
                return "UNKNOWN";
        }
    }
    
    std::mutex mutex_;
    std::ofstream logFile_;
    std::string logPath_;
    
    // Prevent copying and assignment
    AuditLog(const AuditLog&) = delete;
    AuditLog& operator=(const AuditLog&) = delete;
};

// Helper macros for logging
#define LOG_INFO(msg) crusty::AuditLog::getInstance().log(crusty::AuditLog::EventType::Info, msg)
#define LOG_WARNING(msg) crusty::AuditLog::getInstance().log(crusty::AuditLog::EventType::Warning, msg)
#define LOG_ERROR(msg) crusty::AuditLog::getInstance().log(crusty::AuditLog::EventType::Error, msg)
#define LOG_SECURITY(msg) crusty::AuditLog::getInstance().log(crusty::AuditLog::EventType::SecurityEvent, msg)

} // namespace crusty

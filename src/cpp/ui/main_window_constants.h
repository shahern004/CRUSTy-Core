#pragma once

#include <QString>

namespace crusty {
namespace constants {
    // UI Constants
    constexpr int STATUS_MESSAGE_DURATION_MS = 5000;
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 768;
    constexpr int SPLITTER_LEFT_WIDTH = 300;
    constexpr int SPLITTER_RIGHT_WIDTH = 700;
    
    // File Extensions
    constexpr const char* ENCRYPTED_EXTENSION = ".encrypted";
    constexpr const char* DECRYPTED_EXTENSION = ".decrypted";
    
    // Filter Strings
    constexpr const char* ALL_FILES_FILTER = "All Files (*.*)";
    constexpr const char* ENCRYPTED_FILES_FILTER = "Encrypted Files (*.encrypted);;All Files (*.*)";
    
    // Color Constants
    const QString PRIMARY_COLOR = "#2962FF";
    const QString SECONDARY_COLOR = "#FF6D00";
    
    // Column Indices for File Model
    constexpr int FILE_NAME_COLUMN = 0;
    constexpr int FILE_SIZE_COLUMN = 1;
    constexpr int FILE_DATE_COLUMN = 2;
    constexpr int FILE_PATH_COLUMN = 3;
    
    // Column Indices for Device Model
    constexpr int DEVICE_NAME_COLUMN = 0;
    constexpr int DEVICE_TYPE_COLUMN = 1;
    constexpr int DEVICE_STATUS_COLUMN = 2;
    constexpr int DEVICE_ID_COLUMN = 3;
} // namespace constants
} // namespace crusty

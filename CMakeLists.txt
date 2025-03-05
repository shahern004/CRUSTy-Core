cmake_minimum_required(VERSION 3.15)
# This is the CRUSTy-Core repository with a hybrid C++/Rust architecture
project(CRUSTy VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Qt packages
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Set up Corrosion for Rust integration
include(FetchContent)
FetchContent_Declare(
    corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.4
)
FetchContent_MakeAvailable(corrosion)

# Add Rust crypto library
corrosion_import_crate(MANIFEST_PATH rust/crypto/Cargo.toml)

# C++ components
add_library(cpp_components
    src/cpp/core/encryptor.cpp
    src/cpp/core/file_operations.cpp
)
target_include_directories(cpp_components PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
)

# Qt UI components
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(qt_ui
    src/cpp/ui/main_window.cpp
)
target_link_libraries(qt_ui PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
    cpp_components
)
target_include_directories(qt_ui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
)

# Main application
add_executable(crusty_qt src/cpp/main.cpp)
target_link_libraries(crusty_qt PRIVATE 
    cpp_components 
    qt_ui 
    rust_crypto
)

# Install rules
install(TARGETS crusty_qt
    RUNTIME DESTINATION bin
)

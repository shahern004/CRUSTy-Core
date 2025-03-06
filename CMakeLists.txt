cmake_minimum_required(VERSION 3.15)
# This is the CRUSTy-Core repository with a hybrid C++/Rust architecture
project(CRUSTy VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add security-focused compiler flags
if(MSVC)
    add_compile_options(/GS /DYNAMICBASE /NXCOMPAT /CETCOMPAT /Zc:__cplusplus /permissive-)
    # Control Flow Guard for MSVC
    add_compile_options(/guard:cf)
elseif(MINGW)
    # MinGW-specific options (without the unsupported -z flag)
    add_compile_options(-fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE)
    add_link_options(-pie)
else()
    # Linux/Unix options
    add_compile_options(-fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE)
    add_link_options(-Wl,-z,relro,-z,now -pie)
endif()

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Qt packages
option(USE_QT "Enable Qt UI components" ON)
if(USE_QT)
    find_package(Qt6 COMPONENTS Core Gui Widgets)
    if(NOT Qt6_FOUND)
        message(WARNING "Qt6 not found. Building without Qt UI components.")
        set(USE_QT OFF)
    endif()
endif()

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

# Link Windows libraries for Rust crypto
if(MINGW)
    target_link_libraries(rust_crypto INTERFACE 
        ws2_32 
        bcrypt 
        ntdll 
        userenv
    )
endif()

# C++ components
add_library(cpp_components
    src/cpp/core/encryptor.cpp
    src/cpp/core/file_operations.cpp
    src/cpp/core/audit_log.h
    src/cpp/core/path_utils.h
    src/cpp/core/secure_utils.h
)
target_include_directories(cpp_components PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
)

if(USE_QT)
    target_link_libraries(cpp_components PUBLIC 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Widgets
    )
else()
    # Define NO_QT_UI when Qt is not available
    target_compile_definitions(cpp_components PUBLIC NO_QT_UI)
endif()

if(USE_QT)
    # Qt UI components
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    add_library(qt_ui
        src/cpp/ui/main_window.cpp
        src/cpp/ui/main_window_impl.cpp
        src/cpp/resources.qrc
    )
    target_link_libraries(qt_ui PRIVATE 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Widgets
        cpp_components  # Add link to cpp_components library
    )
    target_include_directories(qt_ui PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
    )

    # Main application with Qt UI
    add_executable(crusty_qt src/cpp/main.cpp)
    target_link_libraries(crusty_qt PRIVATE 
        cpp_components 
        qt_ui 
        rust_crypto
    )

    # Install rules for Qt application
    install(TARGETS crusty_qt
        RUNTIME DESTINATION bin
    )
else()
    # Simple CLI application without Qt
    add_executable(crusty_cli src/cpp/main.cpp)
    target_link_libraries(crusty_cli PRIVATE 
        cpp_components 
        rust_crypto
    )
    target_compile_definitions(crusty_cli PRIVATE NO_QT_UI)

    # Install rules for CLI application
    install(TARGETS crusty_cli
        RUNTIME DESTINATION bin
    )
endif()

# Remove duplicate install rule

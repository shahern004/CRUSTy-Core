#pragma once

#include <cstring>
#include <type_traits>
#include <vector>
#include <string>

namespace crusty {
namespace secure {

/**
 * Securely wipes memory containing sensitive data
 * 
 * @tparam T Type of data to wipe
 * @param data Reference to data that should be wiped
 */
template<typename T>
inline void wipe(T& data) {
    volatile T* ptr = &data;
    std::memset(const_cast<T*>(ptr), 0, sizeof(T));
}

/**
 * Specialization for std::vector to wipe its contents
 */
template<typename T>
inline void wipe(std::vector<T>& data) {
    if (data.empty()) return;
    volatile T* ptr = data.data();
    std::memset(const_cast<T*>(ptr), 0, data.size() * sizeof(T));
    data.clear();
}

/**
 * Specialization for std::string to wipe its contents
 */
inline void wipe(std::string& data) {
    if (data.empty()) return;
    volatile char* ptr = data.data();
    std::memset(const_cast<char*>(ptr), 0, data.size());
    data.clear();
}

/**
 * RAII wrapper for secure data that's automatically wiped when it goes out of scope
 */
template<typename T>
class SecureData {
private:
    T data_;

public:
    SecureData() = default;
    explicit SecureData(const T& data) : data_(data) {}
    
    ~SecureData() {
        wipe(data_);
    }
    
    // Access the underlying data
    T& get() { return data_; }
    const T& get() const { return data_; }
    
    // Prevent copying
    SecureData(const SecureData&) = delete;
    SecureData& operator=(const SecureData&) = delete;
    
    // Allow moving
    SecureData(SecureData&& other) noexcept : data_(std::move(other.data_)) {}
    SecureData& operator=(SecureData&& other) noexcept {
        if (this != &other) {
            wipe(data_);
            data_ = std::move(other.data_);
        }
        return *this;
    }
};

} // namespace secure
} // namespace crusty

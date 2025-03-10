#pragma once

#include <stdint.h>
#include <stddef.h>

namespace crusty {
namespace crypto {

enum class CryptoErrorCode {
    Success = 0,
    InvalidParams = -1,
    AuthenticationFailed = -2,
    EncryptionError = -3,
    DecryptionError = -4,
    KeyDerivationError = -5,
    BufferTooSmall = -6,
    InternalError = -7,
    HardwareNotAvailable = -8
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t encrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

int32_t decrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

int32_t hash_password(
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_len
);

#ifdef __cplusplus
}
#endif

} // namespace crypto
} // namespace crusty

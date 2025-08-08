#ifndef STRING_OBFUSCATOR_H_
#define STRING_OBFUSCATOR_H_

#include <string>
#include <array>
#include <cstddef>
#include <utility>

namespace Obfuscation {

// Generates a simple compile-time seed from __TIME__ (format: "HH:MM:SS")
constexpr unsigned int compile_time_seed() {
    const char* t = __TIME__;
    return (t[0] - '0') * 36000 + (t[1] - '0') * 3600 +
           (t[3] - '0') * 600   + (t[4] - '0') * 60 +
           (t[6] - '0') * 10    + (t[7] - '0');
}

template <unsigned int Key, size_t N>
class ObfuscatedString {
public:
    class Decrypted {
    public:
        Decrypted(const Decrypted&) = delete;
        Decrypted& operator=(const Decrypted&) = delete;

        Decrypted(Decrypted&& other) noexcept
            : m_buffer(std::exchange(other.m_buffer, {})) {}

        Decrypted& operator=(Decrypted&& other) noexcept {
            if (this != &other) {
                zero();
                m_buffer = std::exchange(other.m_buffer, {});
            }
            return *this;
        }

        explicit Decrypted(const std::array<char, N>& encrypted) {
            for (size_t i = 0; i < N; ++i) {
                m_buffer[i] = encrypted[i] ^ static_cast<char>(Key + i);
            }
            m_buffer[N] = '\0';
        }

        ~Decrypted() {
            zero();
        }

        void zero() {
            for (volatile char& c : m_buffer) {
                c = 0;
            }
        }

        operator const char*() const { return m_buffer.data(); }
        const char* c_str() const { return m_buffer.data(); }
        std::string str() const { return std::string(m_buffer.data()); }

    private:
        std::array<char, N + 1> m_buffer;
    };

    constexpr ObfuscatedString(const std::array<char, N>& encrypted)
        : m_data(encrypted) {}

    Decrypted decrypt() const {
        return Decrypted(m_data);
    }

private:
    const std::array<char, N> m_data;
};

} // namespace Obfuscation

#define OBF_KEY (static_cast<unsigned char>(Obfuscation::compile_time_seed() + __LINE__))

// Obfuscates a string literal at compile time. Decrypts it at runtime into a stack buffer.
#define OBFSTR(s) \
    ([] { \
        constexpr size_t N = sizeof(s) - 1; \
        constexpr auto key = OBF_KEY; \
        constexpr auto encrypted = [] { \
            std::array<char, N> arr{}; \
            for (size_t i = 0; i < N; ++i) { \
                arr[i] = s[i] ^ static_cast<char>(key + i); \
            } \
            return arr; \
        }(); \
        static const Obfuscation::ObfuscatedString<key, N> obfuscated_string(encrypted); \
        return obfuscated_string.decrypt(); \
    }())

#endif // STRING_OBFUSCATOR_H_

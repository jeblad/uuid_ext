/**
 * uuid_ext – A portable UUID extension library
 *
 * Copyright © 2026 John Erling Blad
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 * 
 * See accompanying file LICENSE or the page at https://www.gnu.org/licenses/
 **/

#include "uuid_ext/uuid.hpp"
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>  // Required for std::string
#include <vector>

namespace uuid_ext {

namespace {

struct Encoding {
    std::string alphabet;
    bool case_insensitive;
};

const std::map<std::string, Encoding>& get_encoding_registry() {
    static const std::map<std::string, Encoding> registry = {
        // RFC 4648 - Section 4: Base 64 Encoding
        {"rfc4648-4",
            {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
                false}},
        // RFC 4648 - Section 6
        // Base 32 Encoding
        {"rfc4648-6",
            {"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567",
                true}},
        // RFC 4648 - Section 7
        // Base 32 Encoding with Extended Hex Alphabet
        {"rfc4648-7",
            {"0123456789ABCDEFGHIJKLMNOPQRSTUV",
                true}},
        // RFC 4648 - Section 8
        // Base 16 Encoding (Standard Hex)
        {"rfc4648-8",
            {"0123456789ABCDEF",
                true}},
        // Common Alphanumeric formats
        {"base36",
            {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                true}},
        {"base62",
            {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
                false}}
    };
    return registry;
}

char to_upper(char c) {
    if (c >= 'a' && c <= 'z') return static_cast<char>(c - ('a' - 'A'));
    return c;
}

}  // namespace

/**
 * @brief Gets the UUID version.
 * 
 * The version is located in the most significant bits (4-7) of the 7th byte.
 * For UUIDv4, this will return 4.
 */
uint8_t UUID::version() const {
    // Version is bits 4-7 of the 7th byte (index 6)
    return static_cast<uint8_t>((to_bytes()[6] >> 4) & 0x0F);
}

/**
 * @brief Gets the UUID variant.
 * 
 * The variant determines the layout of the UUID. Standard RFC 4122 UUIDs return 1.
 */
uint8_t UUID::variant() const {
    // Variant is the top bits of the 9th byte (index 8)
    uint8_t b = to_bytes()[8];
    if ((b & 0x80) == 0)
        return 0;  // 0xxx: NCS backward compatibility
    if ((b & 0x40) == 0)
        return 1;  // 10xx: RFC 4122 (standard)
    if ((b & 0x20) == 0)
        return 2;  // 110x: Microsoft backward compatibility
    return 3;      // 111x: Reserved for future use
}

/**
 * @brief Converts the UUID value to a sequence of digits in a given base.
 * 
 * Uses successive division to decompose the 128-bit value.
 * @param base Base between 2 and 256.
 * @return A vector of digits, where the most significant digit is first.
 */
std::vector<uint8_t> UUID::to_base(int base) const {
    if (base < 2 || base > 256) {
        return {};
    }

    unsigned __int128 n = value_;
    if (n == 0) {
        return {0};
    }

    std::vector<uint8_t> digits;
    digits.reserve(128);  // Conservative estimate for base 2

    while (n > 0) {
        digits.push_back(
            static_cast<uint8_t>(n % static_cast<unsigned __int128>(base)));
        n /= base;
    }
    std::reverse(digits.begin(), digits.end());
    return digits;
}

/**
 * @brief Default constructor that initializes a "nil" UUID (all bits are 0).
 */
UUID::UUID() : value_(0) {}

/**
 * @brief Constructor that parses a hexadecimal string.
 * 
 * Supports both standard format with hyphens (36 characters) and compact format (32 characters).
 * @param val A string representing a UUID in hex.
 */
UUID::UUID(const std::string& val) {
    value_ = 0;
    size_t len = val.length();
    if (len != 36 && len != 32) {
        return;
    }

    unsigned __int128 res = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = val[i];
        if (len == 36 && (i == 8 || i == 13 || i == 18 || i == 23)) {
            if (c != '-') return;
            continue;
        }

        int hex_val;
        if (c >= '0' && c <= '9') hex_val = c - '0';
        else if (c >= 'a' && c <= 'f') hex_val = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') hex_val = c - 'A' + 10;
        else return;

        res = (res << 4) | (hex_val & 0xf);
    }
    value_ = res;
}

/**
 * @brief Constructor from a raw 128-bit value.
 */
UUID::UUID(unsigned __int128 val) : value_(val) {}

/**
 * @brief Gets the internal 128-bit value.
 */
unsigned __int128 UUID::get() const {
    return value_;
}

/**
 * @brief Checks if the UUID is nil (all zeros).
 */
bool UUID::is_nil() const {
    return value_ == 0;
}

/**
 * @brief Equality comparison.
 */
bool UUID::operator==(const UUID& other) const {
    return value_ == other.value_;
}

/**
 * @brief Inequality comparison.
 */
bool UUID::operator!=(const UUID& other) const {
    return !(*this == other);
}

/**
 * @brief Returns the UUID as a compact 32-character hexadecimal string without hyphens.
 */
std::string UUID::to_string() const {
    static const char* hex_chars = "0123456789abcdef";
    std::string s;
    s.reserve(32);
    auto bytes = to_bytes();

    for (size_t i = 0; i < 16; ++i) {
        s += hex_chars[(bytes[i] >> 4) & 0x0f];
        s += hex_chars[bytes[i] & 0x0f];
    }
    return s;
}

/**
 * @brief Converts the UUID to a string in a given base (up to 64).
 * 
 * Uses the alphabet 0-9, A-Z, a-z, +, / in order.
 * Note: This is a numerical conversion and does not necessarily follow RFC ordering.
 */
std::string UUID::to_base_string(int base) const {
    if (base < 2 || base > 64) {
        return "";
    }
    static const char* chars =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
    auto digits = to_base(base);
    if (digits.empty()) return "";
    std::string s;
    s.reserve(digits.size());
    for (auto d : digits) {
        s += chars[d];
    }
    return s;
}

/**
 * @brief Converts the UUID to a string based on a named encoding from the registry.
 * 
 * Supports, among others, "rfc4648-4" (Base64) and "base36".
 */
std::string UUID::to_base_string(const std::string& encoding_id) const {
    const auto& registry = get_encoding_registry();
    auto it = registry.find(encoding_id);
    if (it == registry.end()) {
        return "";
    }

    auto digits = to_base(static_cast<int>(it->second.alphabet.length()));
    if (digits.empty()) return "";
    std::string s;
    s.reserve(digits.size());
    for (auto d : digits) {
        s += it->second.alphabet[d];
    }
    return s;
}

/**
 * @brief Creates a UUID from a string in a given base (numerical mapping).
 * 
 * Used for direct conversion where the character's position in the standard alphabet corresponds to its value.
 */
UUID UUID::from_base_string(
    const std::string& s,
    int base
) {
    if (base < 2 || base > 64) {
        return UUID();
    }
    static const std::string chars =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
    unsigned __int128 val = 0;
    for (char c : s) {
        size_t pos = chars.find(c);
        if (pos == std::string::npos || pos >= static_cast<size_t>(base)) {
            return UUID();
        }
        val = val * static_cast<unsigned __int128>(base)
            + static_cast<unsigned __int128>(pos);
    }
    return UUID(val);
}

/**
 * @brief Creates a UUID from a string using a specific encoding (e.g., Base32 or Base64).
 * 
 * Takes case-insensitivity into account if the encoding in the registry is marked as such.
 */
UUID UUID::from_base_string(
    const std::string& s,
    const std::string& encoding_id
) {
    const auto& registry = get_encoding_registry();
    auto it = registry.find(encoding_id);
    if (it == registry.end()) {
        return UUID();
    }

    const auto& enc = it->second;
    unsigned __int128 val = 0;
    unsigned __int128 b = static_cast<unsigned __int128>(enc.alphabet.length());

    if (b < 2) {
        return UUID();
    }

    for (char c : s) {
        char lookup = enc.case_insensitive ? to_upper(c) : c;
        size_t pos = enc.alphabet.find(lookup);

        if (pos == std::string::npos) {
            // If case-insensitive,
            // also try original char just in case alphabet was lower
            pos = enc.alphabet.find(c);
        }

        if (pos == std::string::npos) {
            return UUID();
        }
        val = val * b + static_cast<unsigned __int128>(pos);
    }
    return UUID(val);
}

/**
 * @brief Heuristically parses a string based on its length.
 * 
 * The detection logic is:
 * - 32/36 chars: Standard Hex (UUID)
 * - 26 chars: RFC 4648 Base32
 * - 25 chars: Base36 (Alphanumeric)
 * - 24 chars: Base36
 * - 22 chars: Base64 (Unpadded) or Base62
 * @return A valid UUID on success, or a nil-UUID (0) on failure.
 */
UUID UUID::parse(
    const std::string& s
) {
    if (s.empty()) return UUID();
    size_t len = s.length();

    if (len == 36 || len == 32) return UUID(s);
    if (len == 26) return from_base_string(s, "rfc4648-6");
    if (len == 25) return from_base_string(s, "base36");
    
    if (len == 24) {
        // Base64 padding is dropped; 24 chars is handled as Base36.
        UUID u = from_base_string(s, "base36");
        if (!u.is_nil()) return u;
        return UUID();
    }
    
    if (len == 22) {
        // Ambiguous: Could be Base62 or Base64 (without padding).
        // Prioritize Base64 as it's a common encoding for binary data like UUIDs.
        UUID u = from_base_string(s, "rfc4648-4");
        if (!u.is_nil()) return u;
    
        // If not valid Base64, try Base62.
        u = from_base_string(s, "base62");
        if (!u.is_nil()) return u;
        return UUID();
    }
    
    return UUID();
}

/**
 * @brief Exports the 128-bit value as an array of 16 bytes.
 * Uses big-endian (network byte order) format.
 */
std::array<uint8_t, 16> UUID::to_bytes() const {
    std::array<uint8_t, 16> bytes;
    unsigned __int128 temp_value = value_;
    for (int i = 0; i < 16; ++i) {
        // Extract bytes in big-endian order
        bytes[i] = static_cast<uint8_t>((temp_value >> (8 * (15 - i))) & 0xFF);
    }
    return bytes;
}

}  // namespace uuid_ext

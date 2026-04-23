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

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace uuid_ext {

class UUID {
 private:
      unsigned __int128 value_;

 public:
      // Default constructor (initializes to zero UUID)
      UUID();

      // Constructor from standard string representation
      // (e.g., "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx")
      explicit UUID(const std::string& val);

      // Constructor from unsigned __int128
      explicit UUID(unsigned __int128 val);

      // Introspection
      uint8_t version() const;
      uint8_t variant() const;

      // Get the raw 128-bit value
      unsigned __int128 get() const;

      // Returns the library version string
      // (e.g. "1.0.abc1234-dirty" if there are uncommitted changes)
      static std::string version_string();

      // Returns true if the UUID is all zeros (nil)
      bool is_nil() const;

      // Comparison operators
      bool operator==(const UUID& other) const;
      bool operator!=(const UUID& other) const;

      // Convert to standard UUID string representation
      std::string to_string() const;

      /**
       * @brief Returns the digits of the UUID in the specified base.
       */
      std::vector<uint8_t> to_base(int base) const;

      // Convert to a string in a specified base
      // (e.g., base 10, base 16, base 36)
      // This uses a default alphanumeric alphabet.
      std::string to_base_string(int base) const;

      /**
       * @brief Convert to a string using a specific named encoding (e.g. "rfc4648-7").
       */
      std::string to_base_string(const std::string& encoding_id) const;

      /**
       * @brief Creates a UUID from a string in the specified base.
       */
      static UUID from_base_string(
         const std::string& s,
         int base);

      /**
       * @brief Creates a UUID from a string using a specific named encoding.
       */
      static UUID from_base_string(
         const std::string& s,
         const std::string& encoding_id);

      /**
       * @brief Heuristically parses a string into a UUID based on string length.
       */
      static UUID parse(
         const std::string& s);

      // Convert to an array of 16 bytes
      std::array<uint8_t, 16> to_bytes() const;
};

}  // namespace uuid_ext

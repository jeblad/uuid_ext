# uuid_ext

**uuid_ext is a portable, header-only C++20 library for advanced UUID formatting and heuristic parsing.**

## Overview

`uuid_ext` is designed to bridge the gap between standard UUID representations and human-friendly or web-safe formats. It provides an efficient `UUID` container backed by a 128-bit integer (`unsigned __int128`) and includes a powerful heuristic parser capable of identifying encodings based on textual characteristics.

### Why use uuid_ext?

* **Internationalization (I18n):** Unlike many libraries, `uuid_ext` is fully UTF-8 aware. It supports localized alphabets (e.g., Norwegian `ÆØÅ`) and custom translation tables for case-insensitive processing of non-ASCII characters.
* **Heuristic Parsing:** The library can guess the format of an incoming string (Hex, Base32, Base36, etc.) based on length and content, simplifying the ingestion of identifiers from various sources.
* **Environment Safe:** Focuses on alphanumeric-heavy representations that are safe for URLs, Shell commands, and Filenames without requiring complex escaping.
* **Modern C++:** Zero-dependency, header-only architecture leveraging C++20 ranges and `std::string_view`.

## Integration

`uuid_ext` is a single-header library. You can either copy `include/uuid_ext/uuid_ext.hpp` into your project or use the provided CMake interface:

```cmake
# Add the directory to your include paths
target_link_libraries(your_project PRIVATE uuid_ext)
```

## 3. Usage

```cpp
#include "uuid_ext/uuid_ext.hpp"
#include <iostream>

int main() {
    using uuid_ext::UUID;

    // 1. Create from standard hex
    UUID u1("550e8400-e29b-41d4-a716-446655440000");

    // 2. Heuristically parse unknown formats (e.g., Base36)
    UUID u2 = UUID::parse("17Y9G6X8W4Q2Z0V4B8N6M4L2K");

    // 3. Convert to specialized encodings
    std::cout << "Base36: " << u1.to_base_string("base36") << "\n";
    std::cout << "Base62: " << u1.to_base_string("base62") << "\n";

    return 0;
}
```

## Algorithm Overview

The library is built around a few core algorithmic principles to ensure high-fidelity conversion and robust parsing:

* **128-bit Integer Math:** The core storage uses `unsigned __int128`. All base conversions (Base36, Base62, etc.) are performed using successive division and modulo operations on this 128-bit value, treating the UUID as a single big-endian integer.
+* **Heuristic Classification:** The `parse()` method implements a prioritized decision tree based on input length. It attempts to classify strings into formats like RFC 4648 Base32 (26 characters), Base36 (24-25 characters), or Base64/Base62 (22 characters).
* **UTF-8 Decoding:** Unlike byte-oriented libraries, `uuid_ext` includes a lightweight UTF-8 decoder. It processes input strings as 32-bit Unicode code points. This allows the library to treat multibyte characters (like `Æ` or `Å`) as single semantic units during both encoding and decoding.
* **Translation Mapping:** Before a character is looked up in an encoding's alphabet, it passes through a translation layer. This layer handles custom case-folding and character aliasing (e.g., mapping `æ` to `Æ` for a Norwegian-based identifier), allowing the library to remain robust against inconsistent human input or varied linguistic rules.

This layer handles custom case-folding and character aliasing (e.g., mapping æ to Æ for a Norwegian-based identifier), allowing the library to remain robust against inconsistent human input or varied linguistic rules.

## Technical Deep Dive

### Memory and Exceptions

`uuid_ext` is designed for general-purpose C++ applications. Please note the following regarding its operational profile:

* **Heap Allocation:** Several methods (like `parse`, `to_string`, and base conversions) utilize STL containers such as `std::string` and `std::vector`. These trigger dynamic memory allocations on the heap.
* **Exception Safety:** While the library avoids explicit `throw` statements for logic errors, it is **not** `noexcept`. It relies on the STL, which may throw `std::bad_alloc` if memory is exhausted. It is the caller's responsibility to handle this, if operating in memory-constrained environments.
* **Static Registry:** The encoding registry uses a `std::map` initialized on the first call, which involves one-time heap allocations.

### Heuristic Parsing Logic

The parse() method attempts to identify the format based on the following length-based rules:

* 32 or 36 characters: Standard Hex (with or without hyphens).
* 26 characters: RFC 4648 Base32.
* 24 or 25 characters: Base36 (Alphanumeric).
* 22 characters: Base64 (Unpadded) or Base62.

### UTF-8 & Custom Encodings

`uuid_ext` processes strings as Unicode code points, not raw bytes. This allows for identifiers using localized alphabets.

* Translation Tables: Encodings can define a translations map to handle characters without standard ASCII equivalents (e.g., mapping lowercase æ to uppercase Æ in a numeric context).
* Example: Base-39-norwegian: This built-in encoding extends the alphanumeric set with Æ, Ø, and Å. It is ideal for systems requiring verbal command robustness in Norwegian contexts.

## Building and Testing

### Development Dependencies

This project uses `commit-and-tag-version` to automate versioning via Git hooks. If you are contributing, ensure you have Node.js installed and run:

```bash
npm install -g commit-and-tag-version
```

This project uses CMake for its build system.

```bash
cmake -B build
cmake --build build
```

To run the included unit tests, use CTest after building:

```bash
ctest --test-dir build
```

## License

This project is licensed under the **GNU General Public License v3.0**. See the `LICENSE` file for details.

## Acknowledgements

*Created with assistance from AI tools (Gemini 2.5, 3.0, and 3.1, in both Flash and Pro versions) across all parts of this work.*

This project was developed independently, with no external financial or institutional support other than the AI tools mentioned. The views and conclusions contained herein are those of the author(s) and should not be interpreted as representing the official policies or endorsements, either expressed or implied, of any external agency or entity.

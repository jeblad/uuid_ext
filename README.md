# uuid_ext
A portable C++11 library for handling, formatting, and heuristically parsing UUIDs.

## Overview
`uuid_ext` provides a `UUID` class backed by an `unsigned __int128` for efficient internal storage. It is designed to bridge the gap between various UUID representations, including standard hyphenated Hex, compact Hex, Base32, Base36, Base62, and Base64.

## Features
- **Multi-base support**: Convert UUIDs to and from Base16, Base32 (RFC 4648), Base36, Base62, and Base64.
- **Heuristic Parsing**: A `parse()` method that guesses the encoding based on string length and character content.
- **Clean API**: Supports introspection (version/variant), byte array export, and comparison operators.
- **Zero External Dependencies**: Only requires a standard C++11 compiler.

## Heuristic Parsing Logic
The `parse()` method attempts to identify the format based on the following length-based rules:
- **32 or 36 chars**: Standard Hex (with or without hyphens).
- **26 chars**: RFC 4648 Base32.
- **25 chars**: Base36 (Alphanumeric).
- **24 chars**: Base36 (Alphanumeric).
- **22 chars**: Base64 (Unpadded) or Base62.

## Limitations
- **Performance**: This library is optimized for flexibility and code clarity rather than extreme throughput. It uses `std::map` and `std::string` operations that may not be suitable for high-frequency inner loops.
- **Heuristic Ambiguity**: Because parsing is primarily length-based, a string intended to be Base64 that happens to only contain alphanumeric characters might be misidentified as Base36 or Base62. For unambiguous parsing, use `from_base_string()` with a specific encoding ID.

### Why certain encodings are troublesome
Base64 padding (`=`) and certain special characters (like `+` and `/`) are often problematic in modern software stacks:
- **URLs and Web**: Characters like `=`, `+`, and `/` have special meanings in URLs. Including them in identifiers requires percent-encoding, which makes the strings longer and harder to read.
- **Command Line**: Shells often interpret `=` as an assignment or require complex quoting for strings containing symbols, making it difficult to pass identifiers as arguments.
- **Filenames**: While most filesystems allow these characters, they can interfere with standard CLI tools or path resolution logic.

By focusing on unpadded and "clean" alphanumeric-heavy representations, `uuid_ext` ensures that generated strings are safe for use in filenames, URLs, and terminal commands without additional escaping.

## Building
The library uses CMake.

```bash
cmake -B build
cmake --build build
```

## License
This project is licensed under the **GNU General Public License v3.0**. See the `LICENSE` file for details.

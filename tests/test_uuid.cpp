#include "uuid_ext.hpp"
#include <iostream>
#include <cassert>

void test_heuristics() {
    using uuid_ext::UUID;

    // Verify Versioning
    std::cout << "Library Version: " << UUID::version_string() << std::endl;

    // Test Standard Hex (32 chars)
    std::string hex_raw = "550e8400e29b41d4a716446655440000";
    auto u1 = UUID::parse(hex_raw);
    assert(!u1.is_nil());
    assert(u1.to_string() == hex_raw);

    // Test Base36 (25 chars)
    std::string b36 = "17Y9G6X8W4Q2Z0V4B8N6M4L2K"; 
    auto u2 = UUID::parse(b36, true, "base36");
    assert(!u2.is_nil());

    // Test Base64 (22 chars) - parse prioritizes rfc4648-4
    std::string b64 = "BROEAOKbQdSnFkRmVUQAA+"; // 22 chars, include symbol to force B64
    auto u3 = UUID::parse(b64, true);
    assert(!u3.is_nil());
    
    // Verify that it can round-trip through the explicit encoder
    assert(u3.to_base_string("rfc4648-4") == b64);

    // Test Base62 (22 chars)
    std::string b62 = u3.to_base_string("base62");
    auto u4 = UUID::parse(b62, true);
    assert(u4 == u3);

    // Test comparison
    assert(u1 != u2);
}

void test_ambiguity() {
    using uuid_ext::UUID;

    // A 22-character string is ambiguous between Base62 and Base64 (rfc4648-4)
    std::string input22 = "BROEAOKbQdSnFkRmVUQAA+";

    // Test 1: Expect nil when heuristics are off and no default is specified
    auto u_ambig1 = UUID::parse(input22, false, "");
    assert(u_ambig1.is_nil());
    std::cout << "Verified: Returned nil for ambiguous length 22 (heuristics off)." << std::endl;

    // Test 2: Expect nil when default_encoding is missing and heuristics cannot resolve ambiguity.
    // Length 25 is ambiguous between base36 and base39-norwegian. A string without Norwegian characters
    // will not be distinguishable via heuristics if it is valid in both.
    std::string input25 = "17Y9G6X8W4Q2Z0V4B8N6M4L2K";
    auto u_ambig2 = UUID::parse(input25, true, "");
    assert(u_ambig2.is_nil());
    std::cout << "Verified: Returned nil for ambiguous length 25." << std::endl;

    // Verify that we can override ambiguity by specifying default_encoding explicitly
    auto resolved = UUID::parse(input25, false, "base36");
    assert(!resolved.is_nil());
    std::cout << "Resolved ambiguity using default_encoding." << std::endl;
}

int main() {
    test_heuristics();
    test_ambiguity();
    std::cout << "All uuid_ext tests completed successfully!" << std::endl;
    return 0;
}

#include "uuid_ext/uuid.hpp"
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
    auto u2 = UUID::parse(b36);
    assert(!u2.is_nil());

    // Test comparison
    assert(u1 != u2);
    
    std::cout << "All uuid_ext tests passed!" << std::endl;
}

int main() {
    test_heuristics();
    return 0;
}

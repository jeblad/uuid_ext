#!/bin/bash
set -e

# Get the project root directory
REPO_ROOT=$(git rev-parse --show-toplevel)
cd "$REPO_ROOT"

echo "Running pre-commit version sync and tests..."

# 1. Trigger CMake to re-run the configuration and header generation
cmake -B build -S .

# 2. Build and run tests to ensure the header is valid
cmake --build build --target uuid_test
ctest --test-dir build --output-on-failure

# 3. Stage the newly generated header if it was changed
git add include/uuid_ext/uuid_ext.hpp

echo "Pre-commit checks passed!"
exit 0

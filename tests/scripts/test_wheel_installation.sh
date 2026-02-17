#!/bin/bash
# Test Python bindings using installed wheel
# This script builds a wheel, installs it, and runs all Python tests against it

set -e

echo "==================================="
echo "Testing with Installed Wheel"
echo "==================================="
echo ""

# Get repository root (assuming script is in tests/scripts/)
REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO_ROOT"

# Step 1: Build the wheel
echo "Step 1: Building wheel..."
if [ ! -f "pyproject.toml" ]; then
    echo "ERROR: pyproject.toml not found. Run this from the repository root." >&2
    exit 1
fi

# Clean previous builds
rm -rf dist build

python3 -m build --wheel
if [ $? -ne 0 ]; then
    echo "ERROR: Wheel build failed" >&2
    exit 1
fi
echo "Wheel built successfully"
echo ""

# Step 2: Find the built wheel
WHEEL_FILE=$(ls dist/*.whl | head -n 1)
if [ -z "$WHEEL_FILE" ]; then
    echo "ERROR: No wheel file found in dist/" >&2
    exit 1
fi
echo "Found wheel: $(basename "$WHEEL_FILE")"
echo ""

# Step 3: Create a clean virtual environment
echo "Step 2: Creating clean virtual environment..."
TEST_ENV="$REPO_ROOT/test_wheel_env"
if [ -d "$TEST_ENV" ]; then
    rm -rf "$TEST_ENV"
fi
python3 -m venv "$TEST_ENV"

# Activate virtual environment
source "$TEST_ENV/bin/activate"
echo "Virtual environment activated"
echo ""

# Step 4: Install the wheel
echo "Step 3: Installing wheel..."
pip install --upgrade pip
pip install "$WHEEL_FILE"
if [ $? -ne 0 ]; then
    echo "ERROR: Wheel installation failed" >&2
    exit 1
fi
echo "Wheel installed successfully"
echo ""

# Step 5: Verify installation
echo "Step 4: Verifying installation..."
python3 -c "import game_audio; print(f'Successfully imported game_audio version {game_audio.__version__}')"
if [ $? -ne 0 ]; then
    echo "ERROR: Import verification failed" >&2
    exit 1
fi
echo ""

# Step 6: Run all Python tests
echo "Step 5: Running Python tests against installed wheel..."
echo ""

TEST_FILES=(
    "test_audio_initialization"
    "test_audio_logging"
    "test_audio_volume"
    "test_audio_groups"
    "test_audio_sounds"
    "test_audio_tracks"
    "test_audio_validation"
    "test_audio_threading"
    "test_audio_resources"
)

TEST_RESULTS=()
ALL_PASSED=true

# Change to temp directory to avoid importing local source
TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

for test_name in "${TEST_FILES[@]}"; do
    echo "Running $test_name..."
    TEST_PATH="$REPO_ROOT/tests/python/${test_name}.py"
    if [ -f "$TEST_PATH" ]; then
        python3 "$TEST_PATH"
        RESULT=$?
        TEST_RESULTS+=("$RESULT")
        if [ $RESULT -ne 0 ]; then
            ALL_PASSED=false
        fi
    else
        echo "ERROR: ${test_name}.py not found at $TEST_PATH" >&2
        TEST_RESULTS+=(1)
        ALL_PASSED=false
    fi
    echo ""
done

cd "$REPO_ROOT"
rm -rf "$TEMP_DIR"

# Step 7: Summary
echo "==================================="
echo "Test Summary"
echo "==================================="
echo ""

INDEX=0
for test_name in "${TEST_FILES[@]}"; do
    if [ "${TEST_RESULTS[$INDEX]}" -eq 0 ]; then
        echo "$test_name : PASSED"
    else
        echo "$test_name : FAILED"
    fi
    ((INDEX++))
done

echo ""

# Cleanup
echo "Cleaning up..."
deactivate
rm -rf "$TEST_ENV"

if [ "$ALL_PASSED" = true ]; then
    echo "All tests passed with installed wheel!"
    exit 0
else
    echo "Some tests failed"
    exit 1
fi

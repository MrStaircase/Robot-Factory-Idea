
#!/usr/bin/env bash

set -e

# --------------------------------------------------
# Configuration
# --------------------------------------------------

BUILD_DIR="build"
BUILD_TYPE="Debug"

# --------------------------------------------------
# Functions
# --------------------------------------------------

configure() {
    echo "Configuring project..."

    cmake -S . -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
}

build() {
    configure

    echo "Building project..."
    cmake --build "$BUILD_DIR" --parallel
}

run_game() {
    build

    echo "Starting game..."

    if [[ -f "$BUILD_DIR/main.exe" ]]; then
        "$BUILD_DIR/main.exe"
    elif [[ -f "$BUILD_DIR/Debug/main.exe" ]]; then
        "$BUILD_DIR/Debug/main.exe"
    else
        "$BUILD_DIR/main"
    fi
}

run_tests() {
    build

    echo "Running tests..."
    ctest --test-dir "$BUILD_DIR" --output-on-failure
}

clean() {
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
}

show_help() {
    echo "Usage: ./build.sh [OPTION]"
    echo
    echo "Options:"
    echo "  -b, --build    Build project"
    echo "  -r, --run      Build and run game"
    echo "  -t, --test     Build and run tests"
    echo "  -c, --clean    Remove build directory"
    echo "  -h, --help     Show this help message"
}

menu() {
    echo
    echo "========== MyGame Build Manager =========="
    echo "1. Build project"
    echo "2. Run game"
    echo "3. Run tests"
    echo "4. Clean build"
    echo "5. Exit"
    echo "==========================================="

    read -rp "Select an option: " choice

    case "$choice" in
        1) build ;;
        2) run_game ;;
        3) run_tests ;;
        4) clean ;;
        5) exit 0 ;;
        *) echo "Invalid option." ;;
    esac
}

# --------------------------------------------------
# Main
# --------------------------------------------------

case "${1:---menu}" in
    -b|--build)
        build
        ;;
    -r|--run)
        run_game
        ;;
    -t|--test)
        run_tests
        ;;
    -c|--clean)
        clean
        ;;
    -h|--help)
        show_help
        ;;
    --menu)
        menu
        ;;
    *)
        echo "Unknown option: $1"
        echo
        show_help
        exit 1
        ;;
esac

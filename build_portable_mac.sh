#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------
# Portable macOS build for idler
# Requirements: Qt6 (brew install qt), CMake, Xcode CLT
# Output: build/idler.dmg  — drag idler.app to /Applications
# ---------------------------------------------------------------

BUILD_DIR="build/mac"
APP_PATH="$BUILD_DIR/idler.app"

# Locate Qt via Homebrew if not already in PATH
if ! command -v macdeployqt &>/dev/null; then
    BREW_QT="$(brew --prefix qt 2>/dev/null || true)"
    if [[ -n "$BREW_QT" ]]; then
        export PATH="$BREW_QT/bin:$PATH"
        export CMAKE_PREFIX_PATH="$BREW_QT"
    else
        echo "Error: macdeployqt not found. Install Qt with: brew install qt"
        exit 1
    fi
fi

echo "[1/3] Configuring..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    ${CMAKE_PREFIX_PATH:+-DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"}

echo "[2/3] Building..."
cmake --build "$BUILD_DIR" --config Release

echo "[3/3] Bundling Qt frameworks..."
macdeployqt "$APP_PATH" \
    -no-strip \
    -dmg

# macdeployqt places the .dmg next to the .app
DMG_SRC="$BUILD_DIR/idler.dmg"
DMG_DST="build/idler.dmg"
if [[ -f "$DMG_SRC" ]]; then
    mv "$DMG_SRC" "$DMG_DST"
fi

echo ""
echo "Done!"
echo "  App bundle : $APP_PATH"
echo "  DMG        : $DMG_DST"
echo ""
echo "NOTE: On first launch macOS will ask for Accessibility permission."
echo "  Go to: System Settings → Privacy & Security → Accessibility"
echo "  Enable idler to allow it to send mouse clicks."

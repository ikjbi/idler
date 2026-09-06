#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------
# Portable macOS build for idler
# Requirements: Qt6 (brew install qt), CMake, Xcode CLT
# Output: build/idler.dmg  — drag idler.app to /Applications
# ---------------------------------------------------------------

BUILD_DIR="build/mac"
APP_PATH="$BUILD_DIR/idler.app"
DMG_DST="build/idler.dmg"

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

QT_LIB_PATH="${CMAKE_PREFIX_PATH:-$(brew --prefix qt)}/lib"

echo "[1/5] Configuring..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    ${CMAKE_PREFIX_PATH:+-DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"}

echo "[2/5] Building..."
cmake --build "$BUILD_DIR" --config Release

echo "[3/5] Bundling Qt frameworks..."
# Run macdeployqt without -dmg; codesign errors here are expected and handled below.
macdeployqt "$APP_PATH" -libpath="$QT_LIB_PATH" -no-strip || true

echo "[4/5] Signing..."
# Clear all extended attributes from the whole bundle (including newly-copied frameworks),
# then ad-hoc sign everything. Must happen AFTER macdeployqt so newly-copied files are covered.
chmod -R u+w "$APP_PATH"
xattr -cr "$APP_PATH"
codesign --force --deep --sign - "$APP_PATH"

echo "[5/5] Creating DMG..."
rm -f "$DMG_DST"
hdiutil create \
    -volname "idler" \
    -srcfolder "$APP_PATH" \
    -ov \
    -format UDZO \
    "$DMG_DST"

echo ""
echo "Done!"
echo "  App bundle : $APP_PATH"
echo "  DMG        : $DMG_DST"
echo ""
echo "NOTE: On first launch macOS will ask for Accessibility permission."
echo "  Go to: System Settings → Privacy & Security → Accessibility"
echo "  Enable idler to allow it to send mouse clicks."

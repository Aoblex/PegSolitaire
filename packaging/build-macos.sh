#!/bin/bash
# macOS Build Script for Peg Solitaire
# Usage: ./build-macos.sh [version]

set -e

VERSION=${1:-"1.0.0"}
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-release-macos"
BUNDLE_NAME="PegSolitaire.app"
DMG_NAME="PegSolitaire-${VERSION}-macOS.dmg"

echo "Building Peg Solitaire v$VERSION for macOS..."
echo "Project directory: $PROJECT_DIR"
echo "Build directory: $BUILD_DIR"

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is required but not installed"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make is required but not installed"
    exit 1
fi

if ! command -v macdeployqt &> /dev/null; then
    echo "Error: macdeployqt is required but not installed"
    echo "Install Qt and ensure macdeployqt is in your PATH"
    exit 1
fi

# Clean and create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for Release build
echo "Configuring CMake..."
cmake "$PROJECT_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/install"

# Build the project
echo "Building project..."
make -j$(sysctl -n hw.ncpu)

# Install to staging directory
echo "Installing to staging directory..."
make install

# Find the app bundle
APP_BUNDLE="$BUILD_DIR/install/$BUNDLE_NAME"
if [ ! -d "$APP_BUNDLE" ]; then
    echo "Error: App bundle not found at $APP_BUNDLE"
    exit 1
fi

# Deploy Qt frameworks
echo "Deploying Qt frameworks..."
macdeployqt "$APP_BUNDLE" -dmg -always-overwrite

# Move the DMG to a more descriptive name
if [ -f "$BUILD_DIR/install/PegSolitaire.dmg" ]; then
    mv "$BUILD_DIR/install/PegSolitaire.dmg" "$BUILD_DIR/$DMG_NAME"
    echo "DMG created successfully: $BUILD_DIR/$DMG_NAME"
else
    echo "Warning: DMG not created by macdeployqt, creating manually..."
    
    # Create DMG manually if macdeployqt didn't create one
    DMG_TEMP_DIR="$BUILD_DIR/dmg_temp"
    mkdir -p "$DMG_TEMP_DIR"
    cp -R "$APP_BUNDLE" "$DMG_TEMP_DIR/"
    
    # Create a symbolic link to Applications
    ln -s /Applications "$DMG_TEMP_DIR/Applications"
    
    # Create the DMG
    hdiutil create -volname "Peg Solitaire" \
        -srcfolder "$DMG_TEMP_DIR" \
        -ov -format UDZO \
        "$BUILD_DIR/$DMG_NAME"
    
    # Clean up temporary directory
    rm -rf "$DMG_TEMP_DIR"
    
    echo "DMG created successfully: $BUILD_DIR/$DMG_NAME"
fi

# Verify the app bundle
echo "Verifying app bundle..."
if [ -d "$APP_BUNDLE" ]; then
    echo "✓ App bundle created successfully"
    echo "Bundle size: $(du -sh "$APP_BUNDLE" | cut -f1)"
    
    # Check code signing (informational)
    if command -v codesign &> /dev/null; then
        echo "Code signing status:"
        codesign -dv "$APP_BUNDLE" 2>&1 || echo "  Not code signed"
    fi
else
    echo "✗ App bundle creation failed"
    exit 1
fi

# Final summary
echo ""
echo "Build completed successfully!"
echo "App bundle: $APP_BUNDLE"
echo "DMG file: $BUILD_DIR/$DMG_NAME"
echo ""
echo "To test the app bundle:"
echo "  open '$APP_BUNDLE'"
echo ""
echo "To install from DMG:"
echo "  open '$BUILD_DIR/$DMG_NAME'"

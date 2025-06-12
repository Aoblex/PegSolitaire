#!/bin/bash
# Linux Build Script for Peg Solitaire
# Usage: ./build-linux.sh [version] [format]
# Formats: appimage, deb, rpm, tar

set -e

VERSION=${1:-"1.0.0"}
FORMAT=${2:-"appimage"}
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-release-linux"
INSTALL_DIR="$BUILD_DIR/install"
PACKAGE_NAME="pegsolitaire"

echo "Building Peg Solitaire v$VERSION for Linux ($FORMAT)..."
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

# Clean and create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for Release build
echo "Configuring CMake..."
cmake "$PROJECT_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="/usr" \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DCPACK_PACKAGE_VERSION="$VERSION"

# Build the project
echo "Building project..."
make -j$(nproc)

# Install to staging directory
echo "Installing to staging directory..."
DESTDIR="$INSTALL_DIR" make install

# Define package creation functions
function create_appimage() {
    echo "Creating AppImage..."
    
    # Check for linuxdeploy
    if ! command -v linuxdeploy &> /dev/null; then
        echo "Downloading linuxdeploy..."
        wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O linuxdeploy
        chmod +x linuxdeploy
        LINUXDEPLOY="./linuxdeploy"
    else
        LINUXDEPLOY="linuxdeploy"
    fi
    
    # Check for Qt plugin
    if ! command -v linuxdeploy-plugin-qt &> /dev/null; then
        echo "Downloading linuxdeploy Qt plugin..."
        wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O linuxdeploy-plugin-qt
        chmod +x linuxdeploy-plugin-qt
        export LINUXDEPLOY_PLUGIN_QT="./linuxdeploy-plugin-qt"
    fi
    
    # Create AppDir structure
    APPDIR="$BUILD_DIR/PegSolitaire.AppDir"
    mkdir -p "$APPDIR"
    
    # Copy installed files
    cp -r "$INSTALL_DIR/usr"/* "$APPDIR/"
    
    # Create desktop file if it doesn't exist
    if [ ! -f "$APPDIR/share/applications/pegsolitaire.desktop" ]; then
        mkdir -p "$APPDIR/share/applications"
        cat > "$APPDIR/share/applications/pegsolitaire.desktop" << EOF
[Desktop Entry]
Type=Application
Name=Peg Solitaire
Comment=Classic peg solitaire puzzle game
Exec=pegsolitaire
Icon=pegsolitaire
Categories=Game;LogicGame;
EOF
    fi
    
    # Create AppImage
    $LINUXDEPLOY --appdir "$APPDIR" --plugin qt --output appimage
    
    # Rename AppImage
    if [ -f "Peg_Solitaire-x86_64.AppImage" ]; then
        mv "Peg_Solitaire-x86_64.AppImage" "PegSolitaire-${VERSION}-x86_64.AppImage"
        echo "AppImage created: PegSolitaire-${VERSION}-x86_64.AppImage"
    fi
}

function create_deb_package() {
    echo "Creating DEB package..."
    
    DEB_DIR="$BUILD_DIR/deb"
    mkdir -p "$DEB_DIR/DEBIAN"
    mkdir -p "$DEB_DIR/usr"
    
    # Copy installed files
    cp -r "$INSTALL_DIR/usr"/* "$DEB_DIR/usr/"
    
    # Create control file
    cat > "$DEB_DIR/DEBIAN/control" << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Section: games
Priority: optional
Architecture: amd64
Depends: libc6, libqt6core6, libqt6gui6, libqt6widgets6
Maintainer: Peg Solitaire Team <team@pegsolitaire.com>
Description: Classic peg solitaire puzzle game
 A digital implementation of the classic peg solitaire board game.
 Features multiple difficulty levels and game modes.
Homepage: https://github.com/yourname/solitaire
EOF
    
    # Create postinst script
    cat > "$DEB_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash
set -e
if [ "$1" = "configure" ]; then
    # Update desktop database
    if command -v update-desktop-database >/dev/null 2>&1; then
        update-desktop-database -q /usr/share/applications
    fi
    # Update icon cache
    if command -v gtk-update-icon-cache >/dev/null 2>&1; then
        gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor
    fi
fi
EOF
    chmod +x "$DEB_DIR/DEBIAN/postinst"
    
    # Build DEB package
    dpkg-deb --build "$DEB_DIR" "PegSolitaire-${VERSION}-amd64.deb"
    echo "DEB package created: PegSolitaire-${VERSION}-amd64.deb"
}

function create_rpm_package() {
    echo "Creating RPM package..."
    
    if ! command -v rpmbuild &> /dev/null; then
        echo "Error: rpmbuild is required for RPM packaging"
        exit 1
    fi
    
    RPM_ROOT="$BUILD_DIR/rpm"
    mkdir -p "$RPM_ROOT"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
    
    # Create spec file
    cat > "$RPM_ROOT/SPECS/pegsolitaire.spec" << EOF
Name:           $PACKAGE_NAME
Version:        $VERSION
Release:        1%{?dist}
Summary:        Classic peg solitaire puzzle game
License:        MIT
URL:            https://github.com/yourname/solitaire
Source0:        pegsolitaire-$VERSION.tar.gz
BuildRequires:  cmake, qt6-qtbase-devel
Requires:       qt6-qtbase

%description
A digital implementation of the classic peg solitaire board game.
Features multiple difficulty levels and game modes.

%prep
%setup -q

%build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?_smp_mflags}

%install
cd build
make install DESTDIR=%{buildroot}

%files
%{_bindir}/pegsolitaire
%{_datadir}/applications/pegsolitaire.desktop
%{_datadir}/icons/hicolor/*/apps/pegsolitaire.*

%changelog
* $(date +'%a %b %d %Y') Builder <builder@localhost> - $VERSION-1
- Initial package
EOF
    
    # Create source tarball
    tar -czf "$RPM_ROOT/SOURCES/pegsolitaire-$VERSION.tar.gz" -C "$PROJECT_DIR" .
    
    # Build RPM
    rpmbuild --define "_topdir $RPM_ROOT" -ba "$RPM_ROOT/SPECS/pegsolitaire.spec"
    
    # Copy RPM to build directory
    find "$RPM_ROOT/RPMS" -name "*.rpm" -exec cp {} "$BUILD_DIR/" \;
    echo "RPM package created in $BUILD_DIR/"
}

function create_tar_package() {
    echo "Creating TAR package..."
    
    TAR_NAME="PegSolitaire-${VERSION}-linux-x86_64.tar.gz"
    
    # Create portable structure
    PORTABLE_DIR="$BUILD_DIR/PegSolitaire-$VERSION"
    mkdir -p "$PORTABLE_DIR"
    
    # Copy binary and resources
    cp "$INSTALL_DIR/usr/bin/pegsolitaire" "$PORTABLE_DIR/"
    
    # Copy any required libraries (simplified - in practice you'd use ldd)
    # mkdir -p "$PORTABLE_DIR/lib"
    # ldd "$PORTABLE_DIR/pegsolitaire" | grep -E "(Qt|libicu)" | awk '{print $3}' | xargs -I {} cp {} "$PORTABLE_DIR/lib/"
    
    # Create run script
    cat > "$PORTABLE_DIR/run.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
exec "$SCRIPT_DIR/pegsolitaire" "$@"
EOF
    chmod +x "$PORTABLE_DIR/run.sh"
    
    # Create README
    cat > "$PORTABLE_DIR/README.txt" << EOF
Peg Solitaire v$VERSION - Portable Linux Version

To run the game:
  ./run.sh

Or directly:
  ./pegsolitaire

Requirements:
- Qt 6 libraries (usually available through your distribution's package manager)
- X11 or Wayland display server

For system-wide installation, copy 'pegsolitaire' to /usr/local/bin/
EOF
    
    # Create tarball
    cd "$BUILD_DIR"
    tar -czf "$TAR_NAME" "PegSolitaire-$VERSION"
    echo "TAR package created: $TAR_NAME"
}

# Create different package formats
case "$FORMAT" in
    "appimage")
        create_appimage
        ;;
    "deb")
        create_deb_package
        ;;
    "rpm")
        create_rpm_package
        ;;
    "tar")
        create_tar_package
        ;;
    *)
        echo "Unknown format: $FORMAT"
        echo "Supported formats: appimage, deb, rpm, tar"
        exit 1
        ;;
esac

echo ""
echo "Build completed successfully!"
echo "Output files in: $BUILD_DIR"
echo ""
echo "To test the package:"
case "$FORMAT" in
    "appimage")
        echo "  chmod +x PegSolitaire-${VERSION}-x86_64.AppImage"
        echo "  ./PegSolitaire-${VERSION}-x86_64.AppImage"
        ;;
    "deb")
        echo "  sudo dpkg -i PegSolitaire-${VERSION}-amd64.deb"
        ;;
    "rpm")
        echo "  sudo rpm -i *.rpm"
        ;;
    "tar")
        echo "  tar -xzf PegSolitaire-${VERSION}-linux-x86_64.tar.gz"
        echo "  cd PegSolitaire-$VERSION && ./run.sh"
        ;;
esac

# Peg Solitaire Packaging

This directory contains all the necessary files and scripts for packaging Peg Solitaire across different platforms.

## Directory Structure

```
packaging/
├── build-windows.bat      # Windows build script
├── build-macos.sh         # macOS build script  
├── build-linux.sh         # Linux build script
├── windows-installer.nsi  # NSIS installer configuration
├── Info.plist.in          # macOS app bundle template
├── pegsolitaire.desktop   # Linux desktop entry
└── README.md             # This file
```

## Prerequisites

### Windows
- Visual Studio 2019 or later with C++ support
- Qt 6.2 or later
- CMake 3.20 or later
- NSIS (Nullsoft Scriptable Install System) for installer creation

### macOS
- Xcode Command Line Tools
- Qt 6.2 or later
- CMake 3.20 or later
- macdeployqt (comes with Qt)

### Linux
- GCC 9 or later / Clang 10 or later
- Qt 6.2 or later
- CMake 3.20 or later
- For specific package formats:
  - AppImage: linuxdeploy, linuxdeploy-plugin-qt
  - DEB: dpkg-dev
  - RPM: rpm-build, rpmbuild
  - TAR: tar (standard)

## Building

### Windows

Build a release version with installer:
```batch
cd packaging
build-windows.bat 1.0.0
```

This will:
1. Create a Release build in `build-release-windows/`
2. Deploy Qt dependencies with `windeployqt`
3. Create an NSIS installer: `PegSolitaire-1.0.0-Setup.exe`

### macOS

Build a release version with DMG:
```bash
cd packaging
chmod +x build-macos.sh
./build-macos.sh 1.0.0
```

This will:
1. Create a Release build in `build-release-macos/`
2. Create an app bundle: `PegSolitaire.app`
3. Deploy Qt frameworks with `macdeployqt`
4. Create a DMG installer: `PegSolitaire-1.0.0-macOS.dmg`

### Linux

Build different package formats:
```bash
cd packaging
chmod +x build-linux.sh

# Create AppImage (recommended)
./build-linux.sh 1.0.0 appimage

# Create DEB package
./build-linux.sh 1.0.0 deb

# Create RPM package
./build-linux.sh 1.0.0 rpm

# Create portable TAR archive
./build-linux.sh 1.0.0 tar
```

## Automated Builds

The project includes GitHub Actions workflow (`.github/workflows/release.yml`) that automatically builds packages for all platforms when a version tag is pushed:

```bash
git tag v1.0.0
git push origin v1.0.0
```

This will create:
- Windows: `PegSolitaire-1.0.0-Setup.exe`
- macOS: `PegSolitaire-1.0.0-macOS.dmg`
- Linux: `PegSolitaire-1.0.0-x86_64.AppImage`

## Package Contents

All packages include:
- Main executable (`pegsolitaire` or `PegSolitaire.exe`)
- Required Qt libraries and dependencies
- Application icon and resources
- Desktop integration files (where applicable)

### Windows Installer Features
- Start Menu shortcuts
- Desktop shortcut (optional)
- Uninstaller with proper cleanup
- File association for `.pegsave` files
- Registry entries for proper Windows integration

### macOS App Bundle Features
- Native macOS app bundle structure
- Proper Info.plist configuration
- Application icon in all required sizes
- File associations for save files
- Retina display support

### Linux Packages Features
- Desktop file for application menu integration
- Icon files for various sizes
- MIME type registration for save files
- Standard Linux filesystem hierarchy compliance

## Testing Packages

### Windows
1. Run the installer: `PegSolitaire-1.0.0-Setup.exe`
2. Check Start Menu entry
3. Verify desktop shortcut (if selected)
4. Test file associations with `.pegsave` files
5. Test uninstaller

### macOS
1. Mount the DMG: `open PegSolitaire-1.0.0-macOS.dmg`
2. Drag app to Applications folder
3. Launch from Applications or Spotlight
4. Test file associations

### Linux
```bash
# AppImage
chmod +x PegSolitaire-1.0.0-x86_64.AppImage
./PegSolitaire-1.0.0-x86_64.AppImage

# DEB package
sudo dpkg -i PegSolitaire-1.0.0-amd64.deb
pegsolitaire

# RPM package
sudo rpm -i PegSolitaire-1.0.0-1.x86_64.rpm
pegsolitaire

# TAR archive
tar -xzf PegSolitaire-1.0.0-linux-x86_64.tar.gz
cd PegSolitaire-1.0.0
./run.sh
```

## Troubleshooting

### Common Issues

1. **Missing Qt Libraries**: Ensure Qt is properly installed and deployment tools are in PATH
2. **Build Failures**: Check CMake version and compiler compatibility
3. **Packaging Tool Missing**: Install platform-specific packaging tools as listed in prerequisites

### Windows-specific
- Ensure NSIS is installed and in PATH
- Check Visual Studio C++ redistributables
- Verify Qt bin directory is in PATH for windeployqt

### macOS-specific
- Ensure Xcode Command Line Tools are installed
- Check macOS deployment target compatibility
- Verify app bundle structure with `ls -la PegSolitaire.app/Contents/`

### Linux-specific
- Install distribution-specific Qt development packages
- For AppImage: ensure FUSE is available for testing
- For DEB/RPM: check package dependencies

## Code Signing

For production releases, consider code signing:

### Windows
```batch
signtool sign /f certificate.p12 /p password /t http://timestamp.digicert.com PegSolitaire-1.0.0-Setup.exe
```

### macOS
```bash
codesign --deep --force --verify --verbose --sign "Developer ID Application: Your Name" PegSolitaire.app
```

### Linux
AppImages can be signed using gpg or other methods as needed.

## Distribution

### GitHub Releases
The automated workflow creates GitHub releases with all packages attached.

### Platform-specific Stores
- Windows: Microsoft Store (requires UWP conversion)
- macOS: Mac App Store (requires additional entitlements)
- Linux: Snap Store, Flathub, or distribution repositories

## Customization

### Adding New Platforms
1. Create new build script following existing patterns
2. Update GitHub Actions workflow
3. Add platform-specific packaging files
4. Update this documentation

### Modifying Package Metadata
- Windows: Edit `windows-installer.nsi`
- macOS: Edit `Info.plist.in`
- Linux: Edit `pegsolitaire.desktop` and build scripts

### Adding Dependencies
Update build scripts to include additional libraries and their deployment.

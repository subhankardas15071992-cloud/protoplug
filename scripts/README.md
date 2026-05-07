# Protoplug build scripts

These scripts build Protoplug 1.5.0 in Release mode and create a zip package in `dist/`.

- macOS: `scripts/build-macos.sh`
- Linux: `scripts/build-linux.sh`
- Windows: `scripts/build-windows.ps1`

The packages include the built plugins, `ProtoplugFiles`, `readme.md`, and `license.txt`.

Set `PROTOPLUG_JUCE_DIR` or `PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR` to use local checkouts instead of CMake fetching them.

## macOS runtime libraries

`scripts/build-macos-runtime-libs.sh` rebuilds the bundled macOS universal runtime libraries in `ProtoplugFiles/lib`:

- `libluajit-5.1.dylib`
- `libfftw3.3.dylib`

The script verifies the `x86_64 arm64` slices and applies ad-hoc signing.

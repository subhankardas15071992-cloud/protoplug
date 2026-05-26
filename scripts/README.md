# Protoplug build scripts

These scripts build Protoplug 1.5.0 in Release mode and create a zip package in `dist/`.

- macOS: `scripts/build-macos.sh`
- Linux: `scripts/build-linux.sh`
- Windows: `scripts/build-windows.ps1`

Formats:

- macOS: AU, VST3, CLAP
- Linux: VST3, CLAP, LV2
- Windows: VST3, CLAP

LV2 is built for Linux only. Protoplug's FX and Gen plugins share one
`ProtoplugFiles` folder. On Linux, Protoplug already reads that folder from
`/usr/share/ProtoplugFiles`, so all Linux plugin formats can use the same
installed resources. On macOS and Windows, Protoplug's lookup is based around
the plugin location, and LV2 bundles are expected to be self-contained. Keeping
LV2 off macOS and Windows avoids either duplicating `ProtoplugFiles` into
separate FX and Gen bundles or adding a separate platform-specific shared
resource convention only for LV2.

The packages include the built plugins, `ProtoplugFiles`, `readme.md`, and `license.txt`.

The Linux package also includes `install-linux.sh`. After unpacking the zip, run it from
the package folder to install the plugins into the current user's standard plugin folders
and `ProtoplugFiles` into `/usr/share/ProtoplugFiles`.

Set `PROTOPLUG_JUCE_DIR` or `PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR` to use local checkouts instead of CMake fetching them.

## macOS runtime libraries

`scripts/build-macos-runtime-libs.sh` rebuilds the bundled macOS universal runtime libraries in `ProtoplugFiles/lib`:

- `libluajit-5.1.dylib`
- `libfftw3.3.dylib`

The script verifies the `x86_64 arm64` slices and applies ad-hoc signing.

protoplug
=========
Create audio plugins on-the-fly with LuaJIT.

Protoplug is a plugin that lets you load and edit Lua scripts as audio effects and instruments. Scripts can process audio and MIDI, draw their own interface, and use external libraries. It turns a DAW into a live coding environment for audio.

This fork modernizes Protoplug as version 1.5.0.

Supported plugin formats
------------------------
- macOS: universal AUv2, VST3, CLAP, and LV2 for `arm64` and `x86_64`
- Windows: 64-bit VST3, CLAP, and LV2
- Linux: 64-bit VST3, CLAP, and LV2

Building from source
--------------------
The modern build uses CMake and JUCE 8. CMake fetches JUCE and clap-juce-extensions automatically unless you set `PROTOPLUG_JUCE_DIR` or `PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR`.

macOS:
```sh
scripts/build-macos.sh
```

Linux:
```sh
sudo apt-get install -y cmake ninja-build pkg-config build-essential libasound2-dev libcurl4-openssl-dev libfreetype-dev libgl1-mesa-dev libgtk-3-dev libjack-jackd2-dev libwebkit2gtk-4.1-dev libx11-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev
scripts/build-linux.sh
```

Windows, from a Developer PowerShell:
```powershell
scripts/build-windows.ps1
```

Each script builds Release plugins and creates a zip in `dist/` containing the plugin artifacts, `ProtoplugFiles`, this README, and the license.

Manual CMake build
------------------
```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target protoplug_fx_All protoplug_gen_All protoplug_fx_CLAP protoplug_gen_CLAP
```

On macOS, the default CMake configuration builds universal `arm64;x86_64` binaries with deployment target `10.13`.

License
-------
Protoplug is MIT-licensed. See `license.txt`.

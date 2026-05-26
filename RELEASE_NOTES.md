# Release Notes

## Protoplug 1.5.1

Protoplug 1.5.1 focuses on packaging clarity and larger channel layouts.

- Added bounded multi-channel support up to 64 audio channels where the host and plugin format allow it.
- Added Lua helpers for multi-channel scripts: `plugin.maxChannels`, `plugin.numInputChannels`, `plugin.numOutputChannels`, `plugin.numChannels`, and the `multiIO` helper.
- Fixed the `midi.Event(time, dataSize, data)` constructor so scripts can create MIDI events directly from byte tables.
- Kept existing stereo scripts compatible while allowing `stereoFx` and `polyGen` scripts to run across the active channel layout.
- Limited LV2 releases to Linux. macOS packages now provide AUv2, VST3, and CLAP; Windows packages provide VST3 and CLAP.
- Kept Linux resources shared through `/usr/share/ProtoplugFiles`, so Linux VST3, CLAP, and LV2 can use the same installed `ProtoplugFiles` folder.
- Updated plugin metadata and user-visible welcome text to version 1.5.1.

# Changelog

## 1.5.1 - 2026-05-26

### Added

- Added support for audio layouts up to 64 channels, subject to host and plugin-format support.
- Added Lua-facing channel count values: `plugin.maxChannels`, `plugin.numInputChannels`, `plugin.numOutputChannels`, and `plugin.numChannels`.
- Added the `multiIO` helper for per-channel Lua scripts.

### Changed

- Dropped LV2 support on macOS and Windows. LV2 remains supported on Linux, where Protoplug can use the shared `/usr/share/ProtoplugFiles` resource folder.
- Updated `stereoFx` and `polyGen` helpers so existing stereo scripts remain compatible while multi-channel layouts can be processed.
- Updated plugin metadata, release tooling version, default script welcome text, and documentation to 1.5.1.

### Fixed

- Fixed `midi.Event(time, dataSize, data)` so the documented data-table constructor returns a MIDI event instead of no value.
- Fixed package scripts and GitHub Actions artifact upload paths so they use the current release version instead of the old 1.5.0 package names.

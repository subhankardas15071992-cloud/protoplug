# Runtime libraries

This folder contains platform-specific runtime libraries loaded by Protoplug and by Lua scripts that use FFI.

macOS:
- `libluajit-5.1.dylib`: universal `x86_64 arm64`, built from OpenResty LuaJIT2 `v2.1-20260415`
- `libfftw3.3.dylib`: universal `x86_64 arm64`, built from FFTW `3.3.10`

Linux:
- `libluajit-5.1.so`: x86_64 ELF
- `libfftw3.so.3`: x86_64 ELF

Windows:
- `lua51.dll`: x86_64 PE DLL
- `libfftw3-3.dll`: x86_64 PE DLL

Rebuild the macOS universal libraries with:

```sh
scripts/build-macos-runtime-libs.sh
```

LuaJIT is MIT-licensed. FFTW is distributed under the GNU GPL unless a separate FFTW commercial license is used.

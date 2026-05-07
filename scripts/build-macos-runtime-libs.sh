#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-"$ROOT_DIR/build/runtime-libs"}"
LIB_DIR="$ROOT_DIR/ProtoplugFiles/lib"
JOBS="${JOBS:-"$(sysctl -n hw.ncpu)"}"
MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-10.13}"

FFTW_VERSION="${FFTW_VERSION:-3.3.10}"
FFTW_URL="${FFTW_URL:-"https://www.fftw.org/fftw-${FFTW_VERSION}.tar.gz"}"
LUAJIT_REPO="${LUAJIT_REPO:-"https://github.com/openresty/luajit2.git"}"
LUAJIT_TAG="${LUAJIT_TAG:-"v2.1-20260415"}"

SRC_DIR="$BUILD_DIR/src"
FFTW_SRC="$SRC_DIR/fftw-$FFTW_VERSION"
LUAJIT_SRC="$SRC_DIR/luajit2"
OUT_DIR="$BUILD_DIR/universal"

mkdir -p "$SRC_DIR" "$OUT_DIR" "$LIB_DIR"

if [[ ! -d "$FFTW_SRC" ]]; then
    curl -L --fail -o "$SRC_DIR/fftw-$FFTW_VERSION.tar.gz" "$FFTW_URL"
    tar -xzf "$SRC_DIR/fftw-$FFTW_VERSION.tar.gz" -C "$SRC_DIR"
fi

if [[ ! -d "$LUAJIT_SRC/.git" ]]; then
    rm -rf "$LUAJIT_SRC"
    git clone --depth 1 --branch "$LUAJIT_TAG" "$LUAJIT_REPO" "$LUAJIT_SRC"
fi

rm -rf "$BUILD_DIR/fftw-universal" "$BUILD_DIR/install/fftw-universal"
mkdir -p "$BUILD_DIR/fftw-universal"

(
    cd "$BUILD_DIR/fftw-universal"
    CFLAGS="-arch x86_64 -arch arm64 -O3 -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
    LDFLAGS="-arch x86_64 -arch arm64 -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
        "$FFTW_SRC/configure" \
            --prefix="$BUILD_DIR/install/fftw-universal" \
            --enable-shared \
            --disable-static \
            --disable-fortran
    make -j"$JOBS"
    make install
)

rm -rf "$BUILD_DIR/luajit-x86_64" "$BUILD_DIR/luajit-arm64"
cp -R "$LUAJIT_SRC" "$BUILD_DIR/luajit-x86_64"
cp -R "$LUAJIT_SRC" "$BUILD_DIR/luajit-arm64"

MACOSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    make -C "$BUILD_DIR/luajit-x86_64/src" clean
MACOSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    make -C "$BUILD_DIR/luajit-x86_64/src" -j"$JOBS" \
        TARGET_SYS=Darwin \
        BUILDMODE=dynamic \
        CC="clang -arch x86_64" \
        HOST_CC="clang -arch x86_64" \
        TARGET_FLAGS="-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
        PREFIX="$BUILD_DIR/install/luajit-x86_64"

MACOSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    make -C "$BUILD_DIR/luajit-arm64/src" clean
MACOSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    make -C "$BUILD_DIR/luajit-arm64/src" -j"$JOBS" \
        TARGET_SYS=Darwin \
        BUILDMODE=dynamic \
        CC="clang" \
        HOST_CC="clang -arch x86_64" \
        STATIC_CC="clang -arch arm64" \
        DYNAMIC_CC="clang -arch arm64 -fPIC" \
        TARGET_LD="clang -arch arm64" \
        TARGET_FLAGS="-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
        PREFIX="$BUILD_DIR/install/luajit-arm64"

cp "$BUILD_DIR/install/fftw-universal/lib/libfftw3.3.dylib" "$OUT_DIR/libfftw3.3.dylib"
lipo -create \
    "$BUILD_DIR/luajit-x86_64/src/libluajit.so" \
    "$BUILD_DIR/luajit-arm64/src/libluajit.so" \
    -output "$OUT_DIR/libluajit-5.1.dylib"

install_name_tool -id "@rpath/libfftw3.3.dylib" "$OUT_DIR/libfftw3.3.dylib"
install_name_tool -id "@rpath/libluajit-5.1.dylib" "$OUT_DIR/libluajit-5.1.dylib"

codesign --force --sign - "$OUT_DIR/libfftw3.3.dylib" "$OUT_DIR/libluajit-5.1.dylib"

lipo -info "$OUT_DIR/libfftw3.3.dylib" | grep -q "x86_64 arm64"
lipo -info "$OUT_DIR/libluajit-5.1.dylib" | grep -q "x86_64 arm64"
codesign --verify --verbose=2 "$OUT_DIR/libfftw3.3.dylib" "$OUT_DIR/libluajit-5.1.dylib"

cp "$OUT_DIR/libfftw3.3.dylib" "$OUT_DIR/libluajit-5.1.dylib" "$LIB_DIR/"

echo "Updated $LIB_DIR/libfftw3.3.dylib"
echo "Updated $LIB_DIR/libluajit-5.1.dylib"

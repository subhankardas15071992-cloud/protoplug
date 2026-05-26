#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="$(tr -d '[:space:]' < "$ROOT_DIR/DevScripts/resources/version.txt")"
if [[ -z "$VERSION" ]]; then
    echo "Unable to read package version from DevScripts/resources/version.txt" >&2
    exit 1
fi
CONFIG="${CONFIG:-Release}"
BUILD_DIR="${BUILD_DIR:-"$ROOT_DIR/build/linux-x64"}"
DIST_DIR="${DIST_DIR:-"$ROOT_DIR/dist"}"
PACKAGE_NAME="protoplug-${VERSION}-linux-x64"
PACKAGE_DIR="$DIST_DIR/$PACKAGE_NAME"
ZIP_PATH="$DIST_DIR/$PACKAGE_NAME.zip"

cmake_args=(
    -S "$ROOT_DIR"
    -B "$BUILD_DIR"
    -G Ninja
    -DCMAKE_BUILD_TYPE="$CONFIG"
    -DPROTOPLUG_COPY_PLUGIN_AFTER_BUILD=OFF
)

if [[ -n "${PROTOPLUG_JUCE_DIR:-}" ]]; then
    cmake_args+=("-DPROTOPLUG_JUCE_DIR=$PROTOPLUG_JUCE_DIR")
fi

if [[ -n "${PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR:-}" ]]; then
    cmake_args+=("-DPROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR=$PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR")
fi

cmake "${cmake_args[@]}"

build_args=(
    --build "$BUILD_DIR"
    --config "$CONFIG"
    --target protoplug_fx_All protoplug_gen_All protoplug_fx_CLAP protoplug_gen_CLAP
)

if [[ -n "${JOBS:-}" ]]; then
    build_args+=(--parallel "$JOBS")
else
    build_args+=(--parallel)
fi

cmake "${build_args[@]}"

rm -rf "$PACKAGE_DIR" "$ZIP_PATH"
mkdir -p "$PACKAGE_DIR/Plugins/VST3" "$PACKAGE_DIR/Plugins/CLAP" "$PACKAGE_DIR/Plugins/LV2"

copy_artifact() {
    local src="$1"
    local dest="$2"

    if [[ ! -e "$src" ]]; then
        echo "Missing expected artifact: $src" >&2
        exit 1
    fi

    cp -R "$src" "$dest/"
}

copy_artifact "$BUILD_DIR/protoplug_fx_artefacts/$CONFIG/VST3/Lua Protoplug Fx.vst3" "$PACKAGE_DIR/Plugins/VST3"
copy_artifact "$BUILD_DIR/protoplug_fx_artefacts/$CONFIG/CLAP/Lua Protoplug Fx.clap" "$PACKAGE_DIR/Plugins/CLAP"
copy_artifact "$BUILD_DIR/protoplug_fx_artefacts/$CONFIG/LV2/Lua Protoplug Fx.lv2" "$PACKAGE_DIR/Plugins/LV2"
copy_artifact "$BUILD_DIR/protoplug_gen_artefacts/$CONFIG/VST3/Lua Protoplug Gen.vst3" "$PACKAGE_DIR/Plugins/VST3"
copy_artifact "$BUILD_DIR/protoplug_gen_artefacts/$CONFIG/CLAP/Lua Protoplug Gen.clap" "$PACKAGE_DIR/Plugins/CLAP"
copy_artifact "$BUILD_DIR/protoplug_gen_artefacts/$CONFIG/LV2/Lua Protoplug Gen.lv2" "$PACKAGE_DIR/Plugins/LV2"

cp -R "$ROOT_DIR/ProtoplugFiles" "$PACKAGE_DIR/"
cp "$ROOT_DIR/readme.md" "$ROOT_DIR/license.txt" "$PACKAGE_DIR/"
cp "$ROOT_DIR/scripts/install-linux.sh" "$PACKAGE_DIR/install-linux.sh"
chmod +x "$PACKAGE_DIR/install-linux.sh"

(
    cd "$DIST_DIR"
    cmake -E tar cf "$ZIP_PATH" --format=zip "$PACKAGE_NAME"
)

echo "Created $ZIP_PATH"

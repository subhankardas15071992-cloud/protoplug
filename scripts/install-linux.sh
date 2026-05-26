#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  ./install-linux.sh [--package-dir DIR]

Installs Protoplug from an unpacked Linux package.

Default install locations:
  VST3 -> $HOME/.vst3
  CLAP -> $HOME/.clap
  LV2  -> $HOME/.lv2
  ProtoplugFiles -> /usr/share/ProtoplugFiles

The script uses sudo only for /usr/share/ProtoplugFiles when needed.

Optional plugin directory overrides:
  VST3_DIR=/custom/vst3 CLAP_DIR=/custom/clap LV2_DIR=/custom/lv2 ./install-linux.sh
EOF
}

fail() {
    echo "Error: $*" >&2
    exit 1
}

run_as_root() {
    if [[ "${EUID:-$(id -u)}" -eq 0 ]]; then
        "$@"
    elif command -v sudo >/dev/null 2>&1; then
        sudo "$@"
    else
        fail "installing /usr/share/ProtoplugFiles needs root permission, but sudo was not found"
    fi
}

copy_tree() {
    local src="$1"
    local dest="$2"

    rm -rf "$dest"
    mkdir -p "$(dirname "$dest")"
    cp -a "$src" "$dest"
}

copy_tree_as_root() {
    local src="$1"
    local dest="$2"

    run_as_root rm -rf "$dest"
    run_as_root mkdir -p "$(dirname "$dest")"
    run_as_root cp -a "$src" "$dest"
}

resolve_package_dir() {
    local requested="$1"
    local script_dir="$2"

    if [[ -n "$requested" ]]; then
        (cd "$requested" && pwd)
        return
    fi

    local candidate
    for candidate in "$script_dir" "$script_dir/.." "$PWD"; do
        if [[ -d "$candidate/Plugins" && -d "$candidate/ProtoplugFiles" ]]; then
            (cd "$candidate" && pwd)
            return
        fi
    done

    fail "could not find package root. Run this from the unpacked package, or pass --package-dir DIR."
}

install_plugins() {
    local src_dir="$1"
    local dest_dir="$2"
    local pattern="$3"
    local label="$4"

    [[ -d "$src_dir" ]] || return

    mkdir -p "$dest_dir"

    shopt -s nullglob
    local entries=("$src_dir"/$pattern)
    shopt -u nullglob

    if (( ${#entries[@]} == 0 )); then
        echo "No $label plugins found in $src_dir"
        return
    fi

    local entry target
    for entry in "${entries[@]}"; do
        target="$dest_dir/$(basename "$entry")"
        copy_tree "$entry" "$target"
        echo "Installed $label: $target"
    done
}

package_dir_arg=""

while (( $# > 0 )); do
    case "$1" in
        --package-dir)
            [[ $# -ge 2 ]] || fail "--package-dir needs a directory"
            package_dir_arg="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            fail "unknown argument: $1"
            ;;
    esac
done

if [[ "${EUID:-$(id -u)}" -eq 0 && -n "${SUDO_USER:-}" && "${SUDO_USER}" != "root" ]]; then
    fail "run this script without sudo. It will ask for sudo only when copying /usr/share/ProtoplugFiles."
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_DIR="$(resolve_package_dir "$package_dir_arg" "$SCRIPT_DIR")"
RESOURCE_SRC="$PACKAGE_DIR/ProtoplugFiles"
RESOURCE_DEST="/usr/share/ProtoplugFiles"

[[ -d "$RESOURCE_SRC" ]] || fail "missing ProtoplugFiles in $PACKAGE_DIR"

VST3_DEST="${VST3_DIR:-"$HOME/.vst3"}"
CLAP_DEST="${CLAP_DIR:-"$HOME/.clap"}"
LV2_DEST="${LV2_DIR:-"$HOME/.lv2"}"

echo "Installing Protoplug from: $PACKAGE_DIR"
echo

copy_tree_as_root "$RESOURCE_SRC" "$RESOURCE_DEST"
echo "Installed resources: $RESOURCE_DEST"

install_plugins "$PACKAGE_DIR/Plugins/VST3" "$VST3_DEST" "*.vst3" "VST3"
install_plugins "$PACKAGE_DIR/Plugins/CLAP" "$CLAP_DEST" "*.clap" "CLAP"
install_plugins "$PACKAGE_DIR/Plugins/LV2" "$LV2_DEST" "*.lv2" "LV2"

echo
echo "Done. Restart or rescan your DAW's plugin list."

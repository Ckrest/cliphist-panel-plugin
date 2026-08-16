#!/usr/bin/env bash
# Produce a staged XFCE panel plugin artifact without mutating the live system.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PREFIX="${NATIVE_INSTALL_PREFIX:-/usr}"
NATIVE_ROOT="${SYSTEMS_NATIVE_ROOT:-${XDG_CACHE_HOME:-$HOME/.cache}/systems/native/cliphist-panel-plugin}"
SOURCE_DIR="$NATIVE_ROOT/source"
STAGE_ROOT="$NATIVE_ROOT/stage"
require_safe_native_root() {
    if [[ -z "$NATIVE_ROOT" || "$NATIVE_ROOT" == / || "$NATIVE_ROOT" == "$HOME" ]]; then
        echo "unsafe native build root: $NATIVE_ROOT" >&2; exit 2
    fi
}
prepare() {
    require_safe_native_root
    rm -rf -- "$SOURCE_DIR" "$STAGE_ROOT"
    mkdir -p "$SOURCE_DIR" "$STAGE_ROOT"
    rsync -a --exclude .git --exclude '*.o' --exclude '*.so' "$ROOT/" "$SOURCE_DIR/"
}
build() { make -C "$SOURCE_DIR" -j"$(nproc)"; }
stage() { make -C "$SOURCE_DIR" PREFIX="$PREFIX" DESTDIR="$STAGE_ROOT" install; }
realize() { prepare; build; stage; }
clean() { require_safe_native_root; rm -rf -- "$NATIVE_ROOT"; }
case "${1:-realize}" in
    prepare) prepare ;; build) build ;; stage) stage ;; realize) realize ;; clean) clean ;;
    *) echo "Usage: $0 [prepare|build|stage|realize|clean]" >&2; exit 2 ;;
esac

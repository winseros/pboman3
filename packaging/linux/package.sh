#!/bin/sh
set -eu

usage() {
    printf '%s\n' "Usage: $0 BUILD_DIRECTORY OUTPUT_DIRECTORY"
    printf '%s\n' "Set LINUXDEPLOY to a pinned linuxdeploy executable."
}

[ "$#" -eq 2 ] || { usage >&2; exit 2; }
build_dir=$1
output_dir=$2
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
linuxdeploy=${LINUXDEPLOY:-}

[ -n "$linuxdeploy" ] || { usage >&2; exit 2; }
[ -x "$linuxdeploy" ] || { printf '%s\n' "LINUXDEPLOY is not executable: $linuxdeploy" >&2; exit 2; }

mkdir -p "$output_dir"
output_dir=$(CDPATH= cd -- "$output_dir" && pwd)
appdir=$output_dir/PBOManager.AppDir

"$script_dir/stage-appdir.sh" "$build_dir" "$appdir"

# linuxdeploy discovers a pinned linuxdeploy-plugin-qt beside its executable.
# The Qt plugin deploys the xcb and Wayland platform plugins available in the
# selected Qt installation, along with required image and TLS plugins.
export EXTRA_PLATFORM_PLUGINS=${EXTRA_PLATFORM_PLUGINS:-libqwayland-egl.so;libqwayland-generic.so}
"$linuxdeploy" \
    --appdir "$appdir" \
    --desktop-file "$appdir/usr/share/applications/io.github.winseros.pboman3.desktop" \
    --icon-file "$appdir/usr/share/icons/hicolor/512x512/apps/io.github.winseros.pboman3.png" \
    --plugin qt

tar -C "$output_dir" -czf "$output_dir/PBOManager-x86_64.AppDir.tar.gz" PBOManager.AppDir

ARCH=x86_64 OUTPUT="$output_dir/PBOManager-x86_64.AppImage" \
    "$linuxdeploy" --appdir "$appdir" --output appimage
sha256sum \
    "$output_dir/PBOManager-x86_64.AppDir.tar.gz" \
    "$output_dir/PBOManager-x86_64.AppImage" \
    > "$output_dir/SHA256SUMS"

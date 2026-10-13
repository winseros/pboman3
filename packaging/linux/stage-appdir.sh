#!/bin/sh
set -eu

usage() {
    printf '%s\n' "Usage: $0 BUILD_DIRECTORY APPDIR"
}

[ "$#" -eq 2 ] || { usage >&2; exit 2; }
build_dir=$1
appdir=$2
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

case "$appdir" in
    /*) ;;
    *) appdir=$PWD/$appdir ;;
esac

mkdir -p "$appdir"
DESTDIR=$appdir cmake --install "$build_dir" --prefix /usr
cp "$script_dir/AppRun" "$appdir/AppRun"
chmod +x "$appdir/AppRun"

for required in \
    usr/bin/pbom \
    usr/bin/pboc \
    usr/share/applications/io.github.winseros.pboman3.desktop \
    usr/share/mime/packages/io.github.winseros.pboman3.xml \
    usr/share/icons/hicolor/512x512/apps/io.github.winseros.pboman3.png
do
    [ -e "$appdir/$required" ] || {
        printf '%s\n' "Missing AppDir file: $required" >&2
        exit 1
    }
done

ln -sfn usr/share/applications/io.github.winseros.pboman3.desktop \
    "$appdir/io.github.winseros.pboman3.desktop"
ln -sfn usr/share/icons/hicolor/512x512/apps/io.github.winseros.pboman3.png \
    "$appdir/io.github.winseros.pboman3.png"

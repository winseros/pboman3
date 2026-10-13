#!/bin/sh
set -eu

data_home=${XDG_DATA_HOME:-"$HOME/.local/share"}
config_home=${XDG_CONFIG_HOME:-"$HOME/.config"}

rm -f \
    "$data_home/applications/io.github.winseros.pboman3.desktop" \
    "$data_home/mime/packages/io.github.winseros.pboman3.xml" \
    "$data_home/kio/servicemenus/pboman3-pbo.desktop" \
    "$data_home/kio/servicemenus/pboman3-folder.desktop" \
    "$data_home/nautilus-python/extensions/pboman3.py" \
    "$data_home/icons/hicolor/512x512/apps/io.github.winseros.pboman3.png" \
    "$config_home/pboman3/integration.ini"
rmdir "$config_home/pboman3" 2>/dev/null || true

if [ -d "$data_home/mime" ] && command -v update-mime-database >/dev/null 2>&1; then
    update-mime-database "$data_home/mime"
fi
if [ -d "$data_home/applications" ] && command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$data_home/applications"
fi
if [ -d "$data_home/icons/hicolor" ] && command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t "$data_home/icons/hicolor" >/dev/null 2>&1 || true
fi

printf '%s\n' "PBO Manager integration removed for this user."

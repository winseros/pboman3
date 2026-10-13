#!/bin/sh
set -eu

usage() {
    printf '%s\n' "Usage: $0 --launcher /absolute/path/to/pbom"
}

launcher=
while [ "$#" -gt 0 ]; do
    case "$1" in
        --launcher)
            [ "$#" -ge 2 ] || { usage >&2; exit 2; }
            launcher=$2
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            usage >&2
            exit 2
            ;;
    esac
done

[ -n "$launcher" ] || { usage >&2; exit 2; }
case "$launcher" in
    /*) ;;
    *) printf '%s\n' "The launcher must be an absolute path." >&2; exit 2 ;;
esac
case "$launcher" in
    *"
"*|*""*) printf '%s\n' "The launcher path must be a single line." >&2; exit 2 ;;
esac
[ -x "$launcher" ] || { printf '%s\n' "The launcher is not executable: $launcher" >&2; exit 2; }

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
data_home=${XDG_DATA_HOME:-"$HOME/.local/share"}
config_home=${XDG_CONFIG_HOME:-"$HOME/.config"}
application_dir=$data_home/applications
mime_dir=$data_home/mime
service_dir=$data_home/kio/servicemenus
nautilus_dir=$data_home/nautilus-python/extensions
icon_dir=$data_home/icons/hicolor/512x512/apps
config_dir=$config_home/pboman3

mkdir -p "$application_dir" "$mime_dir/packages" "$service_dir" "$nautilus_dir" "$icon_dir" "$config_dir"

# Desktop Exec token quoting follows the Desktop Entry specification. A literal
# percent must be doubled because percent introduces field codes.
desktop_launcher=$(printf '%s' "$launcher" | sed 's/\\/\\\\/g; s/"/\\"/g; s/`/\\`/g; s/\$/\\$/g; s/%/%%/g')
sed_launcher=$(printf '%s' "$desktop_launcher" | sed 's/[\\&|]/\\&/g')
sed "s|@PBOM_EXECUTABLE@|\"$sed_launcher\"|g" \
    "$script_dir/share/applications/io.github.winseros.pboman3.desktop.in" \
    > "$application_dir/io.github.winseros.pboman3.desktop"
sed "s|@PBOM_EXECUTABLE@|\"$sed_launcher\"|g" \
    "$script_dir/share/kio/servicemenus/pboman3-pbo.desktop.in" \
    > "$service_dir/pboman3-pbo.desktop"
sed "s|@PBOM_EXECUTABLE@|\"$sed_launcher\"|g" \
    "$script_dir/share/kio/servicemenus/pboman3-folder.desktop.in" \
    > "$service_dir/pboman3-folder.desktop"
chmod +x "$service_dir/pboman3-pbo.desktop" "$service_dir/pboman3-folder.desktop"

cp "$script_dir/share/mime/packages/io.github.winseros.pboman3.xml" "$mime_dir/packages/"
cp "$script_dir/share/nautilus-python/extensions/pboman3.py" "$nautilus_dir/pboman3.py"
if [ -f "$script_dir/app512.png" ]; then
    icon_source=$script_dir/app512.png
else
    icon_source=$script_dir/../pbom/res/app512.png
fi
cp "$icon_source" "$icon_dir/io.github.winseros.pboman3.png"

{
    printf '%s\n' '[integration]'
    printf 'executable=%s\n' "$launcher"
} > "$config_dir/integration.ini"
chmod 600 "$config_dir/integration.ini"

command -v update-mime-database >/dev/null 2>&1 && update-mime-database "$mime_dir"
command -v update-desktop-database >/dev/null 2>&1 && update-desktop-database "$application_dir"
command -v gtk-update-icon-cache >/dev/null 2>&1 && gtk-update-icon-cache -f -t "$data_home/icons/hicolor" >/dev/null 2>&1 || true

printf '%s\n' "PBO Manager integration installed for this user."
printf '%s\n' "Restart Nautilus to load or update its context-menu extension."

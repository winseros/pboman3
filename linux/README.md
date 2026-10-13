# Linux desktop integration

PBO Manager ships standard MIME and desktop metadata, Dolphin service menus,
and a Nautilus extension. The extension only starts `pbom`.

For a portable installation, register a stable absolute path. Do not use an
AppImage temporary mount path:

```sh
./linux/install-user.sh --launcher "$PWD/AppDir/usr/bin/pbom"
```

Run the installer again after you move the application. Remove the per-user
integration with:

```sh
./linux/uninstall-user.sh
```

Nautilus requires the distribution package commonly named `nautilus-python`.
Restart Nautilus after an integration change. Dolphin reads the standard
`kio/servicemenus` directory.

The menus accept local selections of folders or `.pbo` files. Folder selections
get pack actions. PBO selections get unpack actions. Mixed and remote selections
do not get PBO Manager actions.

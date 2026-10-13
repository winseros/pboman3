# Arch Linux package

This directory contains three Arch package recipes:

- `pboman3` builds a tagged release from source.
- `pboman3-bin` installs the upstream prebuilt AppImage.
- `pboman3-git` builds the latest development revision from Git.

The AUR and official Arch repositories do not contain these packages yet.
Build them locally with `makepkg`. Before an AUR upload, replace each `SKIP`
checksum after the `v1.11.0` release files become available.

All variants install the GUI, CLI, desktop entry, PBO MIME definition,
application icon, Dolphin service menus, and Nautilus extension.

Build and install the source package as a regular user:

```sh
sudo pacman -S --needed base-devel
cd packaging/arch/pboman3
makepkg -si
```

Use the `pboman3-bin` or `pboman3-git` directory for another variant. Pacman
allows only one variant at a time.

Install `base-devel` before you use `makepkg`. `makepkg -s` installs the other
dependencies. Install the file manager integration that you use:

```sh
# KDE Dolphin integration
sudo pacman -S --needed dolphin

# GNOME Files integration
sudo pacman -S --needed nautilus-python
```

`nautilus-python` installs Nautilus and its Python bindings. Restart the file
manager after you install or update PBO Manager.

To remove the Arch package later, use the exact variant that you installed:

```sh
sudo pacman -Rns pboman3
# or: sudo pacman -Rns pboman3-bin
# or: sudo pacman -Rns pboman3-git
```

## AUR maintenance

Use one AUR Git repository for each package. Copy its `PKGBUILD` and `.SRCINFO`
files to that repository. Do not combine the recipes.

For each release, update `pkgver` and the checksums. Reset `pkgrel` to `1`.
Then regenerate `.SRCINFO` and test the package:

```sh
updpkgsums
makepkg --printsrcinfo > .SRCINFO
makepkg --cleanbuild
namcap PKGBUILD pboman3-*.pkg.tar.zst
```

Commit `PKGBUILD` and `.SRCINFO`. Regenerate `.SRCINFO` after each metadata
change.

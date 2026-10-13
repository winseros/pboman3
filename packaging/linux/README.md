# Linux packaging

`package.sh` stages one AppDir. It creates an AppDir tarball, an AppImage, and
SHA-256 checksums.

Build on x86-64 Ubuntu 22.04 with glibc 2.35. Do not use `-march=native`. Set
`LINUXDEPLOY` to the pinned linuxdeploy binary. Put the matching Qt plugin next
to it. Then run:

```sh
packaging/linux/package.sh build dist
```

Qt must provide the `xcb`, Wayland, image, and TLS plugins. Do not bundle host
graphics drivers or the glibc loader.

Run the AppImage to start the GUI. Use `--cli` for CLI commands:

```sh
./PBOManager-x86_64.AppImage --cli unpack --beside-input -- example.pbo
```

Use `--install-integration` or `--uninstall-integration` to manage desktop
integration for the current user. Run the install command again after moving
the AppImage.

The tarball contains `usr/bin/pbom` and `usr/bin/pboc`. Use it when the system
cannot mount AppImages.

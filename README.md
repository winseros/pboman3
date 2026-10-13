# PBO Manager

A tool to open, pack and unpack ArmA PBO files.

## Key features
 - Can pack/unpack PBO files.
 - Can preview files inside a PBO.
 - Can open mangled (by Mikero's tools) PBO.
 - Integrates with Windows Explorer / immersive menu, GNOME Files and KDE Dolphin.
 - Supports PBO metadata provisioning through [pbo.json](doc/pbo_json.md) or [\$PBOPREFIX\$](doc/prefix_files.md) files.

## Screenshots

![Main application window](doc/img/screenshot01.png 'Main application window')

### Context menu integration

![Unpack PBO context menu](doc/img/screenshot02.png 'Unpack PBO context menu')
![Pack folder as PBO context menu](doc/img/screenshot03.png 'Pack folder as PBO context menu')

### Windows 11 immersive menu integration

![Windows 11 immersive menu](doc/img/screenshot04.png 'Windows 11 immersive menu integration')

## Installation 

:small_blue_diamond: The application requires [Microsoft Visual C++ Redistributable 2015-2019](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run. Othervise, the application will complain regarding the `vcruntime140.dll`.

### Windows 10

#### Installer
1. Download the `installer` from the [Releases](https://github.com/winseros/pboman3/releases) section.
2. Run the `installer`.

#### Manual instllation
1. Download the `binaries` from the [Releases](https://github.com/winseros/pboman3/releases) section.
2. Run the `pbom.exe`
3. Optionally, to get the Windows Explorer integration, register the `dll`:
   ```
    C:\Windows\SysWOW64\regsvr32.exe C:\Where\The\App\Is\pboe.dll
   ```
   To unregister the `dll` later, use:
   ```
   C:\Windows\SysWOW64\regsvr32.exe /u C:\Where\The\App\Is\pboe.dll
   ```
   Normally no admin permissions should be required for the registration.

## Windows 11

The [Windows 10](#windows-10) installation instructions are still valid for Windows 11, although the MSIX package is the suggested method of installation. The Windows 11 immersive menu integration won't function otherwise. :pushpin:

## Linux

Download the Linux AppImage from [Releases](https://github.com/winseros/pboman3/releases).
Make it executable, then run it or register desktop integration for the current
user:

```sh
chmod +x PBOManager-x86_64.AppImage
./PBOManager-x86_64.AppImage
./PBOManager-x86_64.AppImage --install-integration
```

GNOME Files requires `nautilus-python`. Restart GNOME Files or Dolphin after
installation. Use the AppImage's `--uninstall-integration` option to remove the
per-user desktop integration. See [the Linux integration notes](linux/README.md)
for manual and portable installation.

The repository also contains three local Arch package recipes:

- `packaging/arch/pboman3` builds a tagged source release.
- `packaging/arch/pboman3-bin` installs the AppImage release.
- `packaging/arch/pboman3-git` builds the current development branch.

They have not been published to the AUR. See
[the Arch package notes](packaging/arch/README.md) for dependencies and local
installation.

Nix users can run `nix build .` or `nix profile install .`. The flake also
exports `nixosModules.default`, with optional Nautilus integration through
`programs.pboman3.nautilusIntegration`.


## Building from source

1. Set the env variables:

   | Variable | Description                                                       | Example                         |
   |----------|-------------------------------------------------------------------|---------------------------------|
   | Qt6_ROOT | Where QT is located. Needed for CMAKE to build.                   | G:\Qt\6.10.1\msvc2022_64        |


2. Run the script:
   
   ```
   # powershell
   git clone --recurse-submodules git@github.com:winseros/pboman3.git
   cmake -S <path_to_source_code> -B <path_to_build_files>
   cmake --build <path_to_build_files>
   ```

Also, see [how CI builds](.github/workflows/artifcats.yaml).

Linux source builds require Qt 6. Build and install with CMake:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build build
cmake --install build
```

The Linux CI workflow builds and tests with GCC and Clang and produces an
AppImage artifact. See [the Linux packaging notes](packaging/linux/README.md).

## Open in IDE

1. Set the env variabls:

   | Variable | Description                                                       | Example                         |
   |----------|-------------------------------------------------------------------|---------------------------------|
   | Qt6_ROOT | Where QT is located. Needed for CMAKE to build.                   | G:\Qt\6.3.0\msvc2019_64         |
   | PATH     | Where QT binaries are located. Needed for the IDE to run/debug.   | G:\Qt\6.3.0\msvc2019_64\bin     |
   | PATH     | Where OpenSSL binaries are located. Needed for IDE to run/debug.  | G:\Qt\Tools\OpenSSL\Win_x64\bin |

2. Open the the root folder in IDE

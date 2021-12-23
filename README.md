# PBO Manager

A tool to open, pack and unpack ArmA PBO files.

## Key features
 - Can pack/unpack PBO files.
 - Can preview files inside a PBO.
 - Can open mangled (by Mikero's tools) PBO.
 - Integrates with Windows Explorer.
 - Supports PBO metadata provisioning through [pbo.json](doc/pbo_json.md) or [\$PBOPREFIX\$](doc/prefix_files.md) files.

## Screenshots

![Main application window](doc/img/screenshot01.png 'Main application window')
![Unpack PBO context menu](doc/img/screenshot02.png 'Unpack PBO context menu')
![Pack folder as PBO context menu](doc/img/screenshot03.png 'Pack folder as PBO context menu')

## Installation 

:small_blue_diamond: The application requires [Microsoft Visual C++ Redistributable 2015-2019](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run. Othervise, the application will complain regarding the `vcruntime140.dll`.

### Installer
1. Download the `installer` from the [Releases](https://github.com/winseros/pboman3/releases) section.
2. Run the `installer`.

### Manual instllation
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

## Building from source

1. Set the env variables:

   | Variable | Description                                                       | Example                         |
   |----------|-------------------------------------------------------------------|---------------------------------|
   | QT_DIR   | Where QT is located. Needed for CMAKE to build.                   | G:\Qt\6.1.1\msvc2019_64         |


2. Run the script:
   
   ```
   # powershell
   git clone --recurse-submodules git@github.com:winseros/pboman3.git
   cmake -S <path_to_source_code> -B <path_to_build_files>
   cmake --build <path_to_build_files>
   ```

Also, see [how CI builds](.github/workflows/artifcats.yaml).

## Open in IDE

1. Set the env variabls:

   | Variable | Description                                                       | Example                         |
   |----------|-------------------------------------------------------------------|---------------------------------|
   | QT_DIR   | Where QT is located. Needed for CMAKE to build.                   | G:\Qt\6.1.1\msvc2019_64         |
   | PATH     | Where QT binaries are located. Needed for the IDE to run/debug.   | G:\Qt\6.1.1\msvc2019_64\bin     |
   | PATH     | Where OpenSSL binaries are located. Needed for IDE to run/debug.  | G:\Qt\Tools\OpenSSL\Win_x64\bin |

2. Open the the root folder in IDE

# PBO Manager

A tool to pack and unpack ArmA PBO files.

## Key features
 - Can pack/unpack PBO files.
 - Can preview files inside a PBO.
 - Can open mangled (by Mikero's tools) PBO.
 - Integrates with Windows Explorer.

## Screenshots

![Main application window](doc/img/screenshot01.png 'Main application window')
![Unpack PBO context menu](doc/img/screenshot02.png 'Unpack PBO context menu')
![Pack folder as PBO context menu](doc/img/screenshot03.png 'Pack folder as PBO context menu')

## Installation 

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

```
# powershell
git clone --recurse-submodules git@github.com:winseros/pboman3.git
cmake -S <path_to_source_code> -B <path_to_build_files>
cmake --build <path_to_build_files>
```

Also, see [how CI builds](.github/workflows/artifcats.yaml).
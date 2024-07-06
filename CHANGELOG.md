# PBO Manager change log

# Version 1.9.1
- File name sanitization enhanced when extracting a PBO

# Version 1.9.0
- File name sanitization enhanced when extracting a PBO
- Junk filter behavior enhanced

# Version 1.8.0
- Application performance improved

# Version 1.7.0
- [Feature] The junk filter can be disabled in the `Options->Settings` menu.
- [Feature] The `pbo.json` can now be extracted from the user interface.
- [Feature] The `pbo.json` file now extracts automatically when copying or moving files to the filesystem from the main window.
- [Workaround] Signed the MSIX installation package with a self-signed certificate. With the certificate in the system, MSIX can be installed.
- [Fix] Typos in the user interface.

# Version 1.6.0
- [Feature] Added the `Options` menu for the application behavior tweaks
- [Fix] The junk filter from `1.5.0` made less agressive
- [Fix] Spelling errors fixed

## Version 1.5.0
 - [Feature] Windows Explorer integration now supports Windows 11 immersive menu (via the dedicated installer)
 - [Feature] The app tries to filter out the most obvious junk from mangled PBOs
 - [Fix] Now not compressing scripts by default

## Version 1.4.1
 - [Fix] Sometimes the explorer context menu was crasing explorer.exe

## Version 1.4.0
 - [Feature] Dedicated executable `pboc.exe` for usage in scripts

## Version 1.3.0
 - [Feature] Added the `--no-ui` command line flag for usage in scripts

## Version 1.2.0
 - [Feature] Support of PBO metadata provisioning through pbo.json or $PBOPREFIX$ files

## Version 1.1.0
- [Enhancement] UI no longer freezes when opening a PBO
- [Enhancement] UI no longer freezes when pasting files into a PBO from the file system
- [Enhancement] Progress operations are displayed in the Windows taskbar
- [Enhancement] The application now can check for a new version. See "Help->Check for updates"
- [Fix] If make any changes in a PBO while saving, the PBO got corrupted
- [Fix] If "Cancel"  while saving a PBO had a chance to corrupt
- [Fix] The "pack" console command was interpreting "-o" option incorrectly
- [Fix] It was possible to rename a file inside a PBO, if there was a file with the same name in a different casing
- [Fix] The context menu "Extract to <folder_name>" had a superfluous file extension

## Version 1.0.0
- Support of normal PBOs
- Support of Mikero's mangled PBOs
- Windows explorer integration

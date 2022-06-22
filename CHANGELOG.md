# PBO Manager change log

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

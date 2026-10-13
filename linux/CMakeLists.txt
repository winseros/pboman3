include(GNUInstallDirs)

set(PBOM_LINUX_EXECUTABLE "${CMAKE_INSTALL_FULL_BINDIR}/pbom")

# Desktop sessions do not necessarily inherit the shell's PATH, so launchers
# installed below ~/.local must use the final absolute executable path. Quote
# and escape it as one Exec token according to the Desktop Entry specification.
set(PBOM_EXECUTABLE "${PBOM_LINUX_EXECUTABLE}")
string(REPLACE "\\" "\\\\" PBOM_EXECUTABLE "${PBOM_EXECUTABLE}")
string(REPLACE "\"" "\\\"" PBOM_EXECUTABLE "${PBOM_EXECUTABLE}")
string(REPLACE "%" "%%" PBOM_EXECUTABLE "${PBOM_EXECUTABLE}")
set(PBOM_EXECUTABLE "\"${PBOM_EXECUTABLE}\"")

configure_file(
    share/applications/io.github.winseros.pboman3.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/io.github.winseros.pboman3.desktop
    @ONLY)
configure_file(
    share/kio/servicemenus/pboman3-pbo.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/pboman3-pbo.desktop
    @ONLY)
configure_file(
    share/kio/servicemenus/pboman3-folder.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/pboman3-folder.desktop
    @ONLY)

# The Python extension needs a Python string literal rather than Desktop Entry
# quoting. Start again from the unmodified path and escape that representation.
set(PBOM_EXECUTABLE "${PBOM_LINUX_EXECUTABLE}")
string(REPLACE "\\" "\\\\" PBOM_EXECUTABLE "${PBOM_LINUX_EXECUTABLE}")
string(REPLACE "\"" "\\\"" PBOM_EXECUTABLE "${PBOM_EXECUTABLE}")
configure_file(
    share/nautilus-python/extensions/pboman3.py
    ${CMAKE_CURRENT_BINARY_DIR}/pboman3.py
    @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/io.github.winseros.pboman3.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES share/mime/packages/io.github.winseros.pboman3.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages)
install(FILES ${PROJECT_SOURCE_DIR}/pbom/res/app512.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
    RENAME io.github.winseros.pboman3.png)
# KDE requires user-installed service menus to be executable before Dolphin
# considers them authorized.
install(PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/pboman3-pbo.desktop
    ${CMAKE_CURRENT_BINARY_DIR}/pboman3-folder.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/kio/servicemenus)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pboman3.py
    DESTINATION ${CMAKE_INSTALL_DATADIR}/nautilus-python/extensions)

# Portable bundles expose the registration helper with all assets beside it.
install(PROGRAMS install-user.sh uninstall-user.sh
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pboman3/integration)
install(DIRECTORY share
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pboman3/integration)
install(FILES ${PROJECT_SOURCE_DIR}/pbom/res/app512.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pboman3/integration)

"""Nautilus context-menu integration for PBO Manager.

The extension is deliberately launch-only: archive work and user prompts stay in
the Qt application. It supports the callback signatures used by both Nautilus 3
and Nautilus 4.
"""

from __future__ import annotations

import configparser
import os
from pathlib import Path
import subprocess
from typing import Iterable, Optional, Sequence

import gi

try:
    gi.require_version("Nautilus", "4.0")
    from gi.repository import Gio, GObject, Nautilus
except (ImportError, ValueError):
    gi.require_version("Nautilus", "3.0")
    from gi.repository import Gio, GObject, Nautilus


_CONFIG_FILE = Path(
    os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config")
) / "pboman3" / "integration.ini"
_INSTALLED_LAUNCHER = "@PBOM_EXECUTABLE@"


def _launcher() -> Optional[str]:
    parser = configparser.ConfigParser(interpolation=None)
    try:
        with _CONFIG_FILE.open(encoding="utf-8") as stream:
            parser.read_file(stream)
        executable = parser.get("integration", "executable")
    except (OSError, configparser.Error, KeyError):
        executable = _INSTALLED_LAUNCHER
    return executable if os.path.isfile(executable) and os.access(executable, os.X_OK) else None


def _local_path(file_info: Nautilus.FileInfo) -> Optional[str]:
    try:
        location = file_info.get_location()
        path = location.get_path() if location is not None else None
    except (AttributeError, TypeError):
        try:
            path = Gio.File.new_for_uri(file_info.get_uri()).get_path()
        except (AttributeError, TypeError):
            path = None
    return os.fsdecode(path) if path else None


def _selection(callback_args: Sequence[object]) -> list[Nautilus.FileInfo]:
    # Nautilus 3 passes (window, files); Nautilus 4 passes (files,).
    if not callback_args:
        return []
    candidate = callback_args[-1]
    if isinstance(candidate, (list, tuple)):
        return list(candidate)
    try:
        return list(candidate)  # type: ignore[arg-type]
    except TypeError:
        return []


def _is_pbo(file_info: Nautilus.FileInfo, path: str) -> bool:
    try:
        mime_type = file_info.get_mime_type()
    except AttributeError:
        mime_type = None
    return mime_type == "application/x-pbo" or path.casefold().endswith(".pbo")


class PboManagerExtension(GObject.GObject, Nautilus.MenuProvider):
    def _launch(self, command: str, option: str, paths: Iterable[str]) -> None:
        executable = _launcher()
        if executable is None:
            return
        # An argv list is intentional. Never pass file names through a shell.
        subprocess.Popen(
            [executable, command, option, "--", *paths],
            close_fds=True,
            start_new_session=True,
        )

    def _item(
        self,
        name: str,
        label: str,
        command: str,
        option: str,
        paths: Sequence[str],
    ) -> Nautilus.MenuItem:
        item = Nautilus.MenuItem(
            name=f"PboManager::{name}",
            label=label,
            tip=label,
            icon="io.github.winseros.pboman3",
        )
        item.connect("activate", lambda _item: self._launch(command, option, paths))
        return item

    def get_file_items(self, *args: object) -> list[Nautilus.MenuItem]:
        file_infos = _selection(args)
        if not file_infos or _launcher() is None:
            return []

        paths = [_local_path(file_info) for file_info in file_infos]
        if any(path is None for path in paths):
            return []
        local_paths = [path for path in paths if path is not None]

        are_directories = [file_info.is_directory() for file_info in file_infos]
        if all(are_directories):
            return [
                self._item("PackTo", "Pack to…", "pack", "--prompt", local_paths),
                self._item(
                    "PackBeside",
                    "Pack beside folder(s)",
                    "pack",
                    "--beside-input",
                    local_paths,
                ),
            ]

        if not any(are_directories) and all(
            _is_pbo(file_info, path)
            for file_info, path in zip(file_infos, local_paths)
        ):
            return [
                self._item("UnpackTo", "Unpack to…", "unpack", "--prompt", local_paths),
                self._item(
                    "UnpackBeside",
                    "Unpack beside archive(s)",
                    "unpack",
                    "--beside-input",
                    local_paths,
                ),
            ]

        return []

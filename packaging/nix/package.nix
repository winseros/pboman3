{
  lib,
  stdenv,
  cmake,
  ninja,
  fetchurl,
  qt6,
  xdg-utils,
  src ? ../..,
}:

let
  cli11Source = fetchurl {
    url = "https://codeload.github.com/CLIUtils/CLI11/tar.gz/bfffd37e1f804ca4fae1caae106935791696b6a9";
    sha256 = "03c9b7921b8f99ca39ae660b03ebf9bd5a3f4280201f7d04bc5639b1e0496401";
  };
in
stdenv.mkDerivation (finalAttrs: {
  pname = "pboman3";
  version = lib.strings.trim (builtins.readFile (src + "/VERSION"));

  inherit src;

  strictDeps = true;

  nativeBuildInputs = [
    cmake
    ninja
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtwayland
  ];

  cmakeFlags = [
    (lib.cmakeBool "BUILD_TESTING" false)
    (lib.cmakeBool "PBOM_BUILD_GUI" true)
    (lib.cmakeBool "PBOM_BUILD_CLI" true)
    (lib.cmakeFeature "PBOM_VERSION" finalAttrs.version)
  ];

  # Git flake sources omit submodules. Supply the exact revision pinned by the repository.
  postPatch = ''
    mkdir -p __lib__/cli11
    tar -xzf ${cli11Source} -C __lib__/cli11 --strip-components=1
  '';

  postInstall = ''
    install -Dm644 LICENSE "$out/share/licenses/pboman3/LICENSE"
  '';

  qtWrapperArgs = [
    "--prefix PATH : ${lib.makeBinPath [ xdg-utils ]}"
  ];

  meta = {
    description = "GUI and command-line tools for packing and unpacking Arma PBO files";
    homepage = "https://github.com/winseros/pboman3";
    license = lib.licenses.agpl3Plus;
    mainProgram = "pbom";
    platforms = lib.platforms.linux;
  };
})

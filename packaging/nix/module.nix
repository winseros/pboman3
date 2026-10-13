flake:
{
  config,
  lib,
  pkgs,
  ...
}:

let
  cfg = config.programs.pboman3;
  system = pkgs.stdenv.hostPlatform.system;
in
{
  options.programs.pboman3 = {
    enable = lib.mkEnableOption "PBO Manager";

    package = lib.mkOption {
      type = lib.types.package;
      default = flake.packages.${system}.default;
      defaultText = lib.literalExpression "inputs.pboman3.packages.${pkgs.system}.default";
      description = "PBO Manager package to install.";
    };

    nautilusIntegration = lib.mkEnableOption "PBO Manager context-menu actions in GNOME Files";
  };

  config = lib.mkIf cfg.enable {
    environment.systemPackages =
      [ cfg.package ] ++ lib.optional cfg.nautilusIntegration pkgs.nautilus-python;

    environment.pathsToLink = lib.optional cfg.nautilusIntegration
      "/share/nautilus-python/extensions";
  };
}

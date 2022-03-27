{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        pkgs.overlays = [
          (final: prev: {
            esphome = prev.esphome.overrideAttrs (old: rec {
              src = prev.fetchFromGitHub {
                owner = "esphome";
                repo = "esphome";
                rev = "7401fd42a8833b8f04be4f15937bb3c16d4f3fed";
                sha256 = "dsafdsafdjksafeksalfjea";
              };
            });
          })
        ];

        devShell = pkgs.mkShell {
          buildInputs = [
            #(
            #  pkgs.esphome.overrideAttrs (old: rec {
            #    src = pkgs.fetchFromGitHub {
            #      owner = "esphome";
            #      repo = "esphome";
            #      rev = "7401fd42a8833b8f04be4f15937bb3c16d4f3fed";
            #      sha256 = "Ym6LRJQ02cDWQCBA3OMiSBQp+DBy10hLAQdcSOaWDa4=";
            #    };
            #  })
            #)
            pkgs.esphome
          ];
        };
      });
}


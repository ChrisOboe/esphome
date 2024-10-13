{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            (esphome.overrideAttrs {
              src = fetchFromGitHub {
                owner = "esphome";
                repo = "esphome";
                rev = "refs/tags/2024.8.3";
                hash = "sha256-AeiEP1D2u2u8LMNKdBvoA2M3uO6Z9oSG2i5HwGin80M=";
              };
            })
            yq-go
            git-crypt
          ];
        };
      }
    );
}

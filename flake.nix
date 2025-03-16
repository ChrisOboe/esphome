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
            esphome
            # needs at least 2025 version since otherwise mqtt wont be unique
            #(esphome.overrideAttrs {
            #  src = fetchFromGitHub {
            #    owner = "esphome";
            #    repo = "esphome";
            #    rev = "refs/tags/2025.2.0b3";
            #    hash = "sha256-s8R/0QK6tuOzy8wllrHVU5NkkoNH4wtx04dPWIrUXNs=";
            #  };
            #})
            yq-go
            git-crypt
          ];
        };
      }
    );
}

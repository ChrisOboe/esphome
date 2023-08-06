{
  inputs = {
    nixpkgs.url = "https://github.com/NixOS/nixpkgs/archive/pull/229461/head.tar.gz";
    #nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShell = pkgs.mkShell {
        buildInputs = with pkgs; [
          esphome
          yq-go
          git-crypt
        ];
      };
    });
}

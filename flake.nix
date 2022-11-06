{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/f01cf053f6a800c7ce80f79a0afb9e87f7590142";
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
        buildInputs = [
          pkgs.esphome
          pkgs.gnumake
        ];
      };
    });
}

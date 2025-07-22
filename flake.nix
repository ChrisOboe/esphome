{
  inputs = {
    # dependencies
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    # helpers
    flake-utils.url = "github:numtide/flake-utils";

    # development
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    git-hooks = {
      url = "github:cachix/git-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      treefmt-nix,
      git-hooks,
      ...
    }:
    let
      pkgs = import nixpkgs {
        system = "x86_64-linux";
      };

      treefmtEval = treefmt-nix.lib.evalModule pkgs {
        projectRootFile = "flake.nix";
        programs = {
          nixfmt.enable = true;
          statix.enable = true;
          yamlfmt.enable = true;
        };
      };
    in
    {
      formatter.x86_64-linux = treefmtEval.config.build.wrapper;
      checks.x86_64-linux = {
        format-check = treefmtEval.config.build.check self;
        pre-commit-check = git-hooks.lib.x86_64-linux.run {
          src = ./.;
          hooks = {
            statix.enable = true;
          };
        };
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          esphome
          git-crypt
        ];
      };
    };
}

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

      patchedPkgs = nixpkgs.legacyPackages.x86_64-linux.applyPatches {
        name = "nixpkgs-patched";
        src = nixpkgs;
        patches = [
          (nixpkgs.legacyPackages.x86_64-linux.fetchpatch {
            url = "https://patch-diff.githubusercontent.com/raw/NixOS/nixpkgs/pull/422153.patch";
            hash = "sha256-utT+YdwZFZud5tALGKJLTxHA+deBBZNECEfSdfUgfjY=";
          })
        ];
      };

      pkgs = import patchedPkgs {
        system = "x86_64-linux";
        config.sandbox = false;
      };

      treefmtEval = treefmt-nix.lib.evalModule pkgs {
        projectRootFile = "flake.nix";
        programs = {
          nixfmt.enable = true;
          statix.enable = true;
          yamlfmt.enable = true;
        };
      };

      mkEsphome =
        { yaml }:
        pkgs.stdenvNoCC.mkDerivation {
          name = nixpkgs.lib.removeSuffix ".yaml" (baseNameOf yaml);
          __noChroot = true;
          src = ./.;
          buildInputs = [
            pkgs.esphome
          ];
          buildPhase = ''
            esphome compile ${yaml}
          '';

          installPhase = ''
            mkdir $out
            YAML="${yaml}"
            PIOPATH=''${YAML%/*}
            cp $PIOPATH/.esphome/build/*/.pioenvs/*/firmware.bin $out
            cp $PIOPATH/.esphome/build/*/.pioenvs/*/firmware.ota.bin $out
          '';
        };

      # esp32 currently broken till https://github.com/NixOS/nixpkgs/pull/422153 merges
      # https://github.com/NixOS/nixpkgs/issues/227230

      yamls = [
        "./shelly/shelly_1v3.yaml"
        "./shelly/shelly_2plus.yaml"
        "./shelly/shelly_2plus_cover.yaml"
        "./shelly/shelly_2.5.yaml"
        "./electricityMeter/iskra_MT681.yaml"
        "./sinilink/xy-wpce.yaml"
        "./smartplugs/nous_a1t.yaml"
        # "./smartplugs/vansware_mss.yaml" broken and unused atm
        "./relay/garagentor.yaml"
        "./wican/cmax.yaml"
        "./wican/corsa.yaml"
        "./esp01/edifier-bt1280db.yaml"
        "./esp01/tcl.yaml"
      ];
    in
    rec {
      formatter.x86_64-linux = treefmtEval.config.build.wrapper;
      checks.x86_64-linux = {
        format-check = treefmtEval.config.build.check self;
        pre-commit-check = git-hooks.lib.x86_64-linux.run {
          src = ./.;
          hooks = {
            statix.enable = true;
          };
        };
      } // packages.x86_64-linux;

      packages.x86_64-linux = builtins.listToAttrs (
        map (x: {
          name = nixpkgs.lib.removeSuffix ".yaml" (baseNameOf x);
          value = mkEsphome { yaml = x; };
        }) yamls
      );

      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          esphome
          git-crypt
        ];
      };
    };
}

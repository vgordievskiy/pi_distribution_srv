{ 
  nixpkgs ? import <nixpkgs> {  },
  unstable ? import <unstable> {  }
}:
 
let
  pkgs = [
    unstable.conan
    nixpkgs.gcc-unwrapped
    nixpkgs.glibc_multi
    nixpkgs.stdenv.cc.cc
  ];
 
in
  nixpkgs.stdenv.mkDerivation {
    name = "env";
    buildInputs = pkgs;
    NIX_LD_LIBRARY_PATH = nixpkgs.lib.makeLibraryPath [
      nixpkgs.stdenv.cc.cc
    ];
    NIX_LD = nixpkgs.lib.fileContents "${nixpkgs.stdenv.cc}/nix-support/dynamic-linker";
  }

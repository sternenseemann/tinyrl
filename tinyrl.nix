{ stdenv, termbox }:

stdenv.mkDerivation rec {
  pname = "tinyrl";
  version = "unstable";

  src = ./.;

  buildInputs = [ termbox ];

  prePatch = ''
    substituteInPlace Makefile --replace "/usr/local" "$out"
  '';
}

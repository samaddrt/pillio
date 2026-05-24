{ pkgs }: {
  deps = [
    pkgs.cmake
    pkgs.gcc
    pkgs.openssl
    pkgs.pkg-config
    pkgs.gnumake
  ];
}

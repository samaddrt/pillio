{ pkgs }: {
  deps = [
    pkgs.cmake
    pkgs.gcc
    pkgs.openssl
    pkgs.pkg-config
    pkgs.gnumake
    pkgs.python311
    pkgs.python311Packages.pip
    (pkgs.python311.withPackages (ps: [
      ps.python-telegram-bot
      ps.requests
      ps.python-dotenv
    ]))
  ];
}

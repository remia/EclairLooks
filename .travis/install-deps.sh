if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew update;
    brew upgrade cmake;
    brew install llvm;
    brew install ccache;
    export PATH="/usr/local/opt/ccache/libexec:$PATH";
    brew install qt;
    brew install boost;
    brew install libtiff;
    brew install ffmpeg;
    brew install libraw;
    brew install openjpeg;
    brew list --versions;

    ./.travis/fix-deps.sh
fi
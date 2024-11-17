rm -rf build

cmake -S webview -B build
mkdir -p build/core/amalgamation

cmake --build  build

mkdir ../bin
cp -afv build/core/libwebview*.dylib ../bin


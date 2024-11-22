rm -rf build

cmake -S webview -B build
mkdir -p build/core/amalgamation

cmake --build  build --config debug

mkdir ../bin
#cp -afv build/core/libwebview*.dylib ../bin
cp -afv build/core/libwebview*.a ../lib


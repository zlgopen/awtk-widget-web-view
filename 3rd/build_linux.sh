rm -rf build

cmake -S webview -B build -DWEBVIEW_WEBKITGTK_API="4.0" -DCMAKE_BUILD_TYPE=Debug
mkdir -p build/core/amalgamation

cmake --build  build

mkdir ../bin
cp -avf ./build/core/libwebview*.so* ../bin



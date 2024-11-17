set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_SYSROOT /usr/x86_64-w64-mingw32)
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc${WEBVIEW_TOOLCHAIN_EXECUTABLE_SUFFIX}")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++${WEBVIEW_TOOLCHAIN_EXECUTABLE_SUFFIX}")
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

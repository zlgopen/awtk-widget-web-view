import os
import platform
import scripts.app_helper as app

helper = app.Helper(ARGUMENTS)

APP_CXXFLAGS=' '
OS_NAME = platform.system()
if OS_NAME == 'Windows' and not helper.awtk.TOOLS_NAME == 'mingw':
  APP_CXXFLAGS += ' /std:c++latest '
else:
  APP_CXXFLAGS += ' -std=gnu++17 '

APP_ROOT = helper.APP_ROOT
APP_CPPPATH = [
  os.path.join(APP_ROOT, "3rd/webview/core/include")
]
helper.add_platform_libs("Darwin", ["webview"])
helper.add_platform_libs("Windows", ["webviewd"])
helper.add_platform_libs("Linux", ["webviewd"])
helper.add_platform_linkflags("Darwin", " -framework WebKit -lSDL2");
helper.add_cxxflags(APP_CXXFLAGS).add_cpppath(APP_CPPPATH)

helper.set_dll_def('src/web_view.def').call(DefaultEnvironment)

SConscriptFiles = ['src/SConscript', 'demos/SConscript']
helper.SConscript(SConscriptFiles)

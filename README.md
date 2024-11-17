# awtk-widget-web-view

awtk-widget-web-view 是通过 [webview](https://github.com/webview/webview) 提供的接口，实现的 [AWTK](https://github.com/zlgopen/awtk) 自定义控件，使得 [AWTK](https://github.com/zlgopen/awtk) 可以方便的显示 web 页面。

> [webview](https://github.com/webview/webview) 提供了一个跨平台的 webview 接口，是一个非常优秀的开源项目，在此对原作者表示感谢。

![](docs/images/ui.png)

## 特性

* 支持显示 web 页面。
* 支持显示本地文件。
* 支持 C/C++调用 JavaScript。
* 支持 JavaScrip 调用 C/C++函数 。
* 支持 Windows、MacOS、Ubuntu。
  
## 准备

* 安装 [webview](https://github.com/webview/webview) 的编译环境。
* 安装 [AWTK](https://github.com/zlgopen/awtk) 的编译环境。
  

## 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

## 生成示例代码的资源

```
python scripts/update_res.py all
```

## 编译

### 编译 Windows 版本

```
cd 3rd
./build_win32.sh
```

```
scons
```

### 编译 MacOS 版本

```
cd 3rd
./build_macos.sh
```

```
scons
```

### 编译 Ubuntu 版本

```
cd 3rd
./build_linux.sh
```

```
scons
```

## 运行

```
./bin/demo
```

* MacOS 需要设置环境变量：

```
export DYLD_LIBRARY_PATH=bin
```

* Ubuntu 需要设置环境变量：

```
export GDK_BACKEND=x11
```

## 相关项目

* [AWTK-HTML-VIEW](https://github.com/zlgopen/awtk-widget-html-view)

* [AWTK-NETSURF](https://github.com/zlgopen/awtk-widget-netsurf)

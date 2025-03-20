# 🌋火山视窗 - PIV模块



## 💽 安装

本 GitHub 仓库没有上传 lib 和 dll 等二进制文件，因此请下载安装发布版模块

下载地址：https://saier.lanzoul.com/b016cwvle

在线更新：[火山平台升级工具](https://www.lkuaiy.com/rsdetails?id=16201836568092) by 仰望

安装本模块前，必须将 [火山软件开发平台](http://www.voldp.com/) 更新到最新版本，**及时安装论坛的更新包才是最新版**。

本模块部分功能需要 VS 2017 以上才能编译使用。

现在仅提供压缩包文件：

- ##### 请将压缩包内的`piv`文件夹解压到 `火山目录\plugins\vprj_win\classlib\user\`




## 🎯 简介

本模块总共有两个子模块，子模块之间功能不重叠，主要是收录标准有以下差别：

- **PIV基础类**

  除了视窗基本类，不依赖其他任何模块。

- **PIV扩展类**

  会依赖部分官方模块，但不会依赖界面库和其他第三方模块。

本模块封装遵从以下标准：

- **不重复造轮子**

  核心库已有的功能，如果没有可明显改进的地方，不会重复封装。

- **开源**

  所有封装的代码均公开源码。

- **效率优先**

  以执行效率为优先，基本使用C++和嵌入式语法封装。

- **源码编译**

  能直接用源码编译的库，就不会编译成 dll 再封装，缺点是会增加编译耗时，如果其他代码出错，可能会一下子蹦出大量报错。

- **面向对象**

  关联性的功能封装成类，以类实例使用。除了常用命令，尽量减少使用全局类，以「类名.静态方法」调用，减少同名冲突。

#### **PIV** 是什么意思？

> 我也不知道，作为起名废，这名称是沿用了我注册的一个域名，而当初注册这三个字母域名的时候，拼凑的思路上比较靠近 pic、pixiv。
>
> 有人说 PIV 是 VIP 的取反，虽然不是我本意，但这的确很恰当，因为PIV模块会一直免费开源。
>

#### 关于过时功能

> 本模块已有一定的历史，一些早期功能已经不合时宜，但为了兼容用户较早前写的源码，我一直有保留。
>
> 为了让用户尝试新的东西，所以从 2023/07 版开始，我通过火山的条件编译功能屏蔽了一些过时功能。
>
> 如果你有旧源码不想改的话，请在项目属性的「编译时预定义宏」中添加 PIV_ENABLE_OBSOLETE


## 📝 更新日志

 详细的更新日志，请看 [完整文档](README_CN.md)




## 💌 开源项目

- [nlohmann **JSON**](https://github.com/nlohmann/json) `3.11.3` `MIT license`  现代 C++ 的 JSON 库
- **[fmtlog](https://github.com/MengRao/fmtlog)**  `2.2.1`  `MIT license` 纳秒级精度的高性能异步日志记录库
- **[{fmt}](https://github.com/fmtlib/fmt)**  `10.2.0`  `MIT license` 格式化文本库，等同 C++20 std::format 的实现 
- **[md4c](https://github.com/mity/md4c)** `0.4.8`  `MIT license` CommonMark 0.30 规范 + GFM 的 Markdown 解析库
- **[simpleini](https://github.com/brofield/simpleini)** `4.20` `MIT license` 简易的 INI 配置文件读写库
- **[simdutf](https://github.com/simdutf/simdutf)** `6.2.0` ` Apache-2.0, MIT license`  使用 SIMD 指令集加速，每秒数十亿字符的Unicode编码验证和转换库
- **[OpenCC](https://github.com/BYVoid/OpenCC)** `1.1.6` `Apache-2.0 license` 中文简繁转换开源项目，支持词汇级别的转换、异体字转换和地区习惯用词转换
- **[7-zip](https://www.7-zip.org/)** `24.09` `GNU LGPL license` 免费的高压缩率开源压缩软件（采用GNU LGPL协议，只使用它的 dll 文件不会传染）
- **[bit7z](https://github.com/rikyoz/bit7z)** `4.0.10` `Mozilla Public License v2.0` 7-zip 的封装类
- **[MinHook](https://github.com/TsudaKageyu/minhook)** `1.3.3` `BSD-2-Clause License` 极简的 x86/x64 API Hook 库
- **[MMKV](https://github.com/Tencent/MMKV)** `2.1.0` `BSD-3-Clause License` 腾讯开源的高效键值对存储框架
- **[xxHash](https://github.com/Cyan4973/xxHash)** `8.0.2` `BSD-2-Clause License` 速度接近 RAM 限制的高速散列算法
- **[Dear ImGui](https://github.com/ocornut/imgui)** `1.91.4-docking` `MIT license` 轻量级的即时渲染界面开发框架
- **[GLFW](https://www.glfw.org)** `3.3.8` `zlib/libpng license` 跨平台的 OpenGL/Vulkan 桌面应用框架
- **[glad](https://github.com/Dav1dde/glad)** `2023-04-04` `MIT License` 多语言的Vulkan/GL/GLES/EGL/GLX/WGL加载程序生成器
- **[SDL](https://github.com/libsdl-org/SDL)** `2.26.5` `Zlib license` Simple DirectMedia Layer 是一个跨平台的多媒体开发库
- **[Vulkan SDK](https://vulkan.lunarg.com/sdk/home)** `1.3.246.1` `Apache-2.0` 一个跨平台的2D和3D绘图应用程序接口
- **[libgo](https://github.com/yyzybb537/libgo)** `3.0` `MIT license` Golang 风格的 C++11 协程库

※本模块封装的开源项目均采用较宽松的开源协议，如 MIT、BSD、MPLv2 ……



## 🧰 闭源项目

本模块封装了以下闭源项目：

- **[BASS](http://www.un4seen.com)** `2.4`  音频播放支持库

  可以在非盈利软件中免费使用，商用需要购买许可证

- **[VMProtect](https://vmpsoft.com)** `3.7.2`  软件加密壳

  需要购买正版才能正常使用


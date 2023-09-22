# 火山视窗 - libgo协程支持库



来了来了，这是一直有人期望的火山视窗协程支持库。

我最近封装了 std::thread，发现协程的封装技术跟这没什么区别，于是就有了这么一个火山协程库。



## 技术选型

无论火山是否支持 C++20，C++20 的协程都不会是第一选择，因为 C++20 对编译器的版本要求比较高，何况火山的视窗基本类有代码不兼容 C++20。

另外 C++20 协程支持的实现太过底层、太过灵活，只适合库作者而不是用户使用，不过也有比如阿里 async_simple 之类基于 C++20 封装的协程库。

再将目光转向其他的 C++ 协程实现，溜达了一圈，发现大部分热门的轮子是中国人写的。

腾讯的 libco 是不得不提的，大公司出品嘛，有实际用在微信后台上，易语言有人封装过，在创世魂发起的投票里也是得分最高的。

但简单了解过后，我就基本将它排除了，因为 libco 官方只适配 linux 系统，改造到 Windows 上成本太高。

Coost 这个库的问题是太重了，它不单单只有协程，没必要为了这一碟醋去包一顿饺子。

libaco 是 C 语言的库，没有现代 C++ 灵活。

libcopp 是比较新的库，更新也很勤快，目前也可以选择接入 C++20 协程，就是用法比较复杂。虽然也是中国人开源的，但中文和英文的文档都比较简陋，我现在没精力研究，以后有机会的话，可能会深入一下。

最后选择了魅族的 libgo，理由如下：

1. Golang 风格的有栈协程，协程的概念比线程更早，但直到 Golang 的出世才崛起，libgo 从名称上就能看出它是模仿 Golang 的协程实现。libgo 的优势就是像 Golang 协程一样易用。

   注脚：C++20 的无栈协程方案由微软主导，Google 也提出类似 Golang 的有栈协程跟微软竞争，最终是微软胜出了，因为无栈协程的切换和资源占用都更优秀。但无疑 Golang 的有栈协程更加易用，C++20 的协程非常自由，也非常难用，必须封装后才适合普通用户使用。

2. 有栈协程虽然性能比无栈协程低，但不必过于纠结，因为协程主要用在 IO 密集型的业务上，IO 耗时比协程切换耗时要大好几个量级，而且 C++26 也将支持有栈协程。

   注脚：阿里基于 C++20 封装的 async_simple，其中也有无栈和有栈两种实现。

3. 全面的 Windows 支持，最早 libgo 为了方便在 Windows 下开发做了简单支持，但后来重新做了全面的优化。

4. libgo 的功能比 libco 强很多。

5. libgo 只实现了协程相关的功能，称得上轻量级。

6. libgo HOOK 了网络 IO 相关的系统调用，让其他一些第三方库（比如 MySql 客户端）无需改动就能变成异步调用。

7. 我是魅友、我是魅友、我是魅友



## 为什么要使用协程

线程是异步并发的常规方案，但有两个问题：

一、线程占用资源较大，一个线程默认至少需要 1MB 内存，所以线程不适合上千上万的并发场景；

二、线程需要通过内核层操作，线程的创建和切换都有时间成本；

协程可以说是用户态线程，创建和切换协程不需要经过内核，速度更快。ligbo 协程的初始栈堆大小仅为 4KB，意味着协程可以同时进行百万级的并发。

但协程不能无脑替代线程，特别是 CPU 密集型的任务，协程往往会比多线程慢。

协程更适合 IO 密集型任务，比如网络 IO，因为 IO 是这类型任务的主要瓶颈，使用协程可以用更大的并发数来加速。

具体业务是复杂的，有时就是单线程也比多线程和多协程快，所以是否应该使用协程，还是建议做l性能测试再做决定。



## 源码改动

因为火山可以引用的模块太多了，改动的主要目标是避免同名和编译冲突，无其他功能性改动。

libgo 已经有 8 年以上的历史，现在的更新频率很低，修改源码后，后续官方手动同步的成本不算高。

- 修正 libgo 在 MSVC 上的编译警告和报错（已测试通过 VS 2017～2022）

- 将部分 .cpp 合并到 .h，改造成 header-only，将无法 header-only 的 .cpp 文件合并

- 每个头文件都增加了对应的宏，以避免重复 include

- 修改部分比较容易重名的宏

  

## 下载和例子

集成在最新版的 PIV 模块中，推荐用「火山平台升级工具」更新安装（需要等仰望更新）
网盘：https://saier.lanzoul.com/b016cwvle

例子在网盘的 PIV模块例程合集里
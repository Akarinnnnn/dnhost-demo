# dnhost-demo
demo性质  
sshot底下是截图 

## osu.coreclr
让你的O神运行在.NET 10上。但是目前还不支持联网，我也玩不了。也许sb服允许这种客户端，但是现在还是连localhost好了。

使用方法：
1. 克隆仓库
1. 安装.NET 10 SDK(x86版)
1. 安装Visual Studio 2026（社区版即可），选择安装“.NET桌面开发”工作负载和“C++桌面开发”工作负载
1. 准备代码签名证书：
	a. 如果你有，可以跳过自签。只要你有能使用并且被信任的代码签名证书即可。
	a. 自行签发自己的代码签名证书
		1. 怎么签发可以问d指导：deepseek，提示词：请使用Powershell帮我签发一张自签证书，用于exe签名，我要允许导出私钥
		1. 将代码签名证书导入到“受信任的根证书颁发机构”中
		1. 重启电脑（必须）
		1. 其他人无法信任你的自签证书，除非他们也导入了你的自签证书到他们的“受信任的根证书颁发机构”中
1. 打开HostingSdk文件夹，按照[Hosting SDK的README](./HostingSdk/readme.md)中的说明准备.NET 10.0.0 x86 Hosting SDK（版本很重要，差一点都不行）
1. 打开Visual Studio 2026，加载解决方案DotNetHosting.sln
1. 选择“Debug x86”配置（Release也可以，但是会损失Loader的大部分可调试性）
1. 把osu.coreclr项目设置为启动项目
1. 按图片所示，设置osu.coreclr项目的当前目录为osu.Loader项目的输出目录（`..\osu.Loader\bin\Debug\net10.0-windows\`）
   ![如何调整osu.coreclr项目的调试选项](./sshot/如何调整osu.coreclr项目的调试选项.png)
1. 按Ctrl+B编译
1. 编译成功后，打开Developer PowerShell for VS 2026，准备签名loader
1. Developer Powershell进入到osu.Loader项目的输出目录（例如`cd path\to\DotNetHosting\osu.Loader\bin\Debug\net10.0-windows\`）
1. 使用signtool工具签名loader.exe，命令类似于：`signool sign /n "我的证书名 字" /fd SHA512 .\osu.coreclr.exe`，具体参数请参考[signtool文档](https://learn.microsoft.com/windows/win32/seccrypto/signtool)
1. 启动`osu.coreclr`项目，如果一切顺利，你应该能看到osu更新窗口弹出
1. 如果你复制了osu!.exe和相关隐藏dll（osu!auth.dll除外，这是反作弊会发力的）到`osu.coreclr`输出目录下，
   运行`osu.coreclr.exe`应该会弹出osu主界面，注意请事先将Visual Studio移动到另一个桌面（按Win+Tab）或显示器，
1. 以免触发断点时Visual Studio被osu的窗口覆盖什么都无法操作
1. osu.Loader会尝试加载osu!.exe并运行它。请注意，它会修改启动命令行参数，添加禁止联网的参数。如果你想联网（sb服也好别的什么服也好），请自行修改osu.Loader项目的Loader.cs，把禁止联网的参数去掉。
1. 祝你好运，我启动的时候，因为绘图模块传了空指针，引发了用户模式A卡驱动的Access Violation。希望你能玩。

如果有人愿意帮忙完善这个项目，欢迎发issue和PR探讨。

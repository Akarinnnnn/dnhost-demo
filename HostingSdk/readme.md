# 这个目录干吗的

这个目录放Nuget那下载的`nethost`库、头文件等  

## 这些东西去哪下

[点这个链接](https://www.nuget.org/packages?q=runtime.+.Microsoft.NETCore.DotNetAppHost)搜一个合适的，
比如runtime.win-x86.Microsoft.NETCore.DotNetAppHost，点进去再点_Download Package_，下回来的nuget包当zip解压就行

## 怎么放

以6.0.4版本的sdk为例  
在这里新建一个叫`6.0.4`文件夹，`6.0.4\inc\`放头文件，`6.0.4\x64\`放64位的dll、lib啥的，32位依此类推放到`6.0.4\x86\`

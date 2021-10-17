## 一些限制
这些api基本只能用在默认appdomain上

## ICLRRuntimeHost4
* UnloadAppDomain2: 没用过不知道

## ICLRRuntimeHost2
* CreateDelegate:  
	备注：
	1. 这条路子无视托管方法的可访问性  
	2. 但必须是静态的  
	3. 似乎可以直接调用，调用约定状况不明
	4. 类型不能是开放泛型类型  

	细节：  
	`className`也许不必是程序集限定名称  
	第一轮先找不重名的，第二轮再找重名的  
	非静态的、开放泛型的找到都不要  
	`UnmanagedCallersOnly`直接获得stub  
	托管不清楚

	参考：  
	[FM_Flags::FM_Unique, memberload.h](https://github.com/dotnet/runtime/blob/57bfe474518ab5b7cfe6bf7424a79ce3af9d6657/src/coreclr/vm/memberload.h#L175)  
	[CorHost2::CreateDelegate, corhost.cpp](https://github.com/dotnet/runtime/blob/7088332d24a276f557e1fe3612947fb17e0f04a3/src/coreclr/vm/corhost.cpp#L694-L786)

* Authenticate：也是不做事的
* RegisterMacEHPort: 屁事不干，当不存在
* SetStartupFlags: 会清空旧的StartupFlags，而且你还拿不到旧的，小心使用
* DllGetActivationFactory：感觉也是不做事的
* ExecuteAssembly：
	0. 程序集必须包含合适的`Main`
	1. `pwzAssemblyPath`不能为空指针或空字符串，否则返回`E_POINTER`
	2. `argc`不能小于0。仅当`argc`为0时，`argv`可以为空
	3. `pReturnValue`是`Main`返回值

## ICLRRuntimeHost
* SetHostConrtol, GetCLRControl：两个不做事的

## 根本没定义的接口
* ICLRControl
* IHostControl
* IActivationFactory
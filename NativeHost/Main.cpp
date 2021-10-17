#include <Windows.h>
#include <cassert>
#include <iostream>
#include <string>
#include <filesystem>

#include ".NET 5.0.10/nethost.h"
#include "fxr.hpp"
// #include "policy.hpp"
#include ".NET 5.0.10/coreclr_delegates.h"
#include ".NET 5.0.10/mscoree.h"
#include <vector>


/// <remarks>
/// ppUnk可以是
/// ICLRRuntimeHost**
/// ICLRRumtimeHost2**
/// ICLRRumtimeHost4**
/// Uinx系统还有
/// IPrivateManagedExceptionReporting**
/// </remarks>
// typedef HRESULT(CORECLR_DELEGATE_CALLTYPE* FnGetCLRRuntimeHost)(const IID& riid, ICLRRuntimeHost4** ppUnk);

enum class HostE : HRESULT
{
	BufferTooSmall = 0x80008098i32
};

HostFxR fxr;
// HostPolicy pol;

using std::wcout;
using std::endl;

static HANDLE clrerr;
static HANDLE hstderr = GetStdHandle(STD_ERROR_HANDLE);

static void err_writer(const wchar_t* msg)
{
	auto size = wcslen(msg);
	WriteFile(clrerr, msg, (DWORD)size * 2, nullptr, nullptr);
	WriteConsoleW(hstderr, msg, (DWORD)size, nullptr, nullptr);
}

void GetFxr(std::wstring& path, size_t& pathsize, const get_hostfxr_parameters* p)
{
	HRESULT hr = get_hostfxr_path(path.data(), &pathsize, p);
	path.resize(pathsize);

	if (hr == (HRESULT)HostE::BufferTooSmall)
		get_hostfxr_path(nullptr, &pathsize, p);
}

int wmain(int argc, wchar_t* argv[])
{
	std::wcout.imbue(std::locale{ "" });
	std::wstring strbuff{ 260, L'\0', std::allocator<wchar_t>{} };
	HRESULT hr = S_OK;
	clrerr = CreateFileW(
		L".\\CLR Error Log.txt",
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN,
		nullptr
	);
	WriteFile(clrerr, L"\ufeff", 4, nullptr, nullptr);

	size_t pathsize = 260;
	GetFxr(strbuff, pathsize, nullptr);
	wcout << strbuff << endl;

	#if 0 // advanced search
	get_hostfxr_parameters param{ sizeof(param), nullptr, nullptr };
	param.assembly_path = LR"(your\custom\hostfxr\directory)";
	GetFxr(strbuff, pathsize, &param);
	wcout << strbuff << endl;

	param.assembly_path = nullptr;
	param.dotnet_root = LR"(..\Build\Debug_x64\)"; // where dotnet.exe locates
	GetFxr(strbuff, pathsize, &param);
	wcout << strbuff << endl;
	#endif

	#if 1 // FXR Test
	wcout << L"demo fxr" << endl;
	auto hfxr = LoadLibraryW(strbuff.c_str());
	fxr.Load(hfxr);
	fxr.SetErrorWriter(err_writer);
	hr = fxr.InitCfg(
		L".\\NativeHost.runtimeconfig.json",
		nullptr,
		&fxr.Handle
	);

	if (FAILED(hr))
		return hr;

	try
	{
		std::filesystem::path cd = std::filesystem::canonical(L".\\");
		wcout << L"\nset APP_PATH and NATIVE_DLL_SEARCH_DIRECTORIES\n";
		fxr.SetRtProp(fxr.Handle, L"APP_PATHS", cd.c_str());
		const wchar_t* old_native = nullptr;
		fxr.GetRtProp(fxr.Handle, L"NATIVE_DLL_SEARCH_DIRECTORIES", &old_native);
		strbuff.assign(cd.native()).append(old_native);
		fxr.SetRtProp(fxr.Handle, L"NATIVE_DLL_SEARCH_DIRECTORIES", strbuff.c_str());
	}
	catch (std::filesystem::filesystem_error& e)
	{
		wcout << L"fs errc:" << e.code().value() << L", path:" << e.path1().c_str();
	}

	wcout << L"\ninspect props\n";
	{
		std::vector<const wchar_t*> k, v;
		size_t propsize = 0;
		fxr.GetRtProps(fxr.Handle, &propsize, nullptr, nullptr);
		k.resize(propsize);
		v.resize(propsize);
		fxr.GetRtProps(fxr.Handle, &propsize, k.data(), v.data());
		for (size_t i = 0; i != propsize; ++i)
			wcout << k[i] << L':' << v[i] << endl;;
	}
	#endif

	#if 0 // hostpolicy，不搞了，太复杂
	wcout << L"\ndemo policy\n";
	auto hpol = GetModuleHandleW(L"hostfxr");
	pol.Load(hpol);
	corehost_initialize_request_t initreq;
	initreq.version = sizeof(initreq);
	// initreq

	#endif // 1

	wcout << L"\nGet managed delegate\n";
	load_assembly_and_get_function_pointer_fn LoadAsmForFunc = nullptr;
	typedef int (__cdecl *unhandled_exception_listener)(HRESULT hr, const wchar_t* message, const wchar_t* desc);
	int(__cdecl * managedEntry)(int, int) = nullptr;
	int(__cdecl * eh_surscibe)(unhandled_exception_listener listener) = nullptr;
	hr = (fxr.GetDelegate(fxr.Handle, hdt_load_assembly_and_get_function_pointer, (void**)&LoadAsmForFunc));
	if (FAILED(hr))
		return hr;
	auto hcoreclr = GetModuleHandleW(L"coreclr.dll");
	assert(hcoreclr);
	auto GetClrRuntimeHost = (FnGetCLRRuntimeHost)GetProcAddress(hcoreclr, "GetCLRRuntimeHost");
	assert(GetClrRuntimeHost);

	ICLRRuntimeHost4* host4 = nullptr;
	hr = GetClrRuntimeHost(IID_ICLRRuntimeHost4, (IUnknown**)&host4);
	if (FAILED(hr))
	{
		strbuff.resize(FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 2052, strbuff.data(), strbuff.capacity(), nullptr));
		wcout << L"Failed to find ICLRRuntimeHost, msg is :" << strbuff << endl;
		return hr;
	}
	// https://docs.microsoft.com/zh-cn/dotnet/framework/unmanaged-api/hosting/startup-flags-enumeration
	// 我超，不太敢用
	/*host4->SetStartupFlags(
		(STARTUP_FLAGS)(STARTUP_CONCURRENT_GC | STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN)
	);*/

	// 你他妈根本不实现是吗？
	// ICLRControl* clr4 = nullptr;
	// host4->GetCLRControl(&clr4);

	// host2新东西
	DWORD appdomainId = 0;
	INT_PTR entryDlg = 0;
	host4->GetCurrentAppDomainId(&appdomainId);
	host4->CreateDelegate(
		appdomainId,
		L"ManagedEntry",
		L"ManagedEntry.Entry5",
		L"CdeclEntryPoint",
		&entryDlg
	);

	hr = LoadAsmForFunc(
		L"ManagedEntry.dll",
		L"ManagedEntry.Entry5,ManagedEntry",
		L"CdeclEntryPoint",
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr,
		(void**)&managedEntry
	);

	wcout << L"Entry from c-style abi:\n"
		<< (UINT_PTR)managedEntry
		<< L"\nEntry from COM abi:\n"
		<< (UINT_PTR)entryDlg << endl;

	if (FAILED(hr))
	{
		strbuff.resize(FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 2052, strbuff.data(), strbuff.capacity(), nullptr));
		wcout << L"Failed to get managed entry, msg is :" << strbuff << endl;
		return hr;
	}
	hr = LoadAsmForFunc(
		L"ManagedEntry.dll",
		L"ManagedEntry.Entry5,ManagedEntry",
		L"SubscribeUnhandledException",
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr,
		(void**)&eh_surscibe
	);
	if (FAILED(hr))
	{
		strbuff.resize(FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 2052, strbuff.data(), strbuff.capacity(), nullptr));
		wcout << L"Failed to get managed entry, msg is :" << strbuff << endl;
		return hr;
	}

	int ret = managedEntry(1, 2);
	wcout << L"Returned value:" << ret << endl;
	#if 0 // clr interface

	auto hcoreclr = GetModuleHandleW(L"coreclr.dll");

	//
	// host4->  
	#endif

	// assert(SetEnvironmentVariableW(L"SDL_AUDIODRIVER", L"winmm") == 0);
	// HRESULT hr = CoInitialize(nullptr);
	// assert(hr >= 0); // 

	// pol.Unload();
	fxr.Close(fxr.Handle);

	return 0;
}
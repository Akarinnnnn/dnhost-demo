#define _WIN32_LEAN_AND_MEAN 1

#include <windows.h>
#include "fxr.hpp"
#include <10.0.0/inc/nethost.h>
#include <10.0.0/inc/coreclr_delegates.h>

#include <concepts>
#include <span>
#include <string_view>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <coreclr.h>

struct cast_elem
{
	wchar_t** src;
	wchar_t** const end;

	using element_type = std::wstring_view;

	cast_elem(int argc, wchar_t** argv)
		:src(argv), end(argv + argc) {}

	cast_elem(cast_elem& r) : src(r.src), end(r.end) {}

	// movenext
	cast_elem& operator++()
	{
		src++;
		if (src == end)
			throw std::out_of_range("pointer out of bound");
	}

	std::wstring_view operator*() { return { *src }; }
};

auto constexpr cfgfilename = L".\\osu.coreclr.cfg.txt";

void InputExecutable(std::filesystem::path& fxr, std::filesystem::path& osu)
{
	std::cout << "hostfxr.dll path:";
	std::cin >> fxr;
	std::cout << "osu!.exe path:";
	std::cin >> osu;
}

void LoadFromFile(std::filesystem::path& fxr, std::filesystem::path& osu)
{
	std::ifstream cfg{ cfgfilename, std::ios::binary };
	if (!cfg.is_open())
		return InputExecutable(fxr, osu);

	std::wstring buff;
	buff.resize(2048);
	cfg.read((char*)buff.data(), 4096);
	std::wstring_view remaining{ buff };
	auto lf = remaining.find(L'\n');
	if (lf == std::wstring::npos)
		return InputExecutable(fxr, osu);

	fxr = remaining.substr(0, lf);
	remaining = remaining.substr(lf);

	lf = buff.find(L'\n', lf);
	if (lf == std::wstring::npos)
		return InputExecutable(fxr, osu);

	osu = remaining.substr(0, lf);
}

void UseNethost(std::filesystem::path& fxr, std::filesystem::path& osu)
{
	std::wstring buff{ 250, L'\0', std::allocator<wchar_t>() };
	size_t pathSize = 250;
	auto hr = 0;
	if (hr = get_hostfxr_path(buff.data(), &pathSize, nullptr) == 0x80008098)
	{
		buff.resize(pathSize);
		hr = get_hostfxr_path(buff.data(), &pathSize, nullptr);
	}

	if (hr != 0) exit(hr); // TODO throw fnf? interactive?
	fxr = std::wstring_view{ buff.data(), pathSize };

	osu = std::filesystem::current_path();
	osu /= L"osu!.exe";
	if (!std::filesystem::exists(osu))
		exit(2); // TODO
}

void SaveConfig(std::filesystem::path& fxr, std::filesystem::path& osu)
{
	std::ofstream cfg{ cfgfilename, std::ios::binary | std::ios::out | std::ios::trunc };
	if (!cfg.is_open())
		return;
	std::wstring buff = fxr;
	auto constexpr delim = "\n\0";
	cfg.write((char*)buff.data(), buff.size());
	cfg.write(delim, 2);
	buff = osu;
	cfg.write((char*)buff.data(), buff.size());
	cfg.write(delim, 2);
	cfg.close();
}

int PrintOsError(int errc)
{
	std::string s{ 200, '\0', std::allocator<char>() };
	auto result = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errc, 0, s.data(), 200, nullptr);
	s.resize(result);
	std::cout << s;

	return errc;
}

HostFxR LocateAndLoad(std::filesystem::path& osu, std::filesystem::path& fx, bool interactive)
{
	HostFxR fxr{};
	std::filesystem::path pathFxr;
	// 1. load fxr
	// 1.1 locate
	if (interactive)
	{
		InputExecutable(pathFxr, osu);
		SaveConfig(pathFxr, osu);
	}
	else
	{
		if (std::filesystem::exists(cfgfilename))
		{
			LoadFromFile(pathFxr, osu);
		}
		else
		{
			UseNethost(pathFxr, osu);
		}
	}

	// 1.2 load
	{
		fx = pathFxr / L"..\\..\\";
		if (!std::filesystem::exists(fx))
			exit(1);

		auto hfxr = LoadLibraryExW(pathFxr.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (hfxr == nullptr)
		{
			auto errc = GetLastError();
			PrintOsError(errc);
			return fxr;
		}
		fxr.Load(hfxr);
	}
	return fxr;
}

void InitHostContext(HostFxR& fxr, std::filesystem::path& fx, std::filesystem::path& osu, int argc, const wchar_t** argv)
{
	hostfxr_initialize_parameters params{
		sizeof(params),
		osu.c_str(),
		nullptr
	};

	HRESULT hr = fxr.InitCfg(L"osu!.runtimeconfig.json", &params, &fxr.Handle);
	if (FAILED(hr)) exit(hr);
}

void BuildProperties(HostFxR& fxr, std::filesystem::path& osu)
{
	auto osuBase = osu.parent_path();
	std::wstring buff = osuBase;
	buff.reserve(2048);
	const wchar_t* propValue = nullptr;
	fxr.SetRtProp(fxr.Handle, L"APP_CONTEXT_BASE_DIRECTORY", buff.c_str());
	fxr.SetRtProp(fxr.Handle, L"PROBING_DIRECTORIES", buff.c_str());

	fxr.GetRtProp(fxr.Handle, L"NATIVE_DLL_SEARCH_DIRECTORIES", &propValue);
	buff.append(propValue);
	fxr.SetRtProp(fxr.Handle, L"NATIVE_DLL_SEARCH_DIRECTORIES", buff.c_str());

	// TPA
	buff.clear();
	for (auto& ent : std::filesystem::directory_iterator(osuBase))
	{
		if (ent.is_regular_file() && ent.path().extension().compare(L".dll") == 0)
			buff.append(ent.path().native()).append(1, L';');
	}
	fxr.GetRtProp(fxr.Handle, L"TRUSTED_PLATFORM_ASSEMBLIES", &propValue);
	fxr.SetRtProp(fxr.Handle, L"TRUSTED_PLATFORM_ASSEMBLIES", buff.append(propValue).c_str());
}

int wmain(int argc, wchar_t** argv)
{
	using namespace std::string_view_literals;
	std::filesystem::path osu;
	std::filesystem::path fxpath;
	HRESULT hr = S_OK;
	bool interactive = argc > 1 && argv[1] == L"interactive"sv;
	HostFxR fxr = LocateAndLoad(osu, fxpath, interactive);
	// using fxr
	if (fxr.SetRtProp == nullptr)
		return PrintOsError(GetLastError());

	std::vector<const wchar_t*> osuArgv{ argv, argv + argc, std::allocator<const wchar_t*>() };
	osuArgv[0] = osu.c_str();
	if (interactive)
		osuArgv.erase(osuArgv.begin() + 1);

	InitHostContext(fxr, fxpath, osu, osuArgv.size(), osuArgv.data());
	BuildProperties(fxr, osu);
	std::filesystem::current_path(osu.parent_path());

	// load coreclr
	load_assembly_and_get_function_pointer_fn getDelegate;
	hr = fxr.GetDelegate(fxr.Handle, hdt_load_assembly_and_get_function_pointer, (void**)& getDelegate);
	if (FAILED(hr))
		return hr;

#if 0
	ICLRRuntimeHost4* host = nullptr;
	{
		auto hclr = GetModuleHandleW(L"coreclr");
		if (hclr == nullptr)
		{
			return PrintOsError(GetLastError());
		}
		auto GetCLRRuntimeHost = (FnGetCLRRuntimeHost)GetProcAddress(hclr, "GetCLRRuntimeHost");
		if (GetCLRRuntimeHost == nullptr)
		{
			std::cout << "wtf? fakeclr?\n";
			return PrintOsError(GetLastError());
		}

		hr = GetCLRRuntimeHost(IID_ICLRRuntimeHost4, (IUnknown**)&host);
		if (hr != 0)
		{
			PrintOsError(hr);
			return hr;
		}
	}
	DWORD exitcode = 0;
	DWORD defaultDomainId;
	hr = host->Start();
	if (FAILED(hr)) exit(hr);

	host->GetCurrentAppDomainId(&defaultDomainId);
	// signed host required
	hr = host->ExecuteAssembly(defaultDomainId, osu.c_str(), osuArgv.size(), osuArgv.data(), &exitcode);
	if (FAILED(hr)) exit(hr);

	host->Release();
#endif // 0

#pragma region fxr execute
	DWORD exitcode = 0;
	int (*LoadAndExecute)(int argc, const wchar_t** argv, const wchar_t* osuExe, int asmPathLen) = nullptr;
	std::filesystem::path loaderPath = std::filesystem::current_path() / L"osu.Loader.dll";
	hr = getDelegate(
		loaderPath.c_str(),
		L"osu.Loader.Loader, osu.Loader",
		L"LoadAndExecute",
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr,
		(void**)&LoadAndExecute
	);
	if (FAILED(hr)) exit(hr);
	exitcode = LoadAndExecute(osuArgv.size(), osuArgv.data(), osu.c_str(), osu.native().length());
#pragma endregion

	fxr.Close(fxr.Handle);
	return exitcode;
}


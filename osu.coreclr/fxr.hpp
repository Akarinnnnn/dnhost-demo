#pragma once
#include <10.0.0/inc/hostfxr.h>

struct HostFxR
{
	hostfxr_handle Handle;
	//init1
	hostfxr_initialize_for_dotnet_command_line_fn InitCli;
	//init and run
	hostfxr_run_app_fn RunApp;
	//init2
	hostfxr_initialize_for_runtime_config_fn InitCfg;
	//unload
	hostfxr_close_fn Close;

	#pragma region Runtime Property
	hostfxr_get_runtime_property_value_fn GetRtProp;
	hostfxr_set_runtime_property_value_fn SetRtProp;
	//get all prop
	hostfxr_get_runtime_properties_fn GetRtProps;
	#pragma endregion

	// get static func delegate / rt delegate
	hostfxr_get_runtime_delegate_fn GetDelegate;

	// invoke main1
	hostfxr_main_fn Main;
	// invoke main2
	hostfxr_main_startupinfo_fn MainWithStartupInfo;
	// invoke main3
	hostfxr_main_bundle_startupinfo_fn MainWithBundle;

	#pragma region Advanced
		hostfxr_set_error_writer_fn SetErrorWriter;
		#pragma region .NET SDK Relatived
		typedef void (HOSTFXR_CALLTYPE* hostfxr_get_available_sdks_result_fn)(
			int32_t sdk_count,
			const char_t* sdk_dirs[]);

		typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_get_available_sdks_fn)(
			const char_t* exe_dir,
			hostfxr_get_available_sdks_result_fn result);
		// Returns the list of all available SDKs ordered by ascending version.
		//
		// Invoked by MSBuild resolver when the latest SDK used without global.json
		// present is incompatible with the current MSBuild version. It will select
		// the compatible SDK that is closest to the end of this list.
		//
		// Parameters:
		//    exe_dir
		//      The path to the dotnet executable.
		//
		//    result
		//      Callback invoke to return the list of SDKs by their directory paths.
		//      String array and its elements are valid for the duration of the call.
		//
		// Return value:
		//   0 on success, otherwise failure
		hostfxr_get_available_sdks_fn GetSdks;

		enum hostfxr_resolve_sdk2_flags_t
		{
			disallow_prerelease = 0x1,
		};
		enum hostfxr_resolve_sdk2_result_key_t
		{
			resolved_sdk_dir = 0,
			global_json_path = 1,
		};
		typedef void (*hostfxr_resolve_sdk2_result_fn)(
			hostfxr_resolve_sdk2_result_key_t key,
			const char_t* value);
		typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_resolve_sdk2_fn)(
			const char_t* exe_dir,
			const char_t* working_dir,
			int32_t flags,
			hostfxr_resolve_sdk2_result_fn result);

		// Determines the directory location of the SDK accounting for
		// global.json and multi-level lookup policy.
		//
		// Invoked via MSBuild SDK resolver to locate SDK props and targets
		// from an msbuild other than the one bundled by the CLI.
		//
		// Parameters:
		//    exe_dir
		//      The main directory where SDKs are located in sdk\[version]
		//      sub-folders. Pass the directory of a dotnet executable to
		//      mimic how that executable would search in its own directory.
		//      It is also valid to pass nullptr or empty, in which case
		//      multi-level lookup can still search other locations if
		//      it has not been disabled by the user's environment.
		//
		//    working_dir
		//      The directory where the search for global.json (which can
		//      control the resolved SDK version) starts and proceeds
		//      upwards.
		//
		//   flags
		//      Bitwise flags that influence resolution.
		//         disallow_prerelease (0x1)
		//           do not allow resolution to return a prerelease SDK version
		//           unless  prerelease version was specified via global.json.
		//
		//   result
		//      Callback invoked to return values. It can be invoked more
		//      than once. String values passed are valid only for the
		//      duration of a call.
		//
		//      If resolution succeeds, result will be invoked with
		//      resolved_sdk_dir key and the value will hold the
		//      path to the resolved SDK director, otherwise it will
		//      be null.
		//
		//      If global.json is used then result will be invoked with
		//      global_json_path key and the value  will hold the path
		//      to global.json. If there was no global.json found,
		//      or the contents of global.json did not impact resolution
		//      (e.g. no version specified), then result will not be
		//      invoked with global_json_path key.
		//
		// Return value:
		//   0 on success, otherwise failure
		//   0x8000809b - SDK could not be resolved (SdkResolverResolveFailure)
		hostfxr_resolve_sdk2_fn ResolveSdk;
		#pragma endregion
	#pragma endregion

	void Load(HMODULE hfxr)
	{
		InitCli = (hostfxr_initialize_for_dotnet_command_line_fn)GetProcAddress(hfxr, "hostfxr_initialize_for_dotnet_command_line");
		RunApp = (hostfxr_run_app_fn)GetProcAddress(hfxr, "hostfxr_run_app");
		InitCfg = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress(hfxr, "hostfxr_initialize_for_runtime_config");
		Close = (hostfxr_close_fn)GetProcAddress(hfxr, "hostfxr_close");
		GetRtProp = (hostfxr_get_runtime_property_value_fn)GetProcAddress(hfxr, "hostfxr_get_runtime_property_value");
		GetDelegate = (hostfxr_get_runtime_delegate_fn)GetProcAddress(hfxr, "hostfxr_get_runtime_delegate");
		Main = (hostfxr_main_fn)GetProcAddress(hfxr, "hostfxr_main");
		MainWithStartupInfo = (hostfxr_main_startupinfo_fn)GetProcAddress(hfxr, "hostfxr_main_startupinfo");
		MainWithBundle = (hostfxr_main_bundle_startupinfo_fn)GetProcAddress(hfxr, "hostfxr_main_bundle_startupinfo");
		GetSdks = (hostfxr_get_available_sdks_fn)GetProcAddress(hfxr, "hostfxr_get_available_sdks");
		ResolveSdk = (hostfxr_resolve_sdk2_fn)GetProcAddress(hfxr, "hostfxr_resolve_sdk2");

		SetRtProp = (hostfxr_set_runtime_property_value_fn)GetProcAddress(hfxr, "hostfxr_set_runtime_property_value");
		GetRtProps = (hostfxr_get_runtime_properties_fn)GetProcAddress(hfxr, "hostfxr_get_runtime_properties");
		SetErrorWriter = (hostfxr_set_error_writer_fn)GetProcAddress(hfxr, "hostfxr_set_error_writer");
	}
};
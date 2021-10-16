// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

#ifndef __COREHOST_CONTEXT_CONTRACT_H__
#define __COREHOST_CONTEXT_CONTRACT_H__

#include "hostpolicy.h"

#if defined(_WIN32)
	#define HOSTPOLICY_CALLTYPE __cdecl
#else
	#define HOSTPOLICY_CALLTYPE
#endif
#if defined(_WIN32)
	#ifdef _WCHAR_T_DEFINED
		typedef wchar_t char_t;
	#else
		typedef unsigned short char_t;
	#endif
#else
	typedef char char_t;
#endif

// [flags]
enum initialization_options_t : uint32_t
{
	none = 0x0,
	wait_for_initialized = 0x1,                // Wait until initialization through a different request is completed
	get_contract = 0x2,                        // Get the contract for the initialized hostpolicy
	context_contract_version_set = 0x80000000, // The version field has been set in the corehost_context_contract
											   // on input indicating the maximum size of the buffer which can be filled
};

// Delegates for these types will have the stdcall calling convention unless otherwise specified
enum class coreclr_delegate_type
{
	invalid = 0,
	com_activation = 1,
	// IJW activiation
	load_in_memory_assembly = 2,
	winrt_activation = 3,
	com_register = 4,
	com_unregister = 5,
	load_assembly_and_get_function_pointer = 6,
	get_function_pointer = 7,

	__last, // Sentinel value for determining the last known delegate type
};

#pragma pack(push, _HOST_INTERFACE_PACK)
struct corehost_initialize_request_t
{
	size_t version;
	strarr_t config_keys;
	strarr_t config_values;
};

struct corehost_context_contract
{
	size_t version;
	int (HOSTPOLICY_CALLTYPE *get_property_value)(
		const char_t *key,
		/*out*/ const char_t **value);
	int (HOSTPOLICY_CALLTYPE *set_property_value)(
		const char_t *key,
		const char_t *value);
	int (HOSTPOLICY_CALLTYPE *get_properties)(
		/*inout*/ size_t *count,
		/*out*/ const char_t **keys,
		/*out*/ const char_t **values);
	int (HOSTPOLICY_CALLTYPE *load_runtime)();
	int (HOSTPOLICY_CALLTYPE *run_app)(
		const int argc,
		const char_t **argv);
	int (HOSTPOLICY_CALLTYPE *get_runtime_delegate)(
		coreclr_delegate_type type,
		/*out*/ void **delegate);
	size_t last_known_delegate_type; // Added in 5.0
};
#pragma pack(pop)

#endif // __COREHOST_CONTEXT_CONTRACT_H__

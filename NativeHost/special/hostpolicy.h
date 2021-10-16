// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

#ifndef __HOSTPOLICY_H__
#define __HOSTPOLICY_H__

#include <stdint.h>
#include "host_interface.h"
// #include "host_interface.h"

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
	typedef int(HOSTPOLICY_CALLTYPE* corehost_unload_fn) ();
	typedef int(HOSTPOLICY_CALLTYPE* corehost_load_fn) (const host_interface_t* init);
#if 0
typedef int(HOSTPOLICY_CALLTYPE* corehost_main_fn) (
	const int argc,
	const char_t** argv);
typedef int(HOSTPOLICY_CALLTYPE* corehost_main_with_output_buffer_fn) (
	const int argc,
	const char_t** argv,
	char_t* buffer,
	int32_t buffer_size,
	int32_t* required_buffer_size);
#endif // 0

struct corehost_initialize_request_t;
struct corehost_context_contract;

typedef void(HOSTPOLICY_CALLTYPE *corehost_error_writer_fn) (const char_t *message);
typedef corehost_error_writer_fn(HOSTPOLICY_CALLTYPE *corehost_set_error_writer_fn) (corehost_error_writer_fn error_writer);

typedef int(HOSTPOLICY_CALLTYPE *corehost_initialize_fn)(
	const corehost_initialize_request_t *init_request,
	uint32_t options,
	corehost_context_contract *handle);

#endif //__HOSTPOLICY_H__

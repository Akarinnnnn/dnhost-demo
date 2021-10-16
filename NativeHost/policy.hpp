#pragma once
#include "special/hostpolicy.h"
#include "special/corehost_context_contract.h"

struct HostPolicy
{
	corehost_initialize_fn Init;
	corehost_unload_fn Unload;
	corehost_set_error_writer_fn SetErrWriter;

	void Load(HMODULE h)
	{
		Init = (corehost_initialize_fn)GetProcAddress(h, "corehost_initialize");
		Unload = (corehost_unload_fn)GetProcAddress(h, "corehost_unload");
		SetErrWriter = (corehost_set_error_writer_fn)GetProcAddress(h, "corehost_set_error_writer");
	}
};
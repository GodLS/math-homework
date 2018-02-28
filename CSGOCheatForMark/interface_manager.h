#pragma once
#include "Windows.h"

class interface_manager
{
public:

	class interface_req
	{
		using InstantiateInterfaceFn = void*(*)();

	public:

		InstantiateInterfaceFn m_CreateFn;
		const char *m_pName;

		interface_req *m_pNext;
	};

	template<typename T>
	static T *bf_interface(const char *modName, const char *ifaceName, bool exact = false)
	{
		T *iface = nullptr;
		const int part_match_len = strlen(ifaceName);

		const DWORD iface_fn = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(modName), "CreateInterface"));

		if (!iface_fn)
			return nullptr;

		const unsigned int jmpStart = static_cast<unsigned int>(iface_fn)+4;
		const unsigned int jmpTarget = jmpStart + *reinterpret_cast<unsigned int*>(jmpStart + 1) + 5;

		interface_req *interface_list = **reinterpret_cast<interface_req***>(jmpTarget + 6);

		for (interface_req *cur = interface_list; cur; cur = cur->m_pNext)
		{
			if (exact != true)
			{
				if (!strncmp(cur->m_pName, ifaceName, part_match_len) && std::atoi(cur->m_pName + part_match_len) > 0)
					iface = reinterpret_cast<T*>(cur->m_CreateFn());
				
			}
			else
			{
				if (strcmp(cur->m_pName, ifaceName) == 0)
					iface = reinterpret_cast<T*>(cur->m_CreateFn());
			}
		}
		return iface;
	}
};
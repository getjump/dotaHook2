#ifndef I_SOURCE_CLIENT
#define I_SOURCE_CLIENT

#include "Hook.h"
#include "EntitySystem.hpp"
#include "FindPattern.h"

class ISourceClient
{
public:
	EntitySystem* GetEntitySystem()
	{
		typedef EntitySystem* (__fastcall * GetEntitySystemFn)(void*);

		auto ptr = FindPattern((DWORD_PTR)GetModuleHandleA("client.dll"), 0x7FFFFFFF, "\x48\x8B\x0D\x00\x00\x00\x00\x33\xFF\x8B\x81\x40\x20\x01\x00\xFF\xC0", "xxx????xxxxxxxxxx");

		DWORD_PTR dwGameEntitySystem = *(PDWORD_PTR)((ptr + 7) + *(PDWORD)(ptr + 3));

		return (EntitySystem*)dwGameEntitySystem;

		/*auto ptr = GetMethod<GetEntitySystemFn>(this, 121)(this);

		return (EntitySystem*)(ptr - 0x10);*/
	}
};

ISourceClient *sourceClient;

#endif
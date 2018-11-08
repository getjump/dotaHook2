#ifndef I_ENGINE_CLIENT
#define I_ENGINE_CLIENT

#include "Hook.h"

class IEngineClient
{
public:
	int GetLocalPlayer()
	{
		int output = 0;
		typedef int* (__thiscall* GetLocalPlayerFn)(IEngineClient*, int&, int);
		GetMethod<GetLocalPlayerFn>(this, 21)(this, output, 0);
		return output;
	}

	void DesktopNotify(const char* title, const char* message)
	{
		typedef void* (__thiscall* DesktopNotifyFn)(void*, const char*, const char*);
		GetMethod<DesktopNotifyFn>(this, 131)(this, title, message);
	}
};

IEngineClient *engineClient;

#endif
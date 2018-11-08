#include "Hook.h"
#include "FindPattern.h"

class DotaNpcWrapper
{
public:
	//372	__int64 __fastcall	C_DOTA_BaseNPC::IsIllusion(void)
	bool IsIllusion(void)
	{
		typedef bool (__thiscall* IsIllusionFn)(DotaNpcWrapper*);
		return GetMethod<IsIllusionFn>(this, 369)(this);
	}

	//304	__int64 __fastcall	C_DOTA_BaseNPC::GetPhysicalArmorValue(void)
	float GetPhysicalArmorValue(void)
	{
		typedef float (__thiscall* GetPhysicalArmorValueFn)(C_DOTA_BaseNPC*);
		return GetMethod<GetPhysicalArmorValueFn>(this, 301)((C_DOTA_BaseNPC*)this);
 	}

	//308	__int64 __fastcall	C_DOTA_BaseNPC::GetIncreasedAttackSpeed(void)
	float GetIncreasedAttackSpeed(void)
	{
		typedef float(__thiscall* GetIncreasedAttackSpeedFn)(C_DOTA_BaseNPC*);
		return GetMethod<GetIncreasedAttackSpeedFn>(this, 305)((C_DOTA_BaseNPC*)this);
	}

	bool CreateModifier(void)
	{
		auto entity = (C_DOTA_BaseNPC*)this;
		//__int64 __fastcall CModifieractoryDictionary::Create(CModifieractoryDictionary *__hidden this, const char *)	
		//CModifieractoryDictionary::FindFactory(v3, "modifier_rubick_telekinesis")
		//entity->m_ModifierManager.
	}

	void* FindModifierByName(const char* name)
	{
		typedef void*(__thiscall* FindModifierByName_fn)(C_DOTA_BaseNPC*, const char* name, void*);
		static DWORD_PTR g_dwClient = (DWORD_PTR)GetModuleHandleA("client.dll");
		static FindModifierByName_fn FindModifierByName = (FindModifierByName_fn)FindPattern(g_dwClient, 0x7FFFFFFF, "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20\x8B\x05\x00\x00\x00\x00\x49\x8B\xF8\x48\x8B\xDA\x48\x8B\xF1\xA8\x01\x75\x1A", "xxxx?xxxx?xxxxxxx????xxxxxxxxxxxxx");
		return FindModifierByName((C_DOTA_BaseNPC*)(this + 0xAE0), name, 0);
	}
};
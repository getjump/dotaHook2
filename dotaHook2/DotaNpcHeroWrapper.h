#include "Hook.h"

class DotaNpcHeroWrapper
{
public:
	//372	__int64 __fastcall	C_DOTA_BaseNPC::IsIllusion(void)
	bool IsIllusion(void)
	{
		typedef bool(__thiscall* IsIllusionFn)(DotaNpcHeroWrapper*);
		return GetMethod<IsIllusionFn>(this, 371)(this);
	}

	//304	__int64 __fastcall	C_DOTA_BaseNPC::GetPhysicalArmorValue(void)
	int GetPhysicalArmorValue(void)
	{
		typedef int(__thiscall* GetPhysicalArmorValueFn)(DotaNpcHeroWrapper*);
		return GetMethod<GetPhysicalArmorValueFn>(this, 302)(this);
	}

	//308	__int64 __fastcall	C_DOTA_BaseNPC::GetIncreasedAttackSpeed(void)
	int GetIncreasedAttackSpeed(void)
	{
		typedef int(__thiscall* GetIncreasedAttackSpeedFn)(DotaNpcHeroWrapper*);
		return GetMethod<GetIncreasedAttackSpeedFn>(this, 306)(this);
	}
};
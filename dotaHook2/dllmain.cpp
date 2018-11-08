// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"

#include "CHandle.h"

#include "EntitySystemHelper.h"
#include "EntitySystem.hpp"
#include "Interface.hpp"

#include "Hook.h"

#include "DotaNpcWrapper.h"
#include "DotaNpcHeroWrapper.h"

#include "IEngineClient.h"
#include "ISourceClient.h"

#include "FindPattern.h"
#include "intrin.h"

#include "Renderer.h"

#include "CUtlSymbolLarge.h"

#include "DotaRender.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <Psapi.h>
#include <fstream>
#include <ctime>
#include <set>
#include <map>
#include <chrono>

#include <assert.h>

#include <time.h>

using namespace std;

HMODULE module;

enum dotaunitorder_t {
	DOTA_UNIT_ORDER_NONE = 0,
	DOTA_UNIT_ORDER_MOVE_TO_POSITION = 1,
	DOTA_UNIT_ORDER_MOVE_TO_TARGET = 2,
	DOTA_UNIT_ORDER_ATTACK_MOVE = 3,
	DOTA_UNIT_ORDER_ATTACK_TARGET = 4,
	DOTA_UNIT_ORDER_CAST_POSITION = 5,
	DOTA_UNIT_ORDER_CAST_TARGET = 6,
	DOTA_UNIT_ORDER_CAST_TARGET_TREE = 7,
	DOTA_UNIT_ORDER_CAST_NO_TARGET = 8,
	DOTA_UNIT_ORDER_CAST_TOGGLE = 9,
	DOTA_UNIT_ORDER_HOLD_POSITION = 10,
	DOTA_UNIT_ORDER_TRAIN_ABILITY = 11,
	DOTA_UNIT_ORDER_DROP_ITEM = 12,
	DOTA_UNIT_ORDER_GIVE_ITEM = 13,
	DOTA_UNIT_ORDER_PICKUP_ITEM = 14,
	DOTA_UNIT_ORDER_PICKUP_RUNE = 15,
	DOTA_UNIT_ORDER_PURCHASE_ITEM = 16,
	DOTA_UNIT_ORDER_SELL_ITEM = 17,
	DOTA_UNIT_ORDER_DISASSEMBLE_ITEM = 18,
	DOTA_UNIT_ORDER_MOVE_ITEM = 19,
	DOTA_UNIT_ORDER_CAST_TOGGLE_AUTO = 20,
	DOTA_UNIT_ORDER_STOP = 21,
	DOTA_UNIT_ORDER_TAUNT = 22,
	DOTA_UNIT_ORDER_BUYBACK = 23,
	DOTA_UNIT_ORDER_GLYPH = 24,
	DOTA_UNIT_ORDER_EJECT_ITEM_FROM_STASH = 25,
	DOTA_UNIT_ORDER_CAST_RUNE = 26,
	DOTA_UNIT_ORDER_PING_ABILITY = 27,
	DOTA_UNIT_ORDER_MOVE_TO_DIRECTION = 28,
	DOTA_UNIT_ORDER_PATROL = 29,
};

typedef void* (*CreateInterfaceFn)(const char*, int*);

void* GetInterface(const char* pszModuleName, const char* pszInterfaceName)
{
	HMODULE hModule = GetModuleHandle(pszModuleName);

	if (!hModule)
		return nullptr;

	CreateInterfaceFn pCreateInterface = (CreateInterfaceFn)GetProcAddress(hModule, "CreateInterface");

	if (!pCreateInterface)
		return nullptr;

	void* pReturn = nullptr;

	char szBuffer[1024];

	pReturn = pCreateInterface(pszModuleName, nullptr);

	if (pReturn)
		return pReturn;

	for (int i = 0; i < 1000; ++i)
	{
		if (i > 99)
			sprintf(szBuffer, "%s%i", pszInterfaceName, i);
		else if (i > 9)
			sprintf(szBuffer, "%s0%i", pszInterfaceName, i);
		else
			sprintf(szBuffer, "%s00%i", pszInterfaceName, i);

		pReturn = pCreateInterface(szBuffer, nullptr);

		if (pReturn)
			break;
	}

	cout << szBuffer << endl;

	return pReturn;
}

typedef void(__fastcall* tPaintTraverse)(void*, void*, unsigned long long, bool, bool);
tPaintTraverse original;

const char* panel_GetName(void* ptr, unsigned long long vguiPanel)
{
	typedef const char* (__thiscall* OriginalFn)(void*, unsigned long long);
	return GetMethod<OriginalFn>(ptr, 50)(ptr, vguiPanel);
}

unsigned long long frames = 0;

typedef float(__fastcall* GetAttacksPerSecond_fn)(C_DOTA_BaseNPC*);
static GetAttacksPerSecond_fn GetAttacksPerSecond = nullptr;

// int,CEntityIndex,Vector,CEntityIndex,PlayerOrderIssuer_t,C_DOTA_BaseNPC *,OrderQueueBehavior_t,bool
typedef void(__fastcall* PrepareUnitOrdersFn_t)(C_DOTAPlayer*, dotaunitorder_t, int, Vector3<float> movePosition, int, PlayerOrderIssuer_t, C_DOTA_BaseNPC*, OrderQueueBehavior_t, bool);
static PrepareUnitOrdersFn_t prepareUnitOrders = nullptr;

typedef __int64*(__fastcall* PickupRuneFn)(C_DOTA_Item_Rune*, C_BasePlayer*);

std::map<C_DOTA_BaseNPC_Creep*, int> previous_healths;
std::map<C_DOTA_BaseNPC_Creep*, int> current_healths;

long long diff_health_access_count = 0;
std::map<C_DOTA_BaseNPC_Creep*, float> diff_health;
std::map<C_DOTA_BaseNPC_Creep*, int> diff_health_count;

C_DOTA_BaseNPC_Creep* autoTarget = nullptr;

std::chrono::milliseconds lasttime = std::chrono::duration_cast< std::chrono::milliseconds >(
	std::chrono::system_clock::now().time_since_epoch()
	);


void AutoLastHit()
{
	auto hero = entitySystemHelper.localHero;
	auto vecHero = hero->m_vecNetworkOrigin.Get<Vector3<float>>();
	auto attackRange = hero->m_iAttackRange;
	auto attacksPerSecond = GetAttacksPerSecond(hero);
	auto addAttackSpeed = hero->m_flBaseAttackTime;
	auto fullAttack = (hero->m_iDamageMin + hero->m_iDamageMax) * 0.5 + hero->m_iDamageBonus;



	float minimalDistance = -1;
	C_DOTA_BaseNPC_Creep* closest = nullptr;

	std::chrono::milliseconds cur = std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	);
	std::chrono::milliseconds diff = lasttime - cur;
	lasttime = cur;

	diff_health_access_count++;

	if (autoTarget != nullptr)
	{
		if (autoTarget->m_iHealth == 0)
		{
			autoTarget = nullptr;
		}
	}

	for (auto &&creep : entitySystemHelper.creeps)
	{
		current_healths.insert(make_pair(creep, creep->m_iHealth));

		auto creepHelper = (DotaNpcWrapper*)creep;

		auto vecCreep = creep->m_vecNetworkOrigin.Get<Vector3<float>>();
		if (creep->m_iHealth == 0)
			continue;

		auto it = previous_healths.find(creep);

		float dh = creep->m_iHealth;
		float t = 0.0;

		if (it != previous_healths.end())
		{
			dh = std::pow(it->second - dh, 2);
		}
		else {
			dh = 0;
		}

		auto it2 = diff_health.find(creep);

		float sup = 0;

		// (4 + 8) * 1/2 = 6, (6 + 4) * 1/2 = 5 (6 + 4 + 8) * 1/3 = 6

		if (it2 != diff_health.end())
		{
			diff_health[creep] += dh;
			diff_health_count[creep]++;
			sup = std::sqrt(diff_health[creep]) / diff_health_count[creep];
		}
		else {
			diff_health.insert(make_pair(creep, dh));
			diff_health_count.insert(make_pair(creep, 1));
		}

		float dist = vecHero.dist(vecCreep);

		// hp_diff * 1 / aps 

		if (dist < attackRange)
		{
			/*auto armor = creepHelper->GetPhysicalArmorValue();*/
			auto armor = creep->m_flPhysicalArmorValue;
			auto damageReduction = 0.06 * armor / ( 1 + 0.06 * armor);
			// 0-1 
			// 1000 ms = 1 s,  4 a/s, 12 m/s

			auto te = abs(diff.count());

			/*cout << "Attack = " << fullAttack * (1 - damageReduction) << endl;
			cout << "Cripp health = " << creep->m_iHealth << endl;
			cout << "Cripp will die in " << creep->m_iHealth / sup << " ticks " << endl;
			cout << "I will do hit in " << te * attacksPerSecond / 1000 * te * dist / attackRange << " ms" << endl;*/

			float k = dist / attackRange;

			if (k <= attackRange)
			{
				k = 1;
			}

			if (creep->m_iHealth - sup <= fullAttack * (1 - damageReduction)  || creep->m_iHealth / sup  < te * attacksPerSecond / 1000 * te * k) 
			{
				if (creep->m_iTeamNum = hero->m_iTeamNum && creep->m_iHealth > 0.5 * creep->m_iMaxHealth)
					continue;

				//if (dist < minimalDistance || closest == nullptr)
				//{
				//	/*if (autoTarget != nullptr)
				//	{
				//		if (autoTarget->m_iTeamNum == hero->m_iTeamNum && creep->m_iTeamNum != hero->m_iTeamNum)
				//		{
				//			autoTarget = creep;
				//			closest = creep;
				//			minimalDistance = dist;
				//		}
				//	}*/
				//	/*else {*/
						autoTarget = creep;
						closest = creep;
						minimalDistance = dist;
					/*}*/
				//}
			}
		}
	}
	static unsigned long long lastFrame = 0;
	if (closest != nullptr)
	{
		Vector3<float> creep = closest->m_vecNetworkOrigin.Get<Vector3<float>>();
		Vector3<float> creepScreen(0, 0, 0);

		if (WorldToScreenF(creep, &creepScreen) < 0)
		{
			auto heroScreen = WorldToScreenF(vecHero);
			g_pRenderer->Line(creepScreen.x, creepScreen.y, heroScreen.x, heroScreen.y, Color::Red());

			int index = 0;
			EntityGetIndex(closest, index);

			if (frames - lastFrame > 200)
			{
				lastFrame = frames;
				prepareUnitOrders(entitySystemHelper.localPlayer, dotaunitorder_t::DOTA_UNIT_ORDER_ATTACK_TARGET, index, creep, 0, PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_SELECTED_UNITS, 0, OrderQueueBehavior_t::DOTA_ORDER_QUEUE_DEFAULT, 0);
			}
			
			//prepareUnitOrders(entitySystemHelper.localPlayer, dotaunitorder_t::DOTA_UNIT_ORDER_NONE, 0, vecHero, 0, PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_SELECTED_UNITS, 0, OrderQueueBehavior_t::DOTA_ORDER_QUEUE_DEFAULT, 0);
		}
	}

	if (diff_health_access_count > 10000)
	{
		system("cls");
		diff_health_access_count = 0;
		diff_health.clear();
	}

	previous_healths = current_healths;
	current_healths.clear();
}

void StealRune(C_DOTA_Item_Rune* rune)
{
	auto vec1 = rune->m_vecNetworkOrigin.Get<Vector3<float>>();
	auto vec2 = entitySystemHelper.localHero->m_vecNetworkOrigin.Get<Vector3<float>>();

	PickupRuneFn pickupRune = nullptr;
	pickupRune = GetMethod<PickupRuneFn>(rune, 181);

	if (vec1.dist(vec2) < 1000)
	{
		if (rune->m_iRuneType <= 6)
		{
			static unsigned long long lastFrame = 0;

			int out = 0;

			

			EntityGetIndex(rune, out);

			auto runeScreen = WorldToScreenF(vec1);
			auto heroScreen = WorldToScreenF(vec2);		

			if(GetAsyncKeyState(0x05))
			{
				if (frames - lastFrame > 200)
				{
					lastFrame = frames;
					prepareUnitOrders(entitySystemHelper.localPlayer, dotaunitorder_t::DOTA_UNIT_ORDER_PICKUP_RUNE, out, vec1, 0, PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_HERO_ONLY, 0, OrderQueueBehavior_t::DOTA_ORDER_QUEUE_NEVER, 0);
				}

				g_pRenderer->Line(runeScreen.x, runeScreen.y, heroScreen.x, heroScreen.y, Color::Red());
			}
			else {
				g_pRenderer->Line(runeScreen.x, runeScreen.y, heroScreen.x, heroScreen.y, Color::Yellow());
			}
			
			//prepareUnitOrders(localPlayer, dotaunitorder_t::DOTA_UNIT_ORDER_MOVE_TO_POSITION, 0, vec1, 0, PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_SELECTED_UNITS, 0, OrderQueueBehavior_t::DOTA_ORDER_QUEUE_DEFAULT, 0);
		}
	}
}

std::map<C_BaseEntity*, const char*> visibleUnits;

typedef void*(__fastcall* OnNewParticleEffect_fn)(void*, char const*, CNewParticleEffect *);

set<const char*> panels;
void __fastcall Hooked_PaintTraverse(void* pPanel, void* edx, unsigned long long vguiPanel, bool forceRepaint, bool allowForce)
{
	assert(original != nullptr);

	original(pPanel, edx, vguiPanel, forceRepaint, allowForce);

	frames++;

	const char* pSzPanelName = panel_GetName(pPanel, vguiPanel);

	if (!pSzPanelName)
		return;

	/*if (panels.find(pSzPanelName) == panels.end())
		cout << pSzPanelName << endl;
	panels.insert(pSzPanelName);*/

	//RenderSystemTopPanel
	if (pSzPanelName[0] != 'R' || pSzPanelName[6] != 'S' || pSzPanelName[12] != 'T')
		return;

	/*m_HUDFont->String(50, 50, Color::White(), LEFT, "Test text");*/

	for (auto &&rune : entitySystemHelper.runes)
	{
		StealRune(rune);
	}

	for (auto &&v : entitySystemHelper.heroes)
	{
		auto vec = v->m_vecNetworkOrigin.Get<Vector3<float>>();

		Vector3<float> screenVec(0, 0, 0);

		/*if (WorldToScreenF(vec, &screenVec) && visibleUnits.find(v) == visibleUnits.end())
		{
			OnNewParticleEffect_fn newParticleEffect = GetMethod<OnNewParticleEffect_fn>(v, 63);
			std::string effect("particles/items_fx/aura_shivas.vpcf");
			newParticleEffect(v, effect.c_str(), new CNewParticleEffect);
			visibleUnits.insert(make_pair(v, effect.c_str()));
		}*/

		if (WorldToScreenF(vec, &screenVec) < 0) // on screen
		{
			Color color = Color::White(); 
			g_pRenderer->BorderBoxOut(screenVec.x, screenVec.y, 5, 5, 1, color, Color::Black());

			std::string buf = std::to_string(v->m_iUnitType);

			m_ESPFont->String(screenVec.x, screenVec.y, color, LEFT, buf.c_str());
		}
	}

	//AutoLastHit();
}

ISurface *surface;
IIPaintSurface *paintSurface;
CHookData* hook;

bool Hook_PaintTraverse()
{
	// w2s : 68A030
	const int vt_num = 55;

	void* g_dwVgui = (void*)GetModuleHandleA("vgui2.dll");

	void* g_pPanel = GetInterface("vgui2.dll", "VGUI_Panel");
	//auto entity = entitySystem->GetEntityByIndex(1);

	cout << "g_pPanel: 0x" << g_pPanel << endl;

	void* g_dwClient = (void*)GetModuleHandleA("client.dll");

	WorldToScreen = (WorldToScreenFn)FindPattern((DWORD_PTR)g_dwClient, 0x7FFFFFFF, "\x40\x53\x56\x57\x48\x83\xEC\x60\x49\x8B\xF8", "xxxxxxxxxxx");
	EntityGetIndex = (GetIndexFn)FindPattern((DWORD_PTR)g_dwClient, 0x7FFFFFFF, "\x40\x53\x48\x83\xEC\x20\x4C\x8B\x41\x10", "xxxxxxxxxx");
	GetAttacksPerSecond = (GetAttacksPerSecond_fn)FindPattern((DWORD_PTR)g_dwClient, 0x7FFFFFFF, "\x40\x53\x48\x83\xEC\x40\x48\x8B\x01\x48\x8B\xD9\x0F\x29\x74\x24\x00\x0F\x29\x7C\x24\x00\xFF\x90\x00\x00\x00\x00\x48\x8D\x8B\x00\x00\x00\x00", "xxxxxxxxxxxxxxxx?xxxx?xx????xxx????");

	if (g_pPanel)
	{
		surface = (ISurface*)GetInterface("vguirendersurface.dll", "VGUI_Surface");

		void* g_dwVguiRenderSurface = (void*)GetModuleHandleA("vguirendersurface.dll");

		DWORD_PTR base_ptr = FindPattern((DWORD_PTR)g_dwVguiRenderSurface, 0x7FFFFFFF, "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\xBA\x00\x00\x00\x00", "xxx????x????xx????x????");

		void* offset = (void*)(*(int*)(base_ptr + 3));

		// w0t h33k
		paintSurface = (IIPaintSurface*)((DWORD_PTR)offset + base_ptr + 7);

		m_ESPFont = g_pRenderer->CreateFont("Arial", 14, 450, FONTFLAG_OUTLINE);
		m_HUDFont = g_pRenderer->CreateFont("Helvitica", 14, 300, FONTFLAG_DROPSHADOW);

		prepareUnitOrders = GetMethod<PrepareUnitOrdersFn_t>(entitySystemHelper.localPlayer, 401);

		hook = new CHookData(g_pPanel);
		hook->HookMethod(&Hooked_PaintTraverse, vt_num);
		original = hook->GetMethod<tPaintTraverse>(vt_num);

		cout << (void*)original << endl;
	}

	return true;
}

typedef char(__fastcall* HasFlyingVisionFn)(void*);
char HasFlyingVision(void* ptr)
{
	return 1;
}

void StartupThread()
{
	FILE* fileConsole;
	AllocConsole();
	freopen_s(&fileConsole, "CONOUT$", "wb", stdout);

	void* g_dwClient = (void*)GetModuleHandleA("client.dll");

	auto ptr = FindPattern((DWORD_PTR)g_dwClient, 0x7FFFFFFF, "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\x1D\x00\x00\x00\x00\x48\x8B\xF9\x48\x85\xDB\x0F\x84\x00\x00\x00\x00\x48\x8B\x03\x48\x8B\xCB\xFF\x90\x00\x00\x00\x00\x84\xC0\x74\x64", "xxxx?xxxxxxxx????xxxxxxxx????xxxxxxxx????xxxx");
	const char* patch = "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x48\x8B\x1D\xE7\xCC\x4E";
	memcpy((void*)ptr, (void*)patch, strlen(patch));

	return;

	void* g_pSourceClient = GetInterface("client.dll", "Source2Client");

	cout << g_pSourceClient << endl;

	void* g_pEngineClient = GetInterface("engine2.dll", "Source2EngineToClient");

	engineClient = (IEngineClient*)g_pEngineClient;
	sourceClient = (ISourceClient*)g_pSourceClient;

	entitySystemHelper.Init();
	Hook_PaintTraverse();

	CHookData* playerHook = new CHookData(entitySystemHelper.localHero);

	while (true)
	{
		if (GetAsyncKeyState(VK_F10))
		{
			auto localWrap = (DotaNpcWrapper*)entitySystemHelper.localHero;
			std::cout << localWrap->FindModifierByName("modifier_sven_gods_strength") << std::endl;
			Sleep(300);
			//cout << entities.size() << endl;
			//cout << npcs.size() << endl;
			/*cout << "Unloaded " << endl;
			Sleep(1000);*/
		}

		if (GetAsyncKeyState(VK_F9))
		{
			playerHook->HookMethod(&HasFlyingVision, 361);
			Sleep(1000);
		}

		if (GetAsyncKeyState(VK_F11))
		{
			cout << "See ya" << endl;
			if (hook->Hooked())
			{
				hook->Unhook();
				hook->Poof();
			}

			if (playerHook->Hooked())
			{
				playerHook->Unhook();
				playerHook->Poof();
			}
			hook = 0;
			playerHook = 0;
			FreeLibraryAndExitThread(module, 0);
			return;
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	module = hModule;
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartupThread, NULL, 0, NULL);
	}

	return TRUE;
}


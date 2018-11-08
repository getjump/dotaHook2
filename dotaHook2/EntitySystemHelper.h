#include "EntitySystem.hpp"
#include "IEngineClient.h"
#include "ISourceClient.h"
#include "CHandle.h"

#include <iostream>

typedef int(__fastcall* OnAddOrRemove_fn)(void*, CEntityInstance*, CHandle_s<int>*);

OnAddOrRemove_fn OnAddEntity_original;
OnAddOrRemove_fn OnRemoveEntity_original;

int OnAddEntity(void* ptr, CEntityInstance* entityInstance, CHandle_s<int>* handle);
int OnRemoveEntity(void* ptr, CEntityInstance* entityInstance, CHandle_s<int>* handle);

class EntitySystemHelper
{
public:
	std::vector<C_DOTA_BaseNPC*> npcs;
	std::vector<C_DOTA_BaseNPC_Hero*> heroes;
	std::vector<C_DOTA_BaseNPC_Creep*> creeps;
	std::vector<CDOTA_NPC_Observer_Ward*> wards;
	std::vector<C_DOTA_Item_Rune*> runes;

	C_DOTA_BaseNPC_Hero* localHero;
	C_DOTAPlayer* localPlayer;

	CHookData* hook;
	EntitySystem* entitySystem;

	typedef __int64(__fastcall* OnAddEntity_fn)(void*, CEntityInstance*, DWORD*);
	typedef __int64(__fastcall* OnRemoveEntity_fn)(void*, CEntityInstance*, DWORD*);

	~EntitySystemHelper()
	{
		if (hook->Hooked())
		{
			hook->Unhook();
			hook->Poof();
		}

		hook = nullptr;
	}

	void EntityCreate(C_BaseEntity* entity)
	{
		if (entity == nullptr)
			return;

		auto dynamicBinding = entity->Schema_DynamicBinding();

		if (!dynamicBinding->m_classInfo)
			return;

		if (!dynamicBinding->m_classInfo->m_Name.data)
			return;

		std::cout << "Entity " << dynamicBinding->m_classInfo->m_Name.data << " added" << std::endl;

		typedef schema::CSchemaClassBinding* (*dynamicBindingFn)(void);
		if (entity->m_bIsDOTANPC)
		{
			auto npc = (C_DOTA_BaseNPC*)entity;
			npcs.push_back(npc);


			// DOESNT WORK HERE
			/*UNIT_TYPES unitType = (UNIT_TYPES)npc->m_iUnitType;

			switch (unitType)
			{
			case UNIT_TYPE_HERO:
				heroes.push_back((C_DOTA_BaseNPC_Hero*)npc);
				break;
			case UNIT_TYPE_CREEP:
				creeps.push_back((C_DOTA_BaseNPC_Creep*)npc);
				break;
			case UNIT_TYPE_WARD:
				wards.push_back((CDOTA_NPC_Observer_Ward*)npc);
				break;
			}*/

			if (dynamicBinding->m_classInfo->InheritsFrom("C_DOTA_BaseNPC_Creep"))
			{
				creeps.push_back((C_DOTA_BaseNPC_Creep*)npc);
			}
			else if (dynamicBinding->m_classInfo->InheritsFrom("C_DOTA_BaseNPC_Hero")) {
				heroes.push_back((C_DOTA_BaseNPC_Hero*)npc);
			}
		}
		else {
			if (!strcmp(dynamicBinding->m_classInfo->m_Name.data, "C_DOTA_Item_Rune"))
				runes.push_back((C_DOTA_Item_Rune*)entity);
		}

	}

	void EntityDestroy(C_BaseEntity* entity)
	{
		if (entity == nullptr)
			return;

		typedef schema::CSchemaClassBinding* (*dynamicBindingFn)(void);
		if (entity->m_bIsDOTANPC)
		{
			auto npc = (C_DOTA_BaseNPC*)entity;
			npcs.erase(std::remove(npcs.begin(), npcs.end(), npc), npcs.end());

			UNIT_TYPES unitType = (UNIT_TYPES)npc->m_iUnitType;

			switch (unitType)
			{
			case UNIT_TYPE_HERO:
				heroes.erase(std::remove(heroes.begin(), heroes.end(), (C_DOTA_BaseNPC_Hero*)npc), heroes.end());
				break;
			case UNIT_TYPE_CREEP:
				creeps.erase(std::remove(creeps.begin(), creeps.end(), (C_DOTA_BaseNPC_Creep*)npc), creeps.end());
				break;
			case UNIT_TYPE_WARD:
				wards.erase(std::remove(wards.begin(), wards.end(), (CDOTA_NPC_Observer_Ward*)npc), wards.end());
				break;
			}
		}
		else {
			auto schema = (SchemaBase*)entity;

			auto dynamicBinding = schema->Schema_DynamicBinding();

			if (!dynamicBinding->m_classInfo)
				return;

			if (!dynamicBinding->m_classInfo->m_Name.data)
				return;

			if (!strcmp(dynamicBinding->m_classInfo->m_Name.data, "C_DOTA_Item_Rune"))
				runes.erase(std::remove(runes.begin(), runes.end(), (C_DOTA_Item_Rune*)entity), runes.end());
		}
	}

	void Init()
	{
		entitySystem = sourceClient->GetEntitySystem();

		std::cout << "entity System " << (void*)entitySystem << std::endl;

		hook = new CHookData(entitySystem);

		hook->HookMethod(&OnAddEntity, 14);
		OnAddEntity_original = hook->GetMethod<OnAddOrRemove_fn>(14);

		hook->HookMethod(&OnRemoveEntity, 15);
		OnRemoveEntity_original = hook->GetMethod<OnAddOrRemove_fn>(15);

		localPlayer = (C_DOTAPlayer*)entitySystem->GetEntityByIndex(engineClient->GetLocalPlayer());
		localHero = (C_DOTA_BaseNPC_Hero*)entitySystem->GetEntityByIndex(localPlayer->m_hAssignedHero.Get<CHandle_s<C_BaseEntity>>().GetEntryIndex());

		for (int i = 0; i < MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS; i++)
		{
			EntityCreate(entitySystem->GetEntityByIndex(i));
		}

		std::cout << "npcs count = " << npcs.size() << std::endl;
	}

} entitySystemHelper;

int OnAddEntity(void* ptr, CEntityInstance* entityInstance, CHandle_s<int>* handle)
{
	auto result = OnAddEntity_original(ptr, entityInstance, handle);

	if (!entityInstance->m_pEntity)
		return result;

	auto entity = *(C_BaseEntity**)entityInstance->m_pEntity;

	if (!entity)
		return result;

	entitySystemHelper.EntityCreate(entity);

	return result;
}

int OnRemoveEntity(void* ptr, CEntityInstance* entityInstance, CHandle_s<int>* handle)
{
	auto result = OnRemoveEntity_original(ptr, entityInstance, handle);

	if (!entityInstance->m_pEntity)
		return result;

	auto entity = *(C_BaseEntity**)entityInstance->m_pEntity;

	if (!entity)
		return result;

	entitySystemHelper.EntityDestroy(entity);

	return result;
}
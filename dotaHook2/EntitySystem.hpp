#pragma once 

#include "!GlobalTypes_classes.hpp"
#include "client.dll_classes.hpp"
#include "Hook.h"

#include <algorithm>
#include <vector>

#define MAX_ENTITIES_IN_LIST 512 
#define MAX_ENTITY_LISTS 64 
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS 

enum UNIT_TYPES : unsigned long
{
	UNIT_TYPE_WORLD = 0,
	UNIT_TYPE_HERO = 1,
	UNIT_TYPE_FOUNTAIN = 16,
	UNIT_TYPE_TOWER = 20,
	UNIT_TYPE_COURIER = 256,
	UNIT_TYPE_CREEP = 1152,
	UNIT_TYPE_ROSHAN = 2176,
	UNIT_TYPE_WARD = 131072,
};

class CEntityIdentities
{
public:
	CEntityIdentity m_identities[MAX_ENTITIES_IN_LIST];
};

class EntityIdentityList
{
public:
	CEntityIdentities* m_identityList;
};

class EntitySystem
{
public:
	C_BaseEntity* GetEntityByIndex(int index);
	C_BaseEntity* GetEntityByHandle(unsigned int handle);

private:
	void* unknown;
	void* unknown2;
	EntityIdentityList m_entityList[MAX_ENTITY_LISTS];
};

typedef unsigned long long(__fastcall* GetIndexFn)(void*, int&);
static GetIndexFn EntityGetIndex = nullptr;

class EntityIdentityWrapper
{
public:
	int GetIndex()
	{
		int out = 0;
		return EntityGetIndex(this, out);
	};
};
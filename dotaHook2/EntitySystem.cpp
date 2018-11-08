#include "EntitySystem.hpp" 

// #include "EntityIdentityWrapper.hpp" 

C_BaseEntity* EntitySystem::GetEntityByIndex(int index)
{
	if (index >= MAX_TOTAL_ENTITIES)
		return nullptr;

	if ((index >> 9) > 0x3F)
		return nullptr;

	// this is really weird. 
	CEntityIdentities* entityIdentities = ((EntityIdentityList*)&m_entityList[(index >> 9) + 1])->m_identityList;

	if (!entityIdentities)
		return nullptr;
	
	auto identity = entityIdentities->m_identities[index & (MAX_ENTITIES_IN_LIST - 1)];
	
	EntityIdentityWrapper* entityIdentity = (EntityIdentityWrapper*)&identity;

	//auto idx = entityIdentity->GetIndex();

	//EntityIdentityWrapper* entityIdentity = (EntityIdentityWrapper*)&entityIdentities->m_identities[index & (MAX_ENTITIES_IN_LIST - 1)];

	//if (entityIdentity->GetIndex() != index)
	//	return nullptr;
	return (&identity != nullptr) ? *(C_BaseEntity**)&identity : nullptr;
}

C_BaseEntity* EntitySystem::GetEntityByHandle(unsigned int handle)
{
	return GetEntityByIndex(handle & 0x7FFF);
}
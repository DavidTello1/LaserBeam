#include "dvs_pch.h"
#include "EntityManager.h"

namespace Davos {

	EntityManager::EntityManager()
	{
		for (Entity i = 0; i < MAX_ENTITIES; ++i)
			m_AvailableIndices.push(i);
	}

	EntityManager::~EntityManager()
	{
	}

	Entity EntityManager::CreateEntity(Entity id)
	{
		DVS_CORE_ASSERT(m_NumEntities < MAX_ENTITIES, "Reached limit number of entities");

		Entity index = m_AvailableIndices.front();
		m_AvailableIndices.pop();
		++m_NumEntities;
		return index;
	}

	Entity EntityManager::DuplicateEntity(Entity id)
	{
		Entity entity = CreateEntity();
		
		for (auto& [type, pool] : m_ComponentPools)
		{
			if (!pool->Has(id))
				continue;

			pool->Duplicate(id, entity);
		}

		//m_EntityMasks[entity] = m_EntityMasks[id];
		return entity;
	}

	void EntityManager::DestroyEntity(Entity id)
	{
		DVS_CORE_ASSERT(Exists(id), "Entity does not exist");

		for (auto& [type, pool] : m_ComponentPools)
		{
			if (!pool->Has(id))
				continue;

			pool->Remove(id);
		}

		//m_EntityMasks.erase(id);
	}

	bool EntityManager::Exists(Entity id) const
	{
		return id < m_NumEntities; //*** invalid when removing and recycling entities
	}

}

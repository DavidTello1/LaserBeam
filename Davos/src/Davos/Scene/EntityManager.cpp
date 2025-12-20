#include "dvs_pch.h"
#include "EntityManager.h"

namespace Davos {

	EntityManager::EntityManager()
	{
		m_Generations.fill(0);

		for (uint32_t i = 0; i < MAX_ENTITIES; ++i)
			m_AvailableIndices.push(i);
	}

	EntityManager::~EntityManager()
	{
	}

	Entity EntityManager::CreateEntity()
	{
		DVS_CORE_ASSERT(m_NumEntities < MAX_ENTITIES, "Reached limit number of entities");
		DVS_CORE_ASSERT(!m_AvailableIndices.empty(), "No available entity indices");

		uint32_t index = m_AvailableIndices.front();
		m_AvailableIndices.pop();

		DVS_CORE_ASSERT(index < m_Generations.size(), "Entity index out of bounds");

		Entity entity;
		entity.index = index;
		entity.generation = m_Generations[index];
		++m_NumEntities;

		return entity;
	}

	//Entity EntityManager::DuplicateEntity(Entity entity)
	//{
	//	Entity newEntity = CreateEntity();
	//	
	//	for (auto& [type, pool] : m_ComponentPools)
	//	{
	//		if (!pool->Has(entity.index))
	//			continue;

	//		pool->Duplicate(newEntity.index, entity.index);
	//	}

	//	return newEntity;
	//}

	void EntityManager::DestroyEntity(Entity entity)
	{
		DVS_CORE_ASSERT(Exists(entity), "Entity does not exist");
		DVS_CORE_ASSERT(entity.index < m_Generations.size(), "Entity index out of bounds");

		for (auto& [type, pool] : m_ComponentPools)
		{
			if (!pool->Has(entity.index))
				continue;

			pool->Remove(entity.index);
		}

		m_AvailableIndices.push(entity.index);
		m_Generations[entity.index]++;

		m_NumEntities--;
	}

	bool EntityManager::Exists(Entity entity) const
	{
		if (entity.index >= m_Generations.size())
			return false;

		return entity.generation == m_Generations[entity.index];
	}

}

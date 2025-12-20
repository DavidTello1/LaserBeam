#pragma once

namespace Davos {

	constexpr uint32_t MAX_ENTITIES = 10000;

	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;

		virtual bool Has(uint32_t entity) const = 0;
		virtual void Remove(uint32_t entity) = 0;
		//virtual void Duplicate(uint32_t entity_src, uint32_t entity_dst) = 0;
		virtual uint32_t GetEntityAt(uint32_t index) const = 0;
		virtual uint32_t GetIndexOf(uint32_t entity) const = 0;
		virtual uint32_t size() const = 0;
	};

	template <typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		ComponentPool() : m_Count(0) {
			m_Sparse.fill(MAX_ENTITIES);
			m_Dense.fill(MAX_ENTITIES);
		}

		~ComponentPool() = default;

		// ---
		bool Has(uint32_t entity) const override {
			DVS_CORE_ASSERT(entity < MAX_ENTITIES, "Entity ID is out of bounds");

			return m_Sparse[entity] < m_Count && m_Dense[m_Sparse[entity]] == entity;
		}

		T& Add(uint32_t entity, const T& component) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");

			m_Sparse[entity] = m_Count;
			m_Dense[m_Count] = entity;
			m_Components[m_Count] = component;
			m_Count++;

			return m_Components[m_Count - 1];
		}

		template <typename... Args>
		T& Add(uint32_t entity, Args&&... args) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");

			m_Sparse[entity] = m_Count;
			m_Dense[m_Count] = entity;
			m_Components[m_Count] = T(std::forward<Args>(args)...);
			m_Count++;

			return m_Components[m_Count - 1];
		}

		T& Replace(uint32_t entity, const T& component) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = m_Sparse[entity];
			m_Components[index] = component;

			return m_Components[index];
		}

		template <typename... Args>
		T& Replace(uint32_t entity, Args&&... args) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = m_Sparse[entity];
			m_Components[index] = T(std::forward<Args>(args)...);

			return m_Components[index];
		}

		T& AddOrReplace(uint32_t entity, const T& component) {
			if (Has(entity))
				return Replace(entity, component);

			return Add(entity, component);
		}

		template <typename... Args>
		T& AddOrReplace(uint32_t entity, Args&&... args) {
			if (Has(entity))
				return Replace(entity, std::forward<Args>(args)...);

			return Add(entity, std::forward<Args>(args)...);
		}

		T& InsertAfter(uint32_t entity, uint32_t index, const T& component) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");
			DVS_CORE_ASSERT(index <= m_Count, "Index is out of bounds");

			if (index == m_Count)
				return Add(entity, component);

			for (uint32_t i = m_Count; i > index; --i)
			{
				m_Sparse[m_Dense[i - 1]] = i;
				m_Dense[i] = m_Dense[i - 1];
				m_Components[i] = m_Components[i - 1];
			}

			m_Sparse[entity] = index;
			m_Dense[index] = entity;
			m_Components[index] = component;
			m_Count++;

			return m_Components[index];
		}

		template <typename... Args>
		T& InsertAfter(uint32_t entity, uint32_t index, Args&&... args) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");
			DVS_CORE_ASSERT(index < m_Count, "Index is out of bounds");

			if (index == m_Count)
				return Add(entity, std::forward<Args>(args)...);

			for (uint32_t i = m_Count; i > index; --i)
			{
				m_Sparse[m_Dense[i - 1]] = i;
				m_Dense[i] = m_Dense[i - 1];
				m_Components[i] = m_Components[i - 1];
			}

			m_Sparse[entity] = index;
			m_Dense[index] = entity;
			m_Components[index] = T(std::forward<Args>(args)...);
			m_Count++;

			return m_Components[index];
		}

		//void Duplicate(uint32_t entity_src, uint32_t entity_dst) override {
		//	DVS_CORE_ASSERT(Has(entity_src), "Source Entity ID not found");

		//	uint32_t index = m_Sparse[entity_src];
		//	Add(entity_dst, m_Components[index]);
		//}

		void Remove(uint32_t entity) override {
			DVS_CORE_ASSERT(Has(entity), "Entity ID not found");

			uint32_t index = m_Sparse[entity];
			uint32_t lastIndex = m_Dense[m_Count - 1];

			m_Sparse[lastIndex] = index;
			m_Dense[index] = lastIndex;
			m_Components[index] = m_Components[m_Count - 1];

			--m_Count;
			m_Sparse[entity] = MAX_ENTITIES;
		}

		void RemoveOrdered(uint32_t entity, uint32_t childCount = 0) {
			DVS_CORE_ASSERT(Has(entity), "Entity ID not found");

			uint32_t index = m_Sparse[entity];
			uint32_t lastChildIndex = index + childCount;

			DVS_CORE_ASSERT(lastChildIndex < m_Count, "Entity count to remove is out of bounds");

			for (uint32_t i = 0; i < childCount + 1; ++i)
				m_Sparse[m_Dense[index + i]] = MAX_ENTITIES;

			uint32_t cursor = index;
			for (uint32_t i = lastChildIndex + 1; i < m_Count; ++i)
			{
				m_Sparse[m_Dense[i]] = cursor;
				
				m_Dense[cursor] = m_Dense[i];
				m_Components[cursor] = m_Components[i];

				cursor++;
			}

			for (uint32_t i = 0; i < childCount + 1; ++i)
				m_Dense[m_Count - 1 - i] = MAX_ENTITIES;

			m_Count -= childCount + 1;

			//// ------------------------
			//DVS_CORE_ASSERT(Has(entity), "Entity ID not found");

			//uint32_t index = m_Sparse[entity];
			//uint32_t lastIndex = index + count; // inclusive last index to remove

			//DVS_CORE_ASSERT(lastIndex < m_Count, "Removing beyond array bounds");

			//for (uint32_t i = 0; i < count + 1; ++i)
			//{
			//	uint32_t entityToRemove = m_Dense[index + i];
			//	m_Sparse[entityToRemove] = MAX_ENTITIES;
			//}

			//uint32_t elementsToShift = m_Count - (lastIndex + 1);
			//for (uint32_t i = 0; i < elementsToShift; ++i) 
			//{
			//	uint32_t srcIndex = lastIndex + 1 + i;
			//	uint32_t dstIndex = index + i;

			//	uint32_t movedEntity = m_Dense[srcIndex];

			//	m_Sparse[movedEntity] = dstIndex;
			//	m_Dense[dstIndex] = movedEntity;
			//	m_Components[dstIndex] = m_Components[srcIndex];
			//}

			//uint32_t newCount = m_Count - (count + 1);
			//for (uint32_t i = newCount; i < m_Count; ++i) 
			//{
			//	m_Dense[i] = MAX_ENTITIES;
			//	m_Components[i] = T();
			//}

			//m_Count = newCount;
		}

		T& Get(uint32_t entity) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = m_Sparse[entity];
			return m_Components[index];
		}

		const T& Get(uint32_t entity) const {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = m_Sparse[entity];
			return m_Components[index];
		}

		T* TryGet(uint32_t entity) {
			if (!Has(entity))
				return nullptr;

			uint32_t index = m_Sparse[entity];
			return &m_Components[index];
		}

		const T* TryGet(uint32_t entity) const {
			if (!Has(entity))
				return nullptr;

			uint32_t index = m_Sparse[entity];
			return &m_Components[index];
		}

		uint32_t GetEntityAt(uint32_t index) const override {
			DVS_CORE_ASSERT(index < m_Count, "Index is out of bounds");
			return m_Dense[index];
		}

		uint32_t GetIndexOf(uint32_t entity) const override {
			DVS_CORE_ASSERT(Has(entity), "Entity not found");
			return m_Sparse[entity];
		}

		uint32_t size() const override { 
			return m_Count;
		}

	private:
		std::array<uint32_t, MAX_ENTITIES> m_Dense;
		std::array<uint32_t, MAX_ENTITIES> m_Sparse;

		std::array<T, MAX_ENTITIES> m_Components; // actual data, order is the same as m_Dense
		uint32_t m_Count = 0;
	};

}

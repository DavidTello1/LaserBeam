#pragma once
#include "ComponentPool.h"

#include <typeindex>
#include <tuple>
#include <queue>

namespace Davos {

	using Entity = uint32_t;

	template <typename... T>
	class EntityView
	{
	public:
		std::vector<Entity> m_Entities;
		std::tuple<ComponentPool<T>*...> m_Pools;

	public:
		class Iterator
		{
		public:
			Iterator(std::vector<Entity>::iterator it, std::tuple<ComponentPool<T>*...>& pools)
				: m_It(it), m_Pools(pools) {}

			std::tuple<Entity, T&...> operator*() const
			{
				return std::forward_as_tuple(*m_It, std::get<ComponentPool<T>*>(m_Pools)->Get(*m_It)...);
			}

			Iterator& operator++() { ++m_It; return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

			bool operator==(const Iterator& other) const { return m_It == other.m_It; }
			bool operator!=(const Iterator& other) const { return !(*this == other); }

		private:
			std::vector<Entity>::iterator m_It;
			std::tuple<ComponentPool<T>*...>& m_Pools;
		};

		class ConstIterator
		{
		public:
			ConstIterator(std::vector<Entity>::const_iterator it, const std::tuple<ComponentPool<T>*...>& pools)
				: m_It(it), m_Pools(pools) {}

			std::tuple<Entity, const T&...> operator*() const
			{
				return std::forward_as_tuple(*m_It, std::get<ComponentPool<T>*>(m_Pools)->Get(*m_It)...);
			}

			ConstIterator& operator++() { ++m_It; return *this; }
			ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

			bool operator==(const ConstIterator& other) const { return m_It == other.m_It; }
			bool operator!=(const ConstIterator& other) const { return !(*this == other); }

		private:
			std::vector<Entity>::const_iterator m_It;
			const std::tuple<ComponentPool<T>*...>& m_Pools;
		};

	public:
		EntityView(std::vector<Entity> entities, std::tuple<ComponentPool<T>*...> pools)
			: m_Entities(std::move(entities)), m_Pools(std::move(pools)) {}

		~EntityView() = default;

		// ---
		Iterator begin() { return Iterator(m_Entities.begin(), m_Pools); }
		Iterator end() { return Iterator(m_Entities.end(), m_Pools); }

		ConstIterator begin() const { return ConstIterator(m_Entities.begin(), m_Pools); }
		ConstIterator end() const { return ConstIterator(m_Entities.end(), m_Pools); }

		size_t size() const { return m_Entities.size(); }
		bool empty() const { return m_Entities.empty(); }
	};

	// --------------------------------------------------
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		// --- ENTITIES ---
		Entity CreateEntity(Entity id = 0);
		//Entity CreatePrefab(Prefab::Type type, Entity id = 0);
		Entity DuplicateEntity(Entity id);
		void DestroyEntity(Entity id);
		bool Exists(Entity id) const;

		// --- COMPONENTS ---
		template <typename T>
		bool HasComponent(Entity id) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			return (pool && pool->Has(id));
		}

		template <typename... T>
		bool HasComponents(Entity id) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			return (HasComponent<T>(id) && ...);
		}

		template <typename T>
		T& AddComponent(Entity id, const T& component) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				pool = _CreatePool<T>();

			return pool->Add(id, component);
		}

		template <typename T, typename... Args>
		T& AddComponent(Entity id, Args&&... args) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				pool = _CreatePool<T>();

			return pool->Add(id, std::forward<Args>(args)...);
		}
		
		template <typename T>
		T& ReplaceComponent(Entity id, const T& component) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Entity does not have this component");

			return pool->Replace(id, component);
		}

		template <typename T, typename... Args>
		T& ReplaceComponent(Entity id, Args&&... args) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Entity does not have this component");

			return pool->Replace(id, std::forward<Args>(args)...);
		}

		template <typename T>
		T& AddOrReplaceComponent(Entity id, const T& component) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				pool = _CreatePool<T>();

			return pool->AddOrReplace(id, component);
		}

		template <typename T, typename... Args>
		T& AddOrReplaceComponent(Entity id, Args&&... args) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				pool = _CreatePool<T>();

			return pool->AddOrReplace(id, std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent(Entity id) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			pool->Remove(id);
		}

		template <typename... T>
		void RemoveComponents(Entity id) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			(RemoveComponent<T>(id), ...);
		}

		template <typename T>
		T& GetComponent(Entity id) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			return pool->Get(id);
		}

		template <typename T>
		const T& GetComponent(Entity id) const {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			return pool->Get(id);
		}

		template <typename T>
		EntityView<T> GetView() {
			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool != nullptr, "Error Component Pool not found");

			std::vector<Entity> entities;
			entities.reserve(pool->size());

			for (uint32_t i = 0, size = pool->size(); i < size; ++i)
			{
				Entity entity = pool->GetEntityAt(i);
				entities.push_back(entity);
			}

			return EntityView<T>(std::move(entities), std::make_tuple(pool));
		}

		template <typename T>
		const EntityView<T> GetView() const {
			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool != nullptr, "Error Component Pool not found");

			std::vector<Entity> entities;
			entities.reserve(pool->size());

			for (uint32_t i = 0, size = pool->size(); i < size; ++i)
			{
				Entity entity = pool->GetEntityAt(i);
				entities.push_back(entity);
			}

			return EntityView<T>(std::move(entities), std::make_tuple(pool));
		}

		template <typename... T>
		EntityView<T...> GetView() {
			auto pools = std::make_tuple(_GetPool<T>()...);
			DVS_CORE_ASSERT(((std::get<ComponentPool<T>*>(pools) != nullptr) && ...), "One or more Component Pools not found");

			IComponentPool* smallestPool = _FindSmallestPool(std::get<ComponentPool<T>*>(pools)...);

			std::vector<Entity> entities;
			entities.reserve(smallestPool->size());

			for (uint32_t i = 0, size = smallestPool->size(); i < size; ++i)
			{
				Entity entity = smallestPool->GetEntityAt(i);

				if ((std::get<ComponentPool<T>*>(pools)->Has(entity) && ...))
					entities.push_back(entity);
			}

			return EntityView<T...>(std::move(entities), pools);
		}

		template <typename... T>
		const EntityView<T...> GetView() const {
			auto pools = std::make_tuple(_GetPool<T>()...);
			DVS_CORE_ASSERT(((std::get<ComponentPool<T>*>(pools) != nullptr) && ...), "One or more Component Pools not found");

			IComponentPool* smallestPool = _FindSmallestPool(std::get<ComponentPool<T>*>(pools)...);

			std::vector<Entity> entities;
			entities.reserve(smallestPool->size());

			for (uint32_t i = 0, size = smallestPool->size(); i < size; ++i)
			{
				Entity entity = smallestPool->GetEntityAt(i);

				if ((std::get<ComponentPool<T>*>(pools)->Has(entity) && ...))
					entities.push_back(entity);
			}

			return EntityView<T...>(std::move(entities), pools);
		}

	private:
		template <typename T>
		inline ComponentPool<T>* _CreatePool() {
			auto typeID = std::type_index(typeid(T));
			auto it = m_ComponentPools.find(typeID);

			DVS_CORE_ASSERT(it == m_ComponentPools.end(), "ComponentPool of this type already exists");

			Scope<ComponentPool<T>> pool = CreateScope<ComponentPool<T>>(/*pool size*/);
			auto ptr = pool.get();

			m_ComponentPools[typeID] = std::move(pool);

			return ptr;
		}

		template <typename T>
		inline ComponentPool<T>* _GetPool() {
			auto typeID = std::type_index(typeid(T));
			auto it = m_ComponentPools.find(typeID);

			if (it == m_ComponentPools.end())
				return nullptr;

			return static_cast<ComponentPool<T>*>(it->second.get());
		}

		template <typename T>
		inline ComponentPool<T>* _GetOrCreatePool() {
			if (auto* pool = _GetPool<T>())
				return pool;
			return _CreatePool<T>();
		}

		template <typename... P>
		static IComponentPool* _FindSmallestPool(P*... pools) {
			IComponentPool* result = nullptr;
			size_t smallestSize = MAX_ENTITIES;

			auto check = [&](auto* pool) {
				if (pool && pool->size() < smallestSize)
				{
					result = pool;
					smallestSize = pool->size();
				}
			};

			(check(pools), ...);
			return result;
		}

	private:
		std::queue<Entity> m_AvailableIndices;
		uint32_t m_NumEntities = 0;

		//std::array<ComponentMask, MAX_ENTITIES> m_EntityMasks;		
		std::unordered_map<std::type_index, Scope<IComponentPool>> m_ComponentPools;
	};

}

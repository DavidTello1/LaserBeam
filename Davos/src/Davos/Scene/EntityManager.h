#pragma once
#include "ComponentPool.h"

#include <typeindex>
#include <tuple>
#include <queue>

namespace Davos {


	struct Entity
	{
		uint32_t index : 24;
		uint32_t generation : 8;

		constexpr Entity() : index(0), generation(0) {}
		constexpr explicit Entity(uint32_t entity)
			: index(entity & 0xFFFFFF), generation((entity >> 24) & 0xFF) {}

		static const Entity null;

		constexpr bool isNull() const {
			return *this == null;
		}

		constexpr operator uint32_t() const {
			return (generation << 24) | index;
		}

		constexpr bool operator==(const Entity& other) const {
			return static_cast<uint32_t>(*this) == static_cast<uint32_t>(other);
		}
		constexpr bool operator!=(const Entity& other) const {
			return !(*this == other);
		}

		constexpr bool operator==(uint32_t other) const {
			return static_cast<uint32_t>(*this) == other;
		}
		constexpr bool operator!=(uint32_t other) const {
			return !(*this == other);
		}
	};

	inline constexpr Entity Entity::null = Entity(0xFFFFFFFF);

	// --------------------------------------------------
	template <typename... T>
	class EntityView
	{
	private:
		std::tuple<ComponentPool<T>*...> m_Pools;
		IComponentPool* m_Smallest;

	public:
		EntityView()
			: m_Pools(std::make_tuple((ComponentPool<T>*)nullptr ...)),
			m_Smallest(nullptr)
		{}

		EntityView(std::tuple<ComponentPool<T>*...> pools)
			: m_Pools(pools),
			m_Smallest(_FindSmallestPool(pools))
		{}

		bool empty() const {
			return m_Smallest == nullptr || m_Smallest->size() == 0;
		}

		size_t size() const {
			return m_Smallest ? m_Smallest->size() : 0;
		}

		class Iterator
		{
		private:
			uint32_t m_Index;
			uint32_t m_End;
			EntityView* m_View;

		public:
			Iterator(uint32_t index, uint32_t end, EntityView* view)
				: m_Index(index), m_End(end), m_View(view)
			{
				_SkipInvalid();
			}

			Iterator& operator++() {
				++m_Index;
				_SkipInvalid();
				return *this;
			}

			bool operator !=(const Iterator& other) const {
				return m_Index != other.m_Index;
			}

			auto operator*() const {
				uint32_t entityIndex = m_View->m_Smallest->GetEntityAt(m_Index);
				return std::forward_as_tuple(entityIndex, std::get<ComponentPool<T>*>(m_View->m_Pools)->Get(entityIndex)...);
			}

		private:
			template<size_t... Is>
			bool _HasAllComponents(uint32_t entityIndex, std::index_sequence<Is...>) const {
				return (std::get<Is>(m_View->m_Pools)->Has(entityIndex) && ...);
			}

			void _SkipInvalid() {
				if (!m_View || !m_View->m_Smallest)
				{
					m_Index = m_End;
					return;
				}

				while (m_Index < m_End)
				{
					uint32_t entityIndex = m_View->m_Smallest->GetEntityAt(m_Index);

					if (_HasAllComponents(entityIndex, std::index_sequence_for<T...>{}))
						return;

					++m_Index;
				}
			}
		};

		class ConstIterator
		{
		private:
			uint32_t m_Index;
			uint32_t m_End;
			const EntityView* m_View;

		public:
			ConstIterator(uint32_t index, uint32_t end, const EntityView* view)
				: m_Index(index), m_End(end), m_View(view)
			{
				_SkipInvalid();
			}

			ConstIterator& operator++() {
				++m_Index;
				_SkipInvalid();
				return *this;
			}

			bool operator !=(const ConstIterator& other) const {
				return m_Index != other.m_Index;
			}

			auto operator*() const {
				uint32_t entityIndex = m_View->m_Smallest->GetEntityAt(m_Index);
				return std::forward_as_tuple(entityIndex, std::get<ComponentPool<T>*>(m_View->m_Pools)->Get(entityIndex)...);
			}

		private:
			template<size_t... Is>
			bool _HasAllComponents(uint32_t entityIndex, std::index_sequence<Is...>) const {
				return (std::get<Is>(m_View->m_Pools)->Has(entityIndex) && ...);
			}

			void _SkipInvalid() {
				if (!m_View || !m_View->m_Smallest)
				{
					m_Index = m_End;
					return;
				}

				while (m_Index < m_End)
				{
					uint32_t entityIndex = m_View->m_Smallest->GetEntityAt(m_Index);

					if (_HasAllComponents(entityIndex, std::index_sequence_for<T...>{}))
						return;

					++m_Index;
				}
			}
		};

		// ---
		Iterator begin() { 
			if (!m_Smallest)
				return Iterator(0, 0, this);

			return Iterator(0, m_Smallest->size(), this);
		}

		Iterator end() { 
			if (!m_Smallest)
				return Iterator(0, 0, this);

			return Iterator(m_Smallest->size(), m_Smallest->size(), this); 
		}

		ConstIterator begin() const {
			if (!m_Smallest)
				return ConstIterator(0, 0, this);

			return ConstIterator(0, m_Smallest->size(), this);
		}

		ConstIterator end() const {
			if (!m_Smallest)
				return ConstIterator(0, 0, this);

			return ConstIterator(m_Smallest->size(), m_Smallest->size(), this);
		}

	private:
		template <typename Tup, size_t... Is>
		static IComponentPool* _FindSmallestPoolImpl(const Tup& pools, std::index_sequence<Is...>) {
			IComponentPool* result = nullptr;
			size_t smallestSize = MAX_ENTITIES;

			auto check = [&](auto* pool) {
				if (pool && pool->size() < smallestSize) {
					smallestSize = pool->size();
					result = pool;
				}
			};

			(check(std::get<Is>(pools)), ...);
			return result;
		}

		static IComponentPool* _FindSmallestPool(const std::tuple<ComponentPool<T>*...>& pools) {
			return _FindSmallestPoolImpl(pools, std::index_sequence_for<T...>{});
		}
	};

	// --------------------------------------------------
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		EntityManager(const EntityManager&) = delete;
		EntityManager& operator=(const EntityManager&) = delete;

		// --- ENTITIES ---
		Entity CreateEntity();
		//Entity CreatePrefab(Prefab::Type type, Entity entity = Entity::null);
		//Entity DuplicateEntity(Entity entity);
		void DestroyEntity(Entity entity);
		bool Exists(Entity entity) const;

		// --- COMPONENTS ---
		template <typename T>
		bool HasComponent(Entity entity) const {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			const ComponentPool<T>* pool = _GetPool<T>();
			return (pool && pool->Has(entity.index));
		}

		template <typename... T>
		bool HasComponents(Entity entity) const {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			return (HasComponent<T>(entity) && ...);
		}

		template <typename T>
		T& AddComponent(Entity entity, const T& component) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->Add(entity.index, component);
		}

		template <typename T, typename... Args>
		T& AddComponent(Entity entity, Args&&... args) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->Add(entity.index, std::forward<Args>(args)...);
		}
		
		template <typename T>
		T& ReplaceComponent(Entity entity, const T& component) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Entity does not have this component");

			return pool->Replace(entity.index, component);
		}

		template <typename T, typename... Args>
		T& ReplaceComponent(Entity entity, Args&&... args) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Entity does not have this component");

			return pool->Replace(entity.index, std::forward<Args>(args)...);
		}

		template <typename T>
		T& AddOrReplaceComponent(Entity entity, const T& component) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->AddOrReplace(entity.index, component);
		}

		template <typename T, typename... Args>
		T& AddOrReplaceComponent(Entity entity, Args&&... args) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->AddOrReplace(entity.index, std::forward<Args>(args)...);
		}

		template <typename T>
		T& InsertComponentAfter(Entity entity, Entity entityIndex, const T& component) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");
			DVS_CORE_ASSERT(Exists(entityIndex), "Entity Index is invalid");

			ComponentPool<T>* pool = _GetPool<T>();

			uint32_t index = pool->GetIndexOf(entityIndex.index);
			return pool->InsertAfter(entity.index, index + 1, component);
		}

		template <typename T, typename... Args>
		T& InsertComponentAfter(Entity entity, Entity entityIndex, Args&&... args) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");
			DVS_CORE_ASSERT(Exists(entityIndex), "Entity Index is invalid");

			ComponentPool<T>* pool = _GetPool<T>();

			uint32_t index = pool->GetIndexOf(entityIndex.index);
			return pool->InsertAfter(entity.index, index + 1, std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent(Entity entity) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			pool->Remove(entity.index);
		}

		template <typename... T>
		void RemoveComponents(Entity entity) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			(RemoveComponent<T>(entity), ...);
		}

		template <typename T>
		inline void RemoveComponentOrdered(Entity entity, uint32_t childCount = 0) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			pool->RemoveOrdered(entity.index, childCount);
		}

		template <typename... T>
		inline void RemoveComponentsOrdered(Entity entity, uint32_t childCount = 0) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			(RemoveComponentOrdered<T>(entity, childCount), ...);
		}

		template <typename T>
		T& GetComponent(Entity entity) {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			return pool->Get(entity.index);
		}

		template <typename T>
		const T& GetComponent(Entity entity) const {
			DVS_CORE_ASSERT(Exists(entity), "Entity ID not found");

			const ComponentPool<T>* pool = _GetPool<T>();
			DVS_CORE_ASSERT(pool, "Component Type doesn't exist");

			return pool->Get(entity.index);
		}

		template <typename T>
		T* TryGetComponent(Entity entity) {
			if (!Exists(entity))
				return nullptr;

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				return nullptr;

			return pool->TryGet(entity.index);
		}

		template <typename T>
		const T* TryGetComponent(Entity entity) const {
			if (!Exists(entity))
				return nullptr;

			const ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				return nullptr;

			return pool->TryGet(entity.index);
		}

		template <typename... T>
		EntityView<T...> GetView() {
			auto pools = std::make_tuple(_GetPool<T>()...);

			if ((!std::get<ComponentPool<T>*>(pools) || ...))
				return EntityView<T...>();
			
			return EntityView<T...>(pools);
		}

		template <typename... T>
		const EntityView<T...> GetView() const {
			auto pools = std::make_tuple(_GetPool<T>()...);

			if ((!std::get<ComponentPool<T>*>(pools) || ...))
				return EntityView<T...>();

			return EntityView<T...>(pools);
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
		inline const ComponentPool<T>* _GetPool() const {
			auto typeID = std::type_index(typeid(T));
			auto it = m_ComponentPools.find(typeID);

			if (it == m_ComponentPools.end())
				return nullptr;

			return static_cast<const ComponentPool<T>*>(it->second.get());
		}

		template <typename T>
		inline ComponentPool<T>* _GetOrCreatePool() {
			if (auto* pool = _GetPool<T>())
				return pool;
			return _CreatePool<T>();
		}

	private:
		std::queue<uint32_t> m_AvailableIndices;
		std::array<uint8_t, MAX_ENTITIES> m_Generations;
		uint32_t m_NumEntities = 0;

		std::unordered_map<std::type_index, Scope<IComponentPool>> m_ComponentPools;
	};

}

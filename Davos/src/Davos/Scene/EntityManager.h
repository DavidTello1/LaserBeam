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
		public:
			Iterator(uint32_t index, uint32_t end, EntityView* view) 
				: index(index), end(end), view(view) 
			{
				_SkipInvalid();
			}

			Iterator& operator++() {
				++index;
				_SkipInvalid();
				return *this;
			}

			bool operator !=(const Iterator& other) const {
				return index != other.index;
			}

			auto operator*() const {
				Entity entity = view->m_Smallest->GetEntityAt(index);
				return std::forward_as_tuple(entity, std::get<ComponentPool<T>*>(view->m_Pools)->Get(entity)...);
			}

		private:
			void _SkipInvalid() {
				if (!view->m_Smallest)
					return;

				while (index < end)
				{
					Entity entity = view->m_Smallest->GetEntityAt(index);

					if ((std::get<ComponentPool<T>*>(view->m_Pools)->Has(entity) && ...))
						break;

					++index;
				}
			}

		private:
			uint32_t index;
			uint32_t end;
			EntityView* view;
		};

		class ConstIterator
		{
		public:
			ConstIterator(uint32_t index, uint32_t end, const EntityView* view)
				: index(index), end(end), view(view)
			{
				_SkipInvalid();
			}

			ConstIterator& operator++() {
				++index;
				_SkipInvalid();
				return *this;
			}

			bool operator !=(const ConstIterator& other) const {
				return index != other.index;
			}

			auto operator*() const {
				Entity entity = view->m_Smallest->GetEntityAt(index);
				return std::forward_as_tuple(entity, std::get<ComponentPool<T>*>(view->m_Pools)->Get(entity)...);
			}

		private:
			void _SkipInvalid() {
				if (!view->m_Smallest)
					return;

				while (index < end)
				{
					Entity entity = view->m_Smallest->GetEntityAt(index);

					if ((std::get<ComponentPool<T>*>(view->m_Pools)->Has(entity) && ...))
						break;

					++index;
				}
			}

		private:
			uint32_t index;
			uint32_t end;
			const EntityView* view;
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
		template <typename Tup, size_t... I>
		static IComponentPool* _FindSmallestPoolImpl(const Tup& pools, std::index_sequence<I...>) {
			IComponentPool* result = nullptr;
			size_t smallestSize = MAX_ENTITIES;

			auto check = [&](auto* pool) {
				if (pool && pool->size() < smallestSize) {
					smallestSize = pool->size();
					result = pool;
				}
			};

			(check(std::get<I>(pools)), ...);
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

		// --- ENTITIES ---
		Entity CreateEntity();
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

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->Add(id, component);
		}

		template <typename T, typename... Args>
		T& AddComponent(Entity id, Args&&... args) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
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

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
			return pool->AddOrReplace(id, component);
		}

		template <typename T, typename... Args>
		T& AddOrReplaceComponent(Entity id, Args&&... args) {
			DVS_CORE_ASSERT(Exists(id), "Entity ID not found");

			ComponentPool<T>* pool = _GetOrCreatePool<T>();
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
		T* TryGetComponent(Entity id) {
			if (!Exists(id))
				return nullptr;

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				return nullptr;

			return pool->TryGet(id);
		}

		template <typename T>
		const T* TryGetComponent(Entity id) const {
			if (!Exists(id))
				return nullptr;

			ComponentPool<T>* pool = _GetPool<T>();
			if (!pool)
				return nullptr;

			return pool->TryGet(id);
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
		inline ComponentPool<T>* _GetOrCreatePool() {
			if (auto* pool = _GetPool<T>())
				return pool;
			return _CreatePool<T>();
		}

	private:
		std::queue<Entity> m_AvailableIndices;
		uint32_t m_NumEntities = 0;

		//std::array<ComponentMask, MAX_ENTITIES> m_EntityMasks;		
		std::unordered_map<std::type_index, Scope<IComponentPool>> m_ComponentPools;
	};

}

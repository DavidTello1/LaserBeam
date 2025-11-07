#pragma once

namespace Davos {

	constexpr uint32_t MAX_ENTITIES = 10000;

	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;

		virtual bool Has(uint32_t entity) const = 0;
		virtual void Remove(uint32_t entity) = 0;
		virtual void Duplicate(uint32_t entity_src, uint32_t entity_dst) = 0;
		virtual uint32_t GetEntityAt(uint32_t index) const = 0;
		virtual uint32_t size() const = 0;
	};

	template <typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		ComponentPool() : count(0) {
			sparse.fill(MAX_ENTITIES);
			dense.fill(0);
		}

		~ComponentPool() = default;

		// ---
		bool Has(uint32_t entity) const override {
			DVS_CORE_ASSERT(entity < MAX_ENTITIES, "Entity ID is out of bounds");

			return sparse[entity] < count && dense[sparse[entity]] == entity;
		}

		T& Add(uint32_t entity, const T& component) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");

			sparse[entity] = count;
			dense[count] = entity;
			components[count] = component;
			count++;

			return components[count - 1];
		}

		template <typename... Args>
		T& Add(uint32_t entity, Args&&... args) {
			DVS_CORE_ASSERT(!Has(entity), "Entity already has this component");

			sparse[entity] = count;
			dense[count] = entity;
			components[count] = T(std::forward<Args>(args)...);
			count++;

			return components[count - 1];
		}

		T& Replace(uint32_t entity, const T& component) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = sparse[entity];
			components[index] = component;

			return components[index];
		}

		template <typename... Args>
		T& Replace(uint32_t entity, Args&&... args) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = sparse[entity];
			components[index] = T(std::forward<Args>(args)...);

			return components[index];
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

		void Duplicate(uint32_t entity_src, uint32_t entity_dst) override {
			DVS_CORE_ASSERT(Has(entity_src), "Source Entity ID not found");

			uint32_t index = sparse[entity_src];
			Add(entity_dst, components[index]);
		}

		void Remove(uint32_t entity) override {
			DVS_CORE_ASSERT(Has(entity), "Entity ID not found");

			uint32_t index = sparse[entity];
			uint32_t lastIndex = dense[count - 1];

			dense[index] = lastIndex;
			sparse[lastIndex] = index;
			components[index] = components[count - 1];

			--count;
			sparse[entity] = MAX_ENTITIES;
		}

		T& Get(uint32_t entity) {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = sparse[entity];
			return components[index];
		}

		const T& Get(uint32_t entity) const {
			DVS_CORE_ASSERT(Has(entity), "Entity does not have this component");

			uint32_t index = sparse[entity];
			return components[index];
		}

		uint32_t GetEntityAt(uint32_t index) const override {
			DVS_CORE_ASSERT(index < count, "Index is out of bounds");
			return dense[index];
		}

		uint32_t size() const override { 
			return count;
		}

	private:
		std::array<uint32_t, MAX_ENTITIES> dense;
		std::array<uint32_t, MAX_ENTITIES> sparse;

		std::array<T, MAX_ENTITIES> components; // actual data, order is the same as dense
		uint32_t count = 0;
	};

}

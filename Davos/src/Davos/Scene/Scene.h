#pragma once
#include "EntityManager.h"

#include "Davos/Core/UUID.h"

namespace Davos {

	class UUID;
	class TimeStep;
	class Camera;
	struct C_Camera;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(TimeStep dt);
		void OnRender();

		//void OnStart();
		//void OnStop();
		void OnViewportResize(uint32_t width, uint32_t height);

		UUID GetMainCamera() const { return m_MainCamera; }
		void SetMainCamera(UUID camera);

		//UUID FindEntity(const std::string& name);

		//bool IsRunning() const { return m_IsRunning; }
		//bool IsPaused() const { return m_IsPaused; }

		//void SetPaused(bool paused) { m_IsPaused = paused; }
		//void Step(int frames = 1);

		// -------------------------------------------
		// --- ENTITIES ---
		UUID CreateEntity(UUID id = 0);
		//Entity CreatePrefab(Prefab::Type type, Entity id = 0);
		UUID DuplicateEntity(UUID id);
		void DestroyEntity(UUID id);

		template <typename T>
		inline EntityView<T>& GetAllEntitiesWith() {
			return m_EntityManager.GetView<T>();
		}

		template <typename T>
		inline const EntityView<T>& GetAllEntitiesWith() const { 
			return m_EntityManager.GetView<T>();
		}

		template <typename... T>
		inline EntityView<T...>& GetAllEntitiesWith() {
			return m_EntityManager.GetView<T...>();
		}

		template <typename... T>
		inline const EntityView<T...>& GetAllEntitiesWith() const {
			return m_EntityManager.GetView<T...>(); 
		}

		// -------------------------------------------
		// --- COMPONENTS ---
		template <typename T>
		inline bool HasComponent(UUID id) { 
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.HasComponent<T>(handle); 
		}

		template <typename... T>
		inline bool HasComponents(UUID id) { 
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.HasComponents<T...>(handle); 
		}

		template <typename T>
		inline T& AddComponent(UUID id, const T& component) {
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.AddComponent<T>(handle, component);
		}

		template <typename T, typename... Args>
		inline T& AddComponent(UUID id, Args&&... args) { 
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.AddComponent<T>(handle, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline T& ReplaceComponent(Entity entity, const T& component) {
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.ReplaceComponent<T>(handle, component);
		}

		template <typename T, typename... Args>
		inline T& ReplaceComponent(Entity entity, Args&&... args) {
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.ReplaceComponent<T>(handle, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		inline T& AddOrReplaceComponent(UUID id, Args&&... args) { 
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.AddOrReplaceComponent<T>(handle, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline void RemoveComponent(UUID id) { 
			Entity handle = m_EntityHandles[id];
			m_EntityManager.RemoveComponent<T>(handle); 
		}

		template <typename... T>
		inline void RemoveComponents(UUID id) { 
			Entity handle = m_EntityHandles[id];
			m_EntityManager.RemoveComponents<T...>(handle); 
		}

		template <typename T>
		inline const T& GetComponent(UUID id) const {
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.GetComponent<T>(handle);
		}

		template <typename T>
		inline T& GetComponent(UUID id) {
			Entity handle = m_EntityHandles[id];
			return m_EntityManager.GetComponent<T>(handle);
		}

	private:
		std::unordered_map<UUID, Entity> m_EntityHandles;
		EntityManager m_EntityManager;

		UUID m_MainCamera = 0;

		//bool m_IsRunning = false;
		//bool m_IsPaused = false;
		//int m_StepFrames = 0;
	};

}

#pragma once
#include "EntityManager.h"

#include "Davos/Core/TimeStep.h"
#include "Davos/Core/UUID.h"

namespace Davos {

	class EditorCamera;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdateRuntime(TimeStep dt);
		void OnUpdateEditor(TimeStep dt, EditorCamera& camera);
		
		void OnStartRuntime();
		void OnStopRuntime();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetRootEntity() const { return m_Root; }
		void SetRootEntity(Entity root) { m_Root = root; }

		Entity GetMainCamera() const { return m_MainCamera; }
		void SetMainCamera(Entity camera);

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }
		void Step(int frames = 1) { m_StepFrames = frames; }

		//UUID FindEntity(const std::string& name);

		// -------------------------------------------
		// --- ENTITIES ---
		Entity CreateEntity() {
			return m_EntityManager.CreateEntity();
		}

		inline Entity DuplicateEntity(Entity id) {
			return m_EntityManager.DuplicateEntity(id);
		}

		inline void DestroyEntity(Entity id) {
			m_EntityManager.DestroyEntity(id);
		}

		//Entity CreatePrefab(Prefab::Type type, Entity id = 0);

		template <typename... T>
		inline EntityView<T...> GetAllEntitiesWith() {
			return m_EntityManager.GetView<T...>();
		}

		template <typename... T>
		inline const EntityView<T...> GetAllEntitiesWith() const {
			return m_EntityManager.GetView<T...>(); 
		}

		// -------------------------------------------
		// --- COMPONENTS ---
		template <typename T>
		inline bool HasComponent(Entity id) const { 
			return m_EntityManager.HasComponent<T>(id); 
		}

		template <typename... T>
		inline bool HasComponents(Entity id) const { 
			return m_EntityManager.HasComponents<T...>(id); 
		}

		template <typename T>
		inline T& AddComponent(Entity id, const T& component) {
			return m_EntityManager.AddComponent<T>(id, component);
		}

		template <typename T, typename... Args>
		inline T& AddComponent(Entity id, Args&&... args) { 
			return m_EntityManager.AddComponent<T>(id, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline T& ReplaceComponent(Entity entity, const T& component) {
			return m_EntityManager.ReplaceComponent<T>(id, component);
		}

		template <typename T, typename... Args>
		inline T& ReplaceComponent(Entity entity, Args&&... args) {
			return m_EntityManager.ReplaceComponent<T>(id, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		inline T& AddOrReplaceComponent(Entity id, Args&&... args) { 
			return m_EntityManager.AddOrReplaceComponent<T>(id, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline void RemoveComponent(Entity id) { 
			m_EntityManager.RemoveComponent<T>(id); 
		}

		template <typename... T>
		inline void RemoveComponents(Entity id) { 
			m_EntityManager.RemoveComponents<T...>(id); 
		}

		template <typename T>
		inline T& GetComponent(Entity id) {
			return m_EntityManager.GetComponent<T>(id);
		}

		template <typename T>
		inline const T& GetComponent(Entity id) const {
			return m_EntityManager.GetComponent<T>(id);
		}

		template <typename T>
		inline T* TryGetComponent(Entity id) {
			return m_EntityManager.TryGetComponent<T>(id);
		}

		template <typename T>
		inline const T* TryGetComponent(Entity id) const {
			return m_EntityManager.TryGetComponent<T>(id);
		}

	private:
		void _RenderScene();

	private:
		EntityManager m_EntityManager;

		Entity m_Root = MAX_ENTITIES;
		Entity m_MainCamera = MAX_ENTITIES;

		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
	};

}

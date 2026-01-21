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
		void OnUpdateEditor(TimeStep dt, EditorCamera& camera, const Entity& selectedEntity);
		
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

		//inline Entity DuplicateEntity(Entity entity) {
		//	return m_EntityManager.DuplicateEntity(entity);
		//}

		inline void DestroyEntity(Entity entity) {
			m_EntityManager.DestroyEntity(entity);
		}

		//Entity CreatePrefab(Prefab::Type type, Entity entity = 0);

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
		inline bool HasComponent(Entity entity) const { 
			return m_EntityManager.HasComponent<T>(entity); 
		}

		template <typename... T>
		inline bool HasComponents(Entity entity) const { 
			return m_EntityManager.HasComponents<T...>(entity); 
		}

		template <typename T>
		inline T& AddComponent(Entity entity, const T& component) {
			return m_EntityManager.AddComponent<T>(entity, component);
		}

		template <typename T, typename... Args>
		inline T& AddComponent(Entity entity, Args&&... args) { 
			return m_EntityManager.AddComponent<T>(entity, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline T& ReplaceComponent(Entity entity, const T& component) {
			return m_EntityManager.ReplaceComponent<T>(entity, component);
		}

		template <typename T, typename... Args>
		inline T& ReplaceComponent(Entity entity, Args&&... args) {
			return m_EntityManager.ReplaceComponent<T>(entity, std::forward<Args>(args)...);
		}

		template <typename T>
		inline T& AddOrReplaceComponent(Entity entity, const T& component) {
			return m_EntityManager.AddOrReplaceComponent<T>(entity, component);
		}

		template <typename T, typename... Args>
		inline T& AddOrReplaceComponent(Entity entity, Args&&... args) { 
			return m_EntityManager.AddOrReplaceComponent<T>(entity, std::forward<Args>(args)...); 
		}

		template <typename T>
		inline T& InsertComponentAfter(Entity newEntity, Entity entityIndex, const T& component) {
			return m_EntityManager.InsertComponentAfter<T>(newEntity, entityIndex, component);
		}

		template <typename T, typename... Args>
		inline T& InsertComponentAfter(Entity newEntity, Entity entityIndex, Args&&... args) {
			return m_EntityManager.InsertComponentAfter<T>(newEntity, entityIndex, std::forward<Args>(args)...);
		}

		template <typename T>
		inline void RemoveComponent(Entity entity) { 
			m_EntityManager.RemoveComponent<T>(entity); 
		}

		template <typename... T>
		inline void RemoveComponents(Entity entity) { 
			m_EntityManager.RemoveComponents<T...>(entity); 
		}

		template <typename T>
		inline void RemoveComponentOrdered(Entity entity, uint32_t childCount = 0) {
			m_EntityManager.RemoveComponentOrdered<T>(entity, childCount);
		}

		template <typename... T>
		inline void RemoveComponentsOrdered(Entity entity, uint32_t childCount = 0) {
			m_EntityManager.RemoveComponentsOrdered<T...>(entity, childCount);
		}

		template <typename T>
		inline T& GetComponent(Entity entity) {
			return m_EntityManager.GetComponent<T>(entity);
		}

		template <typename T>
		inline const T& GetComponent(Entity entity) const {
			return m_EntityManager.GetComponent<T>(entity);
		}

		template <typename T>
		inline T* TryGetComponent(Entity entity) {
			return m_EntityManager.TryGetComponent<T>(entity);
		}

		template <typename T>
		inline const T* TryGetComponent(Entity entity) const {
			return m_EntityManager.TryGetComponent<T>(entity);
		}

	private:
		void _RenderScene();

	private:
		EntityManager m_EntityManager;

		Entity m_Root = Entity::null;
		Entity m_MainCamera = Entity::null;

		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
	};

}

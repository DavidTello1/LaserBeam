#include "dvs_pch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"

#include "Davos/Renderer/Renderer.h"
#include "Davos/Editor/EditorCamera.h"

namespace Davos {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		// Destroy Scripts
		for (auto [entity, script] : m_EntityManager.GetView<C_NativeScript>())
		{
			if (script.instance)
				script.DestroyScript(&script);
		}
	}

	void Scene::OnUpdateRuntime(TimeStep dt)
	{
		// --- Update Entities ---
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update Native_Scripts
			auto view = m_EntityManager.GetView<C_NativeScript>();
			for (auto [entity, script] : view)
			{
				DVS_CORE_ASSERT(script.instance, "Error script has not been instantiated");
				script.instance->OnUpdate(dt);
			}

			// Update Physics //***
			// ...
		}

		// --- Render Scene ---
		if (m_MainCamera == Entity::null)
		{
			DVS_CORE_WARN("MainCamera is not defined");
			return;
		}

		// Get MainCamera
		Camera mainCamera;
		glm::mat4 mainCameraTransform;
		auto cameraView = m_EntityManager.GetView<C_Transform, C_Camera>();
		for (auto [entity, transform, camera] : cameraView)
		{
			if (entity != m_MainCamera)
				continue;

			mainCamera = (Camera)camera.camera;
			mainCameraTransform = transform.GetTransform();
			break;
		}

		// Render
		Renderer::BeginScene(mainCamera, mainCameraTransform);
		_RenderScene();
		Renderer::EndScene();
	}

	void Scene::OnUpdateEditor(TimeStep dt, EditorCamera& camera)
	{
		// --- Render Scene ---
		Renderer::BeginScene(camera);
		_RenderScene();
		Renderer::EndScene();
	}

	void Scene::OnStartRuntime()
	{
		m_IsRunning = true;

		// Instantiate Scripts
		auto view = m_EntityManager.GetView<C_NativeScript>();
		for (auto [entity, script] : view)
		{
			Entity id = Entity(entity); //*** generation is lost
			if (!script.instance)
			{
				script.instance = script.InstantiateScript();
				script.instance->m_Handle = id;
				script.instance->m_Scene = this;
				script.instance->OnCreate();
			}
		}
	}

	void Scene::OnStopRuntime()
	{
		m_IsRunning = false;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		// Resize all Non-FixedAspectRatio Cameras
		auto view = m_EntityManager.GetView<C_Camera>();
		for (auto [entity, camera] : view)
		{
			if (camera.isFixedAspectRatio)
				continue;

			camera.camera.SetViewportSize(width, height);
		}
	}

	void Scene::SetMainCamera(Entity entity)
	{
		DVS_CORE_ASSERT(!entity.isNull(), "Invalid Entity");
		DVS_CORE_ASSERT(m_EntityManager.HasComponent<C_Camera>(entity), "Entity doesn't have Camera component");

		m_MainCamera = entity;
	}

	// -----------------------------------------------
	void Scene::_RenderScene()
	{
		// Sprites
		const auto spriteView = m_EntityManager.GetView<C_Transform, C_SpriteRenderer>();
		for (const auto& [entity, transform, sprite] : spriteView)
		{
			if (sprite.texture.get())
				Renderer::DrawSprite(transform.GetTransform(), sprite.texture, sprite.color, sprite.tilingFactor, (int)entity);
			else
				Renderer::DrawQuad(transform.GetTransform(), sprite.color, (int)entity);
		}

		//// TileMap
		//const auto tileMapView = m_EntityManager.GetView<C_Transform, C_LevelMap>();
		//for (const auto& [entity, transform, tileMap] : tileMapView)
		//{
		//	// Draw Grid
		//	if (tileMap.isDrawGrid)
		//	{
		//		for (int i = 0; i < tileMap.gridSize.x + 1; ++i) // Vertical Lines
		//		{
		//			glm::vec3 start = glm::vec3(
		//				transform.translation.x + (tileMap.cellSize.x * i),
		//				transform.translation.y,
		//				transform.translation.z + 0.1f //*** manual offset
		//			);

		//			glm::vec3 end = glm::vec3(
		//				start.x,
		//				start.y + (tileMap.cellSize.y * tileMap.gridSize.y),
		//				start.z
		//			);

		//			Renderer::DrawLine(start, end, tileMap.gridColor);
		//		}

		//		for (int i = 0; i < tileMap.gridSize.y + 1; ++i) // Horizontal Lines
		//		{
		//			glm::vec3 start = glm::vec3(
		//				transform.translation.x,
		//				transform.translation.y + (tileMap.cellSize.y * i),
		//				transform.translation.z + 0.1f //*** manual offset
		//			);

		//			glm::vec3 end = glm::vec3(
		//				start.x + (tileMap.cellSize.x * tileMap.gridSize.x),
		//				start.y,
		//				start.z
		//			);

		//			Renderer::DrawLine(start, end, tileMap.gridColor);
		//		}
		//	}

		//	// Draw TileMap
		//	for (uint32_t i = 0; i < tileMap.cells.size(); ++i)
		//	{
		//		glm::vec3 tilePos = glm::vec3(
		//			transform.translation.x + (tileMap.cellSize.x * (i % tileMap.gridSize.x)),
		//			transform.translation.y + (tileMap.cellSize.y * (i / tileMap.gridSize.x)),
		//			transform.translation.z
		//		);

		//		glm::vec3 tileScale = glm::vec3(
		//			transform.scale.x * tileMap.cellSize.x,
		//			transform.scale.y * tileMap.cellSize.y,
		//			transform.scale.z
		//		);

		//		glm::mat4 tileTransform = glm::translate(glm::mat4(1.0f), tilePos) * glm::scale(glm::mat4(1.0f), tileScale);

		//		Renderer::DrawQuad(tileTransform, tileMap.tintColor);
		//		//Renderer::DrawTile(tileTransform, tileMap.tileset.texture.get(), tileCoords, { tileScale.x, tileScale.y }, tileMap.tintColor);
		//	}
		//}
	}

}

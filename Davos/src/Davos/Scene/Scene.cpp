#include "dvs_pch.h"
#include "Scene.h"

#include "Davos/Core/TimeStep.h"
#include "Davos/Renderer/Renderer.h"
#include "Davos/Scene/Components.h"
#include "Davos/Renderer/OrthographicCamera.h" //***

namespace Davos {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(TimeStep dt)
	{
	}

	void Scene::OnRender()
	{
		if (m_MainCamera == 0)
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
			if (entity != m_EntityHandles[m_MainCamera])
				continue;

			mainCamera = (Camera)camera.camera;
			mainCameraTransform = transform.GetTransform();
			break;
		}

		// Render Scene
		Renderer::BeginScene(mainCamera, mainCameraTransform);

		const auto spriteView = m_EntityManager.GetView<C_Transform, C_SpriteRenderer>();
		for (auto [entity, transform, sprite] : spriteView)
		{
			if (sprite.texture.get())
				Renderer::DrawSprite(transform.GetTransform(), sprite.texture, sprite.color, sprite.tilingFactor);
			else
				Renderer::DrawQuad(transform.GetTransform(), sprite.color);
		}

		Renderer::EndScene();
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

	void Scene::SetMainCamera(UUID camera)
	{
		DVS_CORE_ASSERT(camera != 0, "Invalid UUID");

		Entity entity = m_EntityHandles[camera];
		DVS_CORE_ASSERT(m_EntityManager.HasComponent<C_Camera>(entity), "Entity doesn't have Camera component");

		m_MainCamera = camera;
	}

	// -----------------------------------------------
	UUID Scene::CreateEntity(UUID id)
	{
		UUID entity = UUID();
		m_EntityHandles[entity] = m_EntityManager.CreateEntity();
		return entity;
	}

	UUID Scene::DuplicateEntity(UUID id)
	{
		UUID entity = UUID();
		m_EntityManager.DuplicateEntity(m_EntityHandles[id]);
		return entity;
	}

	void Scene::DestroyEntity(UUID id)
	{
		m_EntityManager.DestroyEntity(m_EntityHandles[id]);
	}

}

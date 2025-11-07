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

	void Scene::OnRender(const Camera& camera)
	{
		//if (!m_MainCamera)
		//{
		//	DVS_CORE_WARN("Trying to render Scene without camera");
		//	return;
		//}

		Renderer::BeginScene(camera);

		const EntityView<C_Transform, C_SpriteRenderer>& view = m_EntityManager.GetView<C_Transform, C_SpriteRenderer>();
		for (auto [entity, transform, sprite] : view)
		{
			if (sprite.texture.get())
				Renderer::DrawSprite(transform.GetTransform(), sprite.texture, sprite.color, sprite.tilingFactor);
			else
				Renderer::DrawQuad(transform.GetTransform(), sprite.color);
		}

		Renderer::EndScene();

		//	// Renderer Draw
		//	// ---
		//	static float rotation = 0.0f;
		//	rotation += m_RectRotationSpeed * dt;
		//	if (rotation >= 360.0f)
		//		rotation = 0.0f;

		//	Renderer::BeginScene(m_CameraController.GetCamera());

		//	Renderer::DrawRectFilled({ 0.0f, 0.0f, 1.0f }, rotation, { 1.0f, 1.0f }, m_RectBackgroundColor, m_RectBorderColor);
		//	Renderer::DrawRect({ -2.05f, 0.25f, 1.0f }, 0.0f, { 1.0f, 1.0f }, m_RectBorderColor);

		//	Renderer::DrawSprite({ 2.0f, 0.0f, 1.0f }, m_SpriteRotation, { 0.8f, 0.8f }, m_Texture, { 0.2f, 0.3f, 0.8f, 0.5f });
		//	Renderer::DrawSprite({ 0.0f, 0.0f, 0.5f }, 0.0f, { 4.0f, 4.0f }, m_Texture, glm::vec4(1.0f), 10.0f);

		//	Renderer::EndScene();

		//	// ---
		//	Renderer::BeginScene(m_CameraController.GetCamera());

		//	for (float y = -5.0f; y < 5.0f; y += 0.5f)
		//	{
		//		for (float x = -5.0f; x < 5.0f; x += 0.5f)
		//		{
		//			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
		//			Renderer::DrawRectFilled({ x, y, 0.0f }, 0.0f, { 0.45f, 0.45f }, color);
		//		}
		//	}

		//	Renderer::EndScene();
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

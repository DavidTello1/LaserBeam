#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Davos {

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnInit()
	{
		//FB_Specs fbSpec;
		////fbSpec.attachments = { FB_TextureFormat::RGBA8, FB_TextureFormat::RED_INTEGER, FB_TextureFormat::DEPTH };
		//fbSpec.width = 1280;
		//fbSpec.height = 720;
		//m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_Texture = Texture2D::Create("assets/textures/checkerboard.png");

		//Renderer::SetLineWidth(m_LineWidth);

		//*** ---
		//m_Camera = m_ActiveScene->CreateEntity();
		//m_ActiveScene->AddComponent<C_Transform>(m_Camera);
		//C_CameraController cameraController = m_ActiveScene->AddComponent<C_CameraController>(m_Camera, 1280.0f / 720.0f);

		m_Square = m_ActiveScene->CreateEntity();
		m_ActiveScene->AddComponent<C_Transform>(m_Square);
		m_ActiveScene->AddComponent<C_SpriteRenderer>(m_Square, m_RectBackgroundColor);
	}

	void EditorLayer::OnCleanUp()
	{
	}

	void EditorLayer::OnUpdate(TimeStep dt)
	{
		//m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		//// --- Resize
		//if (FB_Specs spec = m_Framebuffer->GetSpecification();
		//	m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		//	(spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y))
		//{
		//	m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		//	m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		//	m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		//}

		// --- Update
		//  Camera Update
		m_CameraController.OnUpdate(dt);
		//C_CameraController camera = m_ActiveScene->GetComponent<C_CameraController>(m_Camera);
		//C_Transform cameraTransform = m_ActiveScene->GetComponent<C_Transform>(m_Camera);
		//camera.controller.OnUpdate(dt);

		// --- Render
		Renderer::ResetStats();
		
		// Renderer Prep
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		// Renderer Draw
		m_ActiveScene->OnRender(m_CameraController.GetCamera());
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Debug Panel");

		Renderer::Statistics stats = Renderer::GetStats();
		ImGui::Text("--- Renderer Stats ---");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::Separator();
		ImGui::Text("--- Scene Settings ---");

		//ImGui::PushItemWidth(100.0f);
		//ImGui::DragFloat("Sprite Rotation", &m_SpriteRotation, 1.0f, -360.0f, 360.0f, "%.1f");
		//ImGui::DragFloat("Rect Rotation Speed", &m_RectRotationSpeed, 0.1f, -100.0f, 100.0f, "%.1f");
		//ImGui::DragFloat("Line Width", &m_LineWidth, 0.1f, 1.0f, 5.0f, "%.1f");
		//ImGui::PopItemWidth();

		ImGui::ColorEdit4("Rect Color", glm::value_ptr(m_ActiveScene->GetComponent<C_SpriteRenderer>(m_Square).color));
		//ImGui::ColorEdit4("Rect Border Color", glm::value_ptr(m_RectBorderColor));

		////ImGui::InputText("Text", &text.c_str(), text.length());

		//ImGui::Separator();
		//ImGui::Text("--- Scene Settings ---");

		//ImGui::PushItemWidth(100.0f);
		//static float moveSpeed = m_CameraController.GetMoveSpeed();
		//if (ImGui::DragFloat("Move Speed", &moveSpeed, 1.0f, 0.0f, 1000.0f, "%.1f"))
		//	m_CameraController.SetMoveSpeed(moveSpeed);

		//static float zoomSensitivity = m_CameraController.GetZoomSensitivity();
		//if (ImGui::DragFloat("Zoom Sensitivity", &zoomSensitivity, 0.01f, 0.0f, 1.0f, "%.2f"))
		//	m_CameraController.SetZoomSensitivity(zoomSensitivity);
		//ImGui::PopItemWidth();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		//C_CameraController camera = m_ActiveScene->GetComponent<C_CameraController>(m_Camera);
		//camera.controller.OnEvent(e);

		m_CameraController.OnEvent(e);
	}

}

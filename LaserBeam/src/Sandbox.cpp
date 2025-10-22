#include "Sandbox.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox::Sandbox() : Layer("Sandbox"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox::OnInit()
{
	m_Texture = Davos::Texture2D::Create("assets/textures/checkerboard.png");
}

void Sandbox::OnCleanUp()
{
}

void Sandbox::OnUpdate(Davos::TimeStep dt)
{
	// Update
	m_CameraController.OnUpdate(dt);

	// Render
	Davos::Renderer::ResetStats();
	{
		// Renderer Prep
		Davos::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Davos::RenderCommand::Clear();

		Davos::Renderer::SetLineWidth(m_LineWidth);
	}

	{
		// Renderer Draw
		// ---
		static float rotation = 0.0f;
		rotation += m_RectRotationSpeed * dt;
		if (rotation >= 360.0f)
			rotation = 0.0f;

		Davos::Renderer::BeginScene(m_CameraController.GetCamera());

		Davos::Renderer::DrawRectFilled({ 0.0f, 0.0f, 1.0f }, rotation, { 1.0f, 1.0f }, m_RectBackgroundColor, m_RectBorderColor);
		Davos::Renderer::DrawRect({ -2.05f, 0.25f, 1.0f }, 0.0f, { 1.0f, 1.0f }, m_RectBorderColor);

		Davos::Renderer::DrawSprite({ 2.0f, 0.0f, 0.0f }, m_SpriteRotation, { 0.8f, 0.8f }, m_Texture, { 0.2f, 0.3f, 0.8f, 0.5f });
		Davos::Renderer::DrawSprite({ 0.0f, 0.0f, 0.0f }, 0.0f, { 4.0f, 4.0f }, m_Texture, glm::vec4(1.0f), 10.0f);


		Davos::Renderer::EndScene();

		// ---
		Davos::Renderer::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Davos::Renderer::DrawRectFilled({ x, y, 0.0f }, 0.0f, { 0.45f, 0.45f }, color);
			}
		}

		Davos::Renderer::EndScene();
	}
}

void Sandbox::OnImGuiRender()
{
	ImGui::Begin("Debug Panel");

	Davos::Renderer::Statistics stats = Davos::Renderer::GetStats();
	ImGui::Text("--- Renderer Stats ---");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::Separator();
	ImGui::Text("--- Scene Settings ---");

	ImGui::PushItemWidth(100.0f);
	ImGui::DragFloat("Sprite Rotation", &m_SpriteRotation, 1.0f, -360.0f, 360.0f, "%.1f");
	ImGui::DragFloat("Rect Rotation Speed", &m_RectRotationSpeed, 0.1f, -100.0f, 100.0f, "%.1f");
	ImGui::DragFloat("Line Width", &m_LineWidth, 0.1f, 1.0f, 5.0f, "%.1f");
	ImGui::PopItemWidth();

	ImGui::ColorEdit4("Rect Background Color", glm::value_ptr(m_RectBackgroundColor));
	ImGui::ColorEdit4("Rect Border Color", glm::value_ptr(m_RectBorderColor));

	//ImGui::InputText("Text", &text.c_str(), text.length());

	ImGui::Separator();
	ImGui::Text("--- Scene Settings ---");

	ImGui::PushItemWidth(100.0f);
	static float moveSpeed = m_CameraController.GetMoveSpeed();
	if (ImGui::DragFloat("Move Speed", &moveSpeed, 1.0f, 0.0f, 1000.0f, "%.1f"))
		m_CameraController.SetMoveSpeed(moveSpeed);

	static float zoomSensitivity = m_CameraController.GetZoomSensitivity();
	if (ImGui::DragFloat("Zoom Sensitivity", &zoomSensitivity, 0.01f, 0.0f, 1.0f, "%.2f"))
		m_CameraController.SetZoomSensitivity(zoomSensitivity);
	ImGui::PopItemWidth();

	ImGui::End();
}

void Sandbox::OnEvent(Davos::Event& e)
{
	m_CameraController.OnEvent(e);
}

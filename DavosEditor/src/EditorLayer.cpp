#include "EditorLayer.h"

#include "NativeScripts/CameraController.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Davos {

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
	}

	void EditorLayer::OnInit()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		// Panels
		m_PanelViewport.OnInit();
		m_PanelHierarchy.SetScene(m_ActiveScene);
		m_PanelInspector.SetScene(m_ActiveScene);

		// Assets
		m_Texture = Texture2D::Create("assets/textures/checkerboard.png");

		//Renderer::SetLineWidth(m_LineWidth);

		// -------------------------------------------
		//// Camera
		//m_Camera = m_ActiveScene->CreateEntity("Camera");
		//m_ActiveScene->AddComponent<C_Transform>(m_Camera);
		//m_ActiveScene->AddComponent<C_Camera>(m_Camera);
		//m_ActiveScene->AddComponent<C_NativeScript>(m_Camera).Bind<CameraController>();
		//m_ActiveScene->SetMainCamera(m_Camera);

		//// Square
		//m_Square = m_ActiveScene->CreateEntity("Square");
		//m_ActiveScene->AddComponent<C_Transform>(m_Square);
		//m_ActiveScene->AddComponent<C_SpriteRenderer>(m_Square, m_RectBackgroundColor);

		//// Sprite
		//m_Sprite = m_ActiveScene->CreateEntity("Sprite");
		//auto& spriteTransform = m_ActiveScene->AddComponent<C_Transform>(m_Sprite);
		//spriteTransform.translation = { 2.0f, 0.0f, 1.0f };
		//spriteTransform.rotation = { 0.0f, 0.0f, glm::radians(m_SpriteRotation) };
		//spriteTransform.scale = { 0.8f, 0.8f, 1.0f };
		//auto& spriteRenderer = m_ActiveScene->AddComponent<C_SpriteRenderer>(m_Sprite);
		//spriteRenderer.texture = m_Texture;

		//// Rotating Square
		//m_RotatingSquare = m_ActiveScene->CreateEntity("Rotating Square");
		//auto& rsquareTransform = m_ActiveScene->AddComponent<C_Transform>(m_RotatingSquare);
		//rsquareTransform.translation = { -2.5f, 0.0f, 1.0f };
		//rsquareTransform.scale = { 2.0f, 2.0f, 1.0f };
		//m_ActiveScene->AddComponent<C_SpriteRenderer>(m_RotatingSquare, m_RectBackgroundColor);

		//// Quad Grid
		//for (float y = -5.0f; y < 5.0f; y += 0.5f)
		//{
		//	for (float x = -5.0f; x < 5.0f; x += 0.5f)
		//	{
		//		glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };

		//		UUID cell = m_ActiveScene->CreateEntity();

		//		auto& cellTransform = m_ActiveScene->AddComponent<C_Transform>(cell);
		//		cellTransform.translation = { x, y, 0.0f };
		//		cellTransform.scale = { 0.45f, 0.45f, 1.0f };

		//		m_ActiveScene->AddComponent<C_SpriteRenderer>(cell, color);
		//	}
		//}
	}

	void EditorLayer::OnCleanUp()
	{
	}

	void EditorLayer::OnUpdate(TimeStep dt)
	{
		// --- Resize
		m_PanelViewport.OnResize();

		glm::vec2 viewportSize = m_PanelViewport.GetViewportSize();
		m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

		//// --- Update
		//if (m_PanelViewport.IsFocused())
		//	m_CameraController.OnUpdate(dt);

		m_ActiveScene->OnUpdate(dt);

		////*** DEBUG ***
		//{ 
		//	// Rotating Quad Update [make NATIVE_SCRIPT]
		//	static float rotation = 0.0f;
		//	rotation += m_RectRotationSpeed * dt;
		//	if (rotation >= 360.0f)
		//		rotation = 0.0f;
		//	auto& rsquareTransform = m_ActiveScene->GetComponent<C_Transform>(m_RotatingSquare);
		//	rsquareTransform.SetRotation(rotation);
		//}

		// --- Render
		Renderer::ResetStats();
		m_PanelViewport.BindFramebuffer();

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		m_ActiveScene->OnRender();
		m_PanelViewport.UnbindFramebuffer();
	}

	void EditorLayer::OnImGuiRender()
	{
		// Dockspace
		_Dockspace();

		// Main MenuBar
		_DrawMainMenubar();

		// ToolBar
		_DrawToolbar();

		// Panels
		m_PanelViewport.OnRender();
		m_PanelHierarchy.OnImGuiRender();

		Node* node = m_PanelHierarchy.GetSelectedNode();
		m_PanelInspector.SetSelectedNode(node);
		m_PanelInspector.OnImGuiRender();

		//---------------------------------------------------
		//*** DEBUG
		ImGui::Begin("Debug Panel");

		Renderer::Statistics stats = Renderer::GetStats();
		ImGui::Text("--- Renderer Stats ---");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		//ImGui::Separator();
		//ImGui::Text("--- Scene Settings ---");

		//ImGui::PushItemWidth(100.0f);

		//m_SpriteRotation = m_ActiveScene->GetComponent<C_Transform>(m_Sprite).GetRotationDegrees();
		//if (ImGui::DragFloat("Sprite Rotation", &m_SpriteRotation, 1.0f, -360.0f, 360.0f, "%.1f"))
		//	m_ActiveScene->GetComponent<C_Transform>(m_Sprite).SetRotation(m_SpriteRotation);

		//ImGui::DragFloat("Rect Rotation Speed", &m_RectRotationSpeed, 0.1f, -100.0f, 100.0f, "%.1f");

		////ImGui::DragFloat("Line Width", &m_LineWidth, 0.1f, 1.0f, 5.0f, "%.1f");
		//ImGui::PopItemWidth();

		//ImGui::ColorEdit4("Rect Color", glm::value_ptr(m_ActiveScene->GetComponent<C_SpriteRenderer>(m_Square).color));
		////ImGui::ColorEdit4("Rect Border Color", glm::value_ptr(m_RectBorderColor));

		//////ImGui::InputText("Text", &text.c_str(), text.length());

		////ImGui::Separator();
		////ImGui::Text("--- Scene Settings ---");

		////ImGui::PushItemWidth(100.0f);
		////static float moveSpeed = m_CameraController.GetMoveSpeed();
		////if (ImGui::DragFloat("Move Speed", &moveSpeed, 1.0f, 0.0f, 1000.0f, "%.1f"))
		////	m_CameraController.SetMoveSpeed(moveSpeed);

		////static float zoomSensitivity = m_CameraController.GetZoomSensitivity();
		////if (ImGui::DragFloat("Zoom Sensitivity", &zoomSensitivity, 0.01f, 0.0f, 1.0f, "%.2f"))
		////	m_CameraController.SetZoomSensitivity(zoomSensitivity);
		////ImGui::PopItemWidth();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		//C_CameraController camera = m_ActiveScene->GetComponent<C_CameraController>(m_Camera);
		//camera.controller.OnEvent(e);

		//m_CameraController.OnEvent(e);
	}

	// -----------------------------------------------
	void EditorLayer::_DrawMainMenubar()
	{
		// --- Escena ---
		// 
		// New Scene (Ctrl+N)
		// Open Scene (Ctrl+O)
		// ---
		// Save Scene (Ctrl+S)
		// Save Scene As...
		// Save All Scenes (Ctrl+Shift+S)
		// ---
		// Undo (Ctrl+Z)
		// Redo (Ctrl+Shift+Z)
		// ---
		// Close Scene (Ctrl+Shift+W)
		// ---
		// Exit (Ctrl+Q)


		// --- Proyecto ---
		//
		// Settings 
		// Build


		// --- Scene State --- (Centered)
		//
		// Play (Opens Game Window)
		// Pause
		// Stop (Closes Game Window)
	}

	void EditorLayer::_DrawToolbar()
	{

	}

	void EditorLayer::_Dockspace()
	{
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		ImGui::End();
	}

}

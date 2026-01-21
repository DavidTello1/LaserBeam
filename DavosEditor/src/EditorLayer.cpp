#include "EditorLayer.h"

#include "NativeScripts/CameraController.h"

#include <Davos/Scene/SceneSerializer.h>
#include <Davos/Utils/PlatformUtils.h>

#include <Davos/Events/KeyEvent.h>
#include <Davos/Events/MouseEvent.h>

#include <imgui/imgui.h>

namespace Davos {

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
	}

	void EditorLayer::OnInit()
	{
		//// Editor Assets
		//m_Texture = Texture2D::Create("assets/textures/checkerboard.png");
		////...

		// --- Scene
		_NewScene();
		Entity root = m_ActiveScene->CreateEntity();
		m_ActiveScene->AddComponent<C_UUID>(root);
		m_ActiveScene->AddComponent<C_Hierarchy>(root);
		m_ActiveScene->AddComponent<C_Transform>(root);
		m_ActiveScene->AddComponent<C_Tag>(root, "Root");
		m_ActiveScene->SetRootEntity(root);

		// --- Panels
		m_PanelViewport.OnInit();
		m_PanelAssets.OnInit();

		// --- Renderer
		//Renderer::SetLineWidth(m_LineWidth);

		//***
		_OnCreateEntity(EntityType::CAMERA, root);
		_OnCreateEntity(EntityType::SPRITE, root);
		Entity empty = _OnCreateEntity(EntityType::EMPTY, root);
		_OnCreateEntity(EntityType::SPRITE, empty);
		m_PanelHierarchy.m_EditorLayer = this;
		//---
	}

	void EditorLayer::OnCleanUp()
	{
	}

	void EditorLayer::OnUpdate(TimeStep dt)
	{
		// --- Resize ---
		if (m_PanelViewport.OnResize())
		{
			glm::vec2 viewportSize = m_PanelViewport.GetPanelSize();
			m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		}

		// --- Update & Render
		Renderer::ResetStats();
		m_PanelViewport.BindFramebuffer();

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		// --- Clear entity ID attachment to -1
		m_PanelViewport.ClearFramebufferAttachment(1, -1);

		//if (m_SceneState == SceneState::PLAY)
			//m_ActiveScene->OnUpdateRuntime(dt);
		//else if (m_SceneState == SceneState::EDIT)
		{
			m_ActiveScene->OnUpdateEditor(dt, m_PanelViewport.GetEditorCamera(), m_SelectedEntity);
			m_PanelViewport.OnUpdate(dt);
			_OnMousePicking();
		}

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

		// --- Change Cursor when Drag&Drop
		if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
			ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);

		// Panels
		m_PanelViewport.OnImGuiRender(m_ActiveScene, m_SelectedEntity);
		m_PanelHierarchy.OnImGuiRender(m_ActiveScene, m_SelectedEntity);
		m_PanelInspector.OnImGuiRender(m_ActiveScene, m_SelectedEntity);
		m_PanelAssets.OnImGuiRender();

		//---------------------------------------------------
		//*** DEBUG
		ImGui::Begin("Debug Panel");

		Renderer::Statistics stats = Renderer::GetStats();
		ImGui::Text("--- Renderer Stats ---");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Lines: %d", stats.lineCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		//if (m_SceneState == SceneState::EDIT)
		//{
			m_PanelViewport.OnEvent(e);
		//}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressEvent>(DVS_BIND_EVENT_FN(EditorLayer::_OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressEvent>(DVS_BIND_EVENT_FN(EditorLayer::_OnMouseButtonPressed));
	}

	void EditorLayer::OnProcessLast()
	{
		// Deferred Entity deletion to avoid conflicts
		for (Entity id : m_DirtyEntities)
		{
			if (id == m_ActiveScene->GetRootEntity()) // Root entity cannot be deleted
				continue;

			_OnDestroyEntity(id);
		}
		m_DirtyEntities.clear();
	}

	// --------------------------------------------------
	void EditorLayer::_OnMousePicking()
	{
		glm::vec2 viewportSize = m_PanelViewport.GetPanelSize();
		glm::vec2 mouse = Input::GetMouseScreenPos();
		mouse -= m_PanelViewport.GetPanelPosition();
		mouse.y = viewportSize.y - mouse.y; // inverted Y

		int mouseX = (int)mouse.x;
		int mouseY = (int)mouse.y;
		if (mouseX < 0 || mouseY < 0 || mouseX >= (int)viewportSize.x || mouseY >= (int)viewportSize.y)
			return;

		int pixelData = m_PanelViewport.ReadPixel(1, mouseX, mouseY);
		m_HoveredEntity = (pixelData == -1) ? Entity::null : Entity(pixelData);
	}

	// --------------------------------------------------
	bool EditorLayer::_OnKeyPressed(KeyPressEvent& e)
	{
		// --- Shortcuts ---
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LEFT_CONTROL) || Input::IsKeyPressed(Key::RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(Key::LEFT_SHIFT) || Input::IsKeyPressed(Key::RIGHT_SHIFT);

		// --- SceneFile Commands
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control) { _NewScene(); }
				break;
			}
			case Key::O:
			{
				if (control) { _OpenScene(); }
				break;
			}
			case Key::S:
			{
				if (control) {
					if (shift) { _SaveSceneAs(); }
					else	   { _SaveScene(); }
				}
				break;
			}

			//// --- Scene Commands
			//case Key::D:
			//{
			//	if (control)
			//		OnDuplicateEntity();

			//	break;
			//}
			//case Key::Delete:
			//{
			//	if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
			//	{
			//		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			//		if (selectedEntity)
			//		{
			//			m_SceneHierarchyPanel.SetSelectedEntity({});
			//			m_ActiveScene->DestroyEntity(selectedEntity);
			//		}
			//	}
			//	break;
			//}
		}

		// --- Viewport
		m_PanelViewport.OnShortcuts(e.GetKeyCode(), control, shift);

		return false;
	}

	bool EditorLayer::_OnMouseButtonPressed(MouseButtonPressEvent& e)
	{
		if (e.GetMouseButton() == Mouse::BUTTON_LEFT)
		{
			if (m_PanelViewport.IsHovered())
			{
				if (m_SelectedEntity.isNull() || (!m_SelectedEntity.isNull() && !m_PanelViewport.IsOverGizmo()))
					m_SelectedEntity = m_HoveredEntity;
			}
		}

		return false;
	}

	// --------------------------------------------------
	Entity EditorLayer::_OnCreateEntity(EntityType type, Entity parent)
	{
		DVS_CORE_ASSERT(!parent.isNull(), "Trying to create entity with null parent");

		C_Hierarchy& parentNode = m_ActiveScene->GetComponent<C_Hierarchy>(parent);

		// Get Last Descendant (index where we will insert the ordered components)
		Entity lastDescendant = parent;
		if (parentNode.numChilds > 0)
		{
			C_Hierarchy* node = &parentNode;
			while (node->lastChild != Entity::null)
			{
				lastDescendant = node->lastChild;
				node = &m_ActiveScene->GetComponent<C_Hierarchy>(node->lastChild);
			}
		}

		// Create Entity
		Entity entity = m_ActiveScene->CreateEntity();

		// Default Components (keep same order as scene tree)
		m_ActiveScene->InsertComponentAfter<C_UUID>(entity, lastDescendant);
		m_ActiveScene->InsertComponentAfter<C_Transform>(entity, lastDescendant);
		C_Tag& tag = m_ActiveScene->InsertComponentAfter<C_Tag>(entity, lastDescendant);
		C_Hierarchy& entityNode = m_ActiveScene->InsertComponentAfter<C_Hierarchy>(entity, lastDescendant, parent);

		switch (type)
		{
			case EntityType::EMPTY:
			{
				tag.name = "Empty Entity";
				break;
			}

			case EntityType::CAMERA:
			{
				tag.name = "Camera";
				m_ActiveScene->AddComponent<C_Camera>(entity);
				m_ActiveScene->AddComponent<C_NativeScript>(entity).Bind<CameraController>();

				if (m_ActiveScene->GetMainCamera() == Entity::null)
					m_ActiveScene->SetMainCamera(entity);
				break;
			}

			case EntityType::SPRITE:
			{
				tag.name = "Sprite";
				m_ActiveScene->AddComponent<C_SpriteRenderer>(entity);
				break;
			}
		}

		// Udpate Hierarchy Tree
		entityNode.numParents = parentNode.numParents + 1;

		if (parentNode.lastChild != Entity::null)
		{
			C_Hierarchy& lastChildNode = m_ActiveScene->GetComponent<C_Hierarchy>(parentNode.lastChild);

			lastChildNode.nextSibling = entity;
			entityNode.prevSibling = parentNode.lastChild;
		}

		if (parentNode.firstChild == Entity::null)
			parentNode.firstChild = entity;

		parentNode.numChilds++;
		parentNode.totalChilds++;
		parentNode.lastChild = entity;

		// Update TotalChilds up the Tree
		if (parentNode.parent != Entity::null)
		{
			C_Hierarchy* node = m_ActiveScene->TryGetComponent<C_Hierarchy>(parentNode.parent);
			while (node != nullptr)
			{
				node->totalChilds++;
				node = m_ActiveScene->TryGetComponent<C_Hierarchy>(node->parent);
			}
		}

		DVS_CORE_TRACE("Created Entity with ID: {0} and parent: {1}", (int)entity.index, (int)parent.index);

		return entity;
	}

	void EditorLayer::_OnDestroyEntity(Entity entity)
	{
		DVS_CORE_ASSERT(!entity.isNull(), "Invalid Entity");
		DVS_CORE_ASSERT(entity != m_ActiveScene->GetRootEntity(), "Trying to delete root node");

		const C_Hierarchy& entityNode = m_ActiveScene->GetComponent<C_Hierarchy>(entity);

		// Update Sibling Nodes
		if (entityNode.prevSibling != Entity::null)
		{
			auto& prevSibling = m_ActiveScene->GetComponent<C_Hierarchy>(entityNode.prevSibling);
			prevSibling.nextSibling = entityNode.nextSibling;
		}
		if (entityNode.nextSibling != Entity::null)
		{
			auto& nextSibling = m_ActiveScene->GetComponent<C_Hierarchy>(entityNode.nextSibling);
			nextSibling.prevSibling = entityNode.prevSibling;
		}

		// Update Parent Node
		C_Hierarchy* parentNode = m_ActiveScene->TryGetComponent<C_Hierarchy>(entityNode.parent);
		if (parentNode != nullptr)
		{
			if (parentNode->firstChild == entity) parentNode->firstChild = entityNode.nextSibling;
			if (parentNode->lastChild  == entity) parentNode->lastChild  = entityNode.prevSibling;
			parentNode->numChilds--;

			// Update totalChilds up the tree
			while (parentNode != nullptr)
			{
				parentNode->totalChilds -= (entityNode.totalChilds + 1);
				parentNode = m_ActiveScene->TryGetComponent<C_Hierarchy>(parentNode->parent);
			}
		}

		// Get all childs
		std::vector<Entity> entities = _GetAllChilds(entityNode.firstChild);
		entities.push_back(entity);

		// Remove Default Components (keep SceneTree ordered)
		m_ActiveScene->RemoveComponentsOrdered<C_UUID, C_Transform, C_Tag, C_Hierarchy>(entity, entityNode.totalChilds);

		// Destroy entity and all of its childs
		for (size_t i = 0; i < entities.size(); ++i)
		{
			Entity id = entities[i];
			m_ActiveScene->DestroyEntity(id);

			DVS_CORE_TRACE("Destroyed Entity with ID: {0}", (int)id.index);
		}

		m_SelectedEntity = Entity::null;
	}

	void EditorLayer::_OnSelectEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void EditorLayer::_MarkToDelete(Entity entity)
	{
		m_DirtyEntities.push_back(entity);
	}

	std::vector<Entity> EditorLayer::_GetAllChilds(Entity firstChild)
	{
		std::vector<Entity> entities;

		Entity child = firstChild;
		while (child != Entity::null)
		{
			entities.push_back(child);
			auto* node = &m_ActiveScene->GetComponent<C_Hierarchy>(child);

			if (node->numChilds > 0)
			{
				std::vector<Entity> childs = _GetAllChilds(node->firstChild);
				entities.insert(entities.end(), childs.begin(), childs.end());
			}
			child = node->nextSibling;
		}

		return entities;
	}

	// --------------------------------------------------
	void EditorLayer::_NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();

		//m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::_OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Davos Scene (*.davos)\0*.davos\0");
		if (!filepath.empty())
			_OpenScene(filepath);
	}

	void EditorLayer::_OpenScene(const std::filesystem::path& path)
	{
		//if (m_SceneState != SceneState::Edit)
		//	OnSceneStop();

		if (path.extension().string() != ".davos")
		{
			DVS_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();

		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_ActiveScene = newScene;
			//m_EditorScenePath = path;
		}
	}

	void EditorLayer::_SaveScene()
	{
		//if (!m_EditorScenePath.empty())
		//	_SerializeScene(m_ActiveScene, m_EditorScenePath);
		//else
			_SaveSceneAs();
	}

	void EditorLayer::_SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Davos Scene (*.davos)\0*.davos\0");
		if (!filepath.empty())
		{
			_SerializeScene(m_ActiveScene, filepath);
			//m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::_SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	// --------------------------------------------------
	void EditorLayer::_DrawMainMenubar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// --- File
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))		{ _NewScene(); }
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) { _OpenScene(); }
				ImGui::Separator();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))	{ _SaveScene(); }
				if (ImGui::MenuItem("Save Scene As..."))		{ _SaveSceneAs(); }
				if (ImGui::MenuItem("Save All Scenes", "Ctrl+Shift+S", nullptr, false)) { /*_SaveAllScenes();*/ }
				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, false))		 { /*_Undo();*/ }
				if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", nullptr, false)) { /*_Redo();*/ }
				ImGui::Separator();

				if (ImGui::MenuItem("Close Scene", "Ctrl+Shift+W", nullptr, false)) { /*_CloseScene();*/ }
				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "Ctrl+Q")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}

			// --- Project
			if (ImGui::BeginMenu("Project"))
			{
				ImGui::MenuItem("Settings...");
				ImGui::Separator();

				ImGui::MenuItem("Build...", nullptr, nullptr, false);
				ImGui::Separator();

				ImGui::MenuItem("Reload Project", nullptr, nullptr, false);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

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

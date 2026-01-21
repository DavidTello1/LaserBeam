#include "PanelHierarchy.h"

#define IMGUI_DEFINE_MATH_OPERATORS //*** remove when decoupled from EditorLayer
#include <imgui/imgui.h>

#include "../EditorLayer.h" //***

namespace Davos {

	void PanelHierarchy::OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity)
	{
		ImGui::Begin("Hierarchy");

		if (scene->GetRootEntity() == Entity::null)
		{
			ImGui::End();
			return;
		}

		// --- Blank Space
		// Left-Click
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_EditorLayer->m_SelectedEntity = Entity::null; //***
			//EventEntitySelected(Entity::null);
		}

		// Right-Click
		if (!selectedEntity.isNull() && ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			_RightClickOptions();
			ImGui::EndPopup();
		}

		// --- Entity Nodes
		const auto view = scene->GetAllEntitiesWith<C_Hierarchy, C_Tag>();
		auto it = view.begin();
		auto end = view.end();

		while (it != end)
		{
			auto [entity, hierarchy, tag] = *it;

			Entity id = Entity(entity); //*** generation is lost
			bool isOpen = _DrawEntityNode(id, tag.name.c_str(), (selectedEntity == entity), (hierarchy.numChilds == 0), hierarchy.numParents);

			// Left-Click (Select Entity)
			if (ImGui::IsItemClicked())
			{
				//EventEntitySelected(entity); //***
				m_EditorLayer->m_SelectedEntity = id;
			}

			// Right-Click (Select Entity & Open Popup)
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				//EventEntitySelected(entity); //***
				m_EditorLayer->m_SelectedEntity = id;
				_RightClickOptions();

				ImGui::EndPopup();
			}
			
			if (!isOpen)
			{
				for (uint32_t i = 0; i < hierarchy.totalChilds; ++i)
					++it;
			}

			++it;
		}

		ImGui::End();
	}

	// --------------------------------------------------
	void PanelHierarchy::_DrawMenuBar()
	{
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("Create"))
		{
			_CreateNodeOptions();
			ImGui::EndMenu();
		}

		//ImGui::InputText("", search,)

		ImGui::EndMenuBar();

		//// --- + Button (Add Entity -> Opens Popup with list of Prefabs/Components)
		//// Adds the NewEntity as a child of the Selected_Entity (single selection)
		//// if there is a multiselection the NewEntity is always added to the first Entity (top-most)
	
		//// Search Bar (Filters entities by name)
		//// If a filtered node has a parent that is not included in the filter, it is shown but in translucent
		//// and is not selectable (blocked node)

		//// ---------------------------------------------------
		//// --- Main Begin ---

		//// --- Node
		//// Arrow (if is parent) / Nothing (if leaf)
		//// Icon (double-left-click focuses editor camera on node)
		//// Name
		//// Eye_Icon (toggles visibility)
		//
		//// Right Click on blank space only shows popup if there is a selected entity
		//// Right Click on node selects the node and opens popup
		//// Right Click in multiselection shows different popup (no Create Child)
		//
		//// Left Click on blank space unselects nodes
		//// Left Click on unselected node selects node
		//// Left Click on selected node -> rename


		//// ---------------------------------------------------
		//// --- Drag and Drop

		//// Shows Node Icon + Node Name
		//// Reparent Node
		//// Reorder Node
		//// Create Prefab
	}

	bool PanelHierarchy::_DrawEntityNode(int id, const char* name, bool isSelected, bool isLeaf, uint32_t indent)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (isSelected)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (isLeaf)
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::SetCursorPosX(indent * ImGui::GetStyle().IndentSpacing);

		ImGui::PushID(id);
		bool isOpen = ImGui::TreeNodeEx(name, flags);
		ImGui::PopID();

		return isOpen;
	}

	void PanelHierarchy::_RightClickOptions()
	{
		if (ImGui::BeginMenu("Create"))
		{
			_CreateNodeOptions();
			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X"))
		{
			//***
		}
		if (ImGui::MenuItem("Copy", "Ctrl+X"))
		{
			//***
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
		{
			//***
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Save as Prefab"))
		{
			//***
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Delete", "Supr"))
		{
			m_EditorLayer->_MarkToDelete(m_EditorLayer->m_SelectedEntity);
			//m_EditorLayer->_OnDestroyEntity(m_EditorLayer->m_SelectedEntity);
			//EventEntityDestroyed(/*selectedEntity*/);
		}
	}

	void PanelHierarchy::_CreateNodeOptions()
	{
		if (ImGui::MenuItem("Empty Entity")) m_EditorLayer->_OnCreateEntity(EntityType::EMPTY, m_EditorLayer->m_SelectedEntity); //{ EventEntityCreated(EntityType::EMPTY); }
		else if (ImGui::MenuItem("Camera"))	 m_EditorLayer->_OnCreateEntity(EntityType::CAMERA, m_EditorLayer->m_SelectedEntity); //{ EventEntityCreated(EntityType::CAMERA); }
		else if (ImGui::MenuItem("Sprite"))	 m_EditorLayer->_OnCreateEntity(EntityType::SPRITE, m_EditorLayer->m_SelectedEntity); //{ EventEntityCreated(EntityType::SPRITE); }
		//else if (ImGui::MenuItem("GridMap")) m_EditorLayer->_OnCreateEntity(EntityType::TILEMAP, m_EditorLayer->m_SelectedEntity); //{ EventEntityCreated(EntityType::TILEMAP); }
	}

}

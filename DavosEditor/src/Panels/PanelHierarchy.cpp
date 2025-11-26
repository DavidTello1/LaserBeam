#include "PanelHierarchy.h"

#include "../NativeScripts/CameraController.h"
#include <imgui/imgui.h>

namespace Davos {

	PanelHierarchy::PanelHierarchy(const Ref<Scene>& scene)
	{
		SetScene(scene);
	}

	PanelHierarchy::~PanelHierarchy()
	{
		for (int i = (int)m_NumNodes - 1; i >= 0; --i)
		{
			delete m_Nodes[i];
			m_Nodes[i] = nullptr;
		}
		m_NumNodes = 0;
	}

	void PanelHierarchy::SetScene(const Ref<Scene>& scene)
	{
		m_Scene = scene;

		m_NumNodes = 0;
		m_Nodes.fill(nullptr);

		// Create Root Manually
		Entity id = m_Scene->CreateEntity();
		Node* root = new Node(id, "Root", nullptr);
		m_Nodes[0] = root;
		m_NumNodes++;

		m_SelectedNode = root;

		// Create Main Camera
		Node* node = CreateEntity("Camera", root);
		m_Scene->AddComponent<C_Transform>(node->id);
		m_Scene->AddComponent<C_Camera>(node->id);
		m_Scene->AddComponent<C_NativeScript>(node->id).Bind<CameraController>();

		if (m_Scene->GetMainCamera() == MAX_ENTITIES)
			m_Scene->SetMainCamera(node->id);
	}

	void PanelHierarchy::OnImGuiRender()
	{
		ImGui::ShowDemoWindow(); //***

		ImGui::Begin("Hierarchy");

		if (!m_Scene)
		{
			ImGui::End();
			return;
		}

		// Draw Nodes
		_DrawEntityNode(m_Nodes[0]);

		// Left-Click (Blank Space)
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedNode = nullptr;

		// Right-Click (Blank Space)
		if (m_SelectedNode != nullptr && ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			_RightClickOptions();
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void PanelHierarchy::SetSelectedEntity(Entity entity)
	{
		DVS_CORE_ASSERT(entity != MAX_ENTITIES, "Invalid Entity");

		uint32_t index = _FindNode(entity);
		if (index == MAX_ENTITIES)
			return;

		m_SelectedNode = m_Nodes[index];
	}

	Node* PanelHierarchy::CreateEntity(const std::string& name, Node* parent)
	{
		DVS_CORE_ASSERT(parent, "Trying to create entity with null parent");

		// Create Node
		Entity id = m_Scene->CreateEntity();
		Node* node = new Node(id, name + std::to_string(id), parent);

		uint32_t parentIndex = _FindNode(parent->id);
		DVS_CORE_ASSERT(parentIndex != MAX_ENTITIES, "Parent Node not found");

		uint32_t nodeIndex = parentIndex + parent->totalChilds + 1;

		// Shift Right from back
		for (uint32_t i = m_NumNodes; i > nodeIndex; --i)
		{
			m_Nodes[i] = m_Nodes[i - 1];
		}

		// Insert Node
		m_Nodes[nodeIndex] = node;
		m_NumNodes++;

		// Update parent's childs
		parent->childs.push_back(node);
		parent->numChilds++;
		parent->totalChilds++;

		// Update totalChilds up the tree
		parent = parent->parent;
		while (parent != nullptr)
		{
			parent->totalChilds++;
			parent = parent->parent;
		}

		return node;
	}

	void PanelHierarchy::RemoveEntity(Entity id)
	{
		DVS_CORE_ASSERT(id > 0 && id < MAX_ENTITIES, "Invalid Entity ID");

		// Remove Entity from Scene
		m_Scene->DestroyEntity(id);

		uint32_t index = _FindNode(id);
		DVS_CORE_ASSERT(index != MAX_ENTITIES, "Entity not found");

		Node* node = m_Nodes[index];
		uint32_t lastChildIndex = index + node->totalChilds;
		uint32_t numRemoved = node->totalChilds + 1;

		// Delete All Children (starting from the back)
		for (uint32_t i = lastChildIndex; i > index; --i)
		{
			m_Scene->DestroyEntity(m_Nodes[i]->id);

			delete m_Nodes[i];
			m_Nodes[i] = nullptr;
		}

		// Update parent's childs
		Node* parent = node->parent;
		for (auto it = parent->childs.begin(); it != parent->childs.end(); ++it)
		{
			if ((*it) != node)
				continue;

			parent->childs.erase(it);
			parent->numChilds--;
			parent->totalChilds -= numRemoved;
			break;
		}

		// Update totalChilds up the tree
		parent = parent->parent;
		while (parent != nullptr)
		{
			parent->totalChilds -= numRemoved;
			parent = parent->parent;
		}

		// Delete Node
		delete m_Nodes[index];
		m_Nodes[index] = nullptr;

		// Shift Left remaining nodes
		uint32_t cursor = index;
		for (uint32_t i = lastChildIndex + 1; i < m_NumNodes; ++i)
		{
			m_Nodes[cursor] = m_Nodes[i];
			m_Nodes[i] = nullptr;
			cursor++;
		}

		m_NumNodes -= numRemoved;
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

		// --- + Button (Add Entity -> Opens Popup with list of Prefabs/Components)
		// Adds the NewEntity as a child of the Selected_Entity (single selection)
		// if there is a multiselection the NewEntity is always added to the firts Entity (top-most)
	
		// Search Bar (Filters entities by name)
		// If a filtered node has a parent that is not included in the filter, it is shown but in translucent
		// and is not selectable (blocked node)

		// ---------------------------------------------------
		// --- Main Begin ---

		// --- Node
		// Arrow (if is parent) / Nothing (if leaf)
		// Icon (double-left-click focuses editor camera on node)
		// Name
		// Eye_Icon (toggles visibility)
		
		// Right Click on blank space only shows popup if there is a selected entity
		// Right Click on node selects the node and opens popup
		// Right Click in multiselection shows different popup (no Create Child)
		
		// Left Click on blank space unselects nodes
		// Left Click on unselected node selects node
		// Left Click on selected node -> rename


		// ---------------------------------------------------
		// --- Right Click Options (Single Selection)

		// Add Child Node (Ctrl+A)
		// ---
		// Cut (Ctrl+X)
		// Copy (Ctrl+C)
		// ---
		// Rename (F2)
		// ---
		// Duplicate (Ctrl+D)
		// ---
		// Save as Prefab
		// ---
		// Delete (Supr)
		

		// ---------------------------------------------------
		// --- Right Click Options (Multi Selection)

		// Cut (Ctrl+X)
		// Copy (Ctrl+C)
		// ---
		// Duplicate (Ctrl+D)
		// ---
		// Delete (Supr)


		// ---------------------------------------------------
		// --- Drag and Drop

		// Shows Node Icon + Node Name
		// Reparent Node
		// Reorder Node
	}

	void PanelHierarchy::_DrawEntityNode(Node* node)
	{
		DVS_CORE_ASSERT(node, "Node is nullptr");

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

		if (m_SelectedNode == node)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (node->numChilds == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool isOpen = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)node->id, flags, node->name.c_str());

		// Left-Click (Select Entity or Rename)
		if (ImGui::IsItemClicked())
		{
			if (m_SelectedNode != node)
				m_SelectedNode = node;
			else
				node->isRename = true;
		}

		// Right-Click (Select Entity)
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			m_SelectedNode = node;
			_RightClickOptions();

			ImGui::EndPopup();
		}

		// Draw Childs
		if (isOpen)
		{
			for (Node* child : node->childs)
				_DrawEntityNode(child);

			ImGui::TreePop();
		}
	}

	void PanelHierarchy::_RightClickOptions()
	{
		if (ImGui::BeginMenu("Create"))
		{
			_CreateNodeOptions();
			ImGui::EndMenu();
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, false))
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+X", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Rename", "F2", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Save as Prefab", nullptr, false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Delete", "Supr", false, (m_SelectedNode->id != 0)))
		{
			RemoveEntity(m_SelectedNode->id);
			m_SelectedNode = nullptr;
		}
	}

	void PanelHierarchy::_CreateNodeOptions()
	{
		Node* parent = (m_SelectedNode) ? m_SelectedNode : m_Nodes[0];

		// Empty Entity
		if (ImGui::MenuItem("Empty Entity"))
		{
			CreateEntity("Entity", parent);
		}

		// Camera
		else if (ImGui::MenuItem("Camera"))
		{
			Node* node = CreateEntity("Camera", parent);
			m_Scene->AddComponent<C_Transform>(node->id);
			m_Scene->AddComponent<C_Camera>(node->id);
			m_Scene->AddComponent<C_NativeScript>(node->id).Bind<CameraController>();

			if (m_Scene->GetMainCamera() == MAX_ENTITIES)
				m_Scene->SetMainCamera(node->id);
		}

		// Sprite
		else if (ImGui::MenuItem("Sprite"))
		{
			Node* node = CreateEntity("Sprite", parent);
			m_Scene->AddComponent<C_Transform>(node->id);
			m_Scene->AddComponent<C_SpriteRenderer>(node->id);
		}

		// Level Map
		else if (ImGui::MenuItem("GridMap"))
		{
			Node* node = CreateEntity("GridMap", parent);
			m_Scene->AddComponent<C_Transform>(node->id);
			m_Scene->AddComponent<C_LevelMap>(node->id);
		}
	}

	uint32_t PanelHierarchy::_FindNode(Entity id)
	{
		DVS_CORE_ASSERT(id < MAX_ENTITIES, "Entity ID is invalid");

		for (uint32_t i = 0; i < m_NumNodes; ++i)
		{
			if (m_Nodes[i]->id == id)
				return i;
		}

		return MAX_ENTITIES;
	}

}

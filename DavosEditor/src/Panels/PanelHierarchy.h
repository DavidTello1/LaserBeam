#pragma once
#include "Node.h"

namespace Davos {

	class PanelHierarchy
	{
	public:
		PanelHierarchy() = default;
		PanelHierarchy(const Ref<Scene>& scene);

		~PanelHierarchy();

		void SetScene(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return (m_SelectedNode) ? m_SelectedNode->id : MAX_ENTITIES; }
		Node* GetSelectedNode() const { return m_SelectedNode; }
		void SetSelectedEntity(Entity entity);

		Node* CreateEntity(const std::string& name, Node* parent);
		void RemoveEntity(Entity id);

		//void ReparentEntity(Entity id, Entity newParent);
		//void ReorderEntity(Entity id, uint8_t position);

	private:
		void _DrawMenuBar();
		void _DrawEntityNode(Node* node);

		void _RightClickOptions();
		void _CreateNodeOptions();

		uint32_t _FindNode(Entity id);

	private:
		Ref<Scene> m_Scene;

		std::array<Node*, MAX_ENTITIES> m_Nodes;
		uint32_t m_NumNodes = 0;

		Node* m_SelectedNode = nullptr;
		// m_Root = m_Nodes[0];
	};

}

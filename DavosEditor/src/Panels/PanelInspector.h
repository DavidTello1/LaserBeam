#pragma once
#include "Node.h"

namespace Davos {

	class PanelInspector
	{
	public:
		PanelInspector() = default;
		PanelInspector(const Ref<Scene>& scene);

		~PanelInspector() = default;

		void SetScene(const Ref<Scene>& scene);

		void OnImGuiRender();

		void SetSelectedNode(Node* node) { m_SelectedNode = node; }

	private:
		void _DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Scene;

		Node* m_SelectedNode = nullptr;
	};

}

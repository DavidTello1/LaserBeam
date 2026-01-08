#pragma once
#include <Davos.h>

namespace Davos {

	class EditorLayer;

	class PanelHierarchy
	{
	public:
		PanelHierarchy() = default;
		~PanelHierarchy() = default;

		void OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity);

	private:
		void _DrawMenuBar();
		bool _DrawEntityNode(int id, const char* name, bool isSelected, bool isLeaf, uint32_t indent);

		void _RightClickOptions();
		void _CreateNodeOptions();

		//***
	public:
		EditorLayer* m_EditorLayer;
	};

}

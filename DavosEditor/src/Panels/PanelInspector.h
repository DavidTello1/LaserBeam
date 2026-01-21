#pragma once
#include "Utils/InspectorUtils.h"

namespace Davos {

	class PanelInspector
	{
	public:
		PanelInspector() = default;
		~PanelInspector() = default;

		void OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity);

	private:
		void _DrawComponents(const Ref<Scene>& scene, Entity entity);
	};

}

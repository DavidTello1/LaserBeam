#pragma once
#include "Davos/Core/Layer.h"

namespace Davos {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void OnInit() override;
		void OnCleanUp() override;
		void OnEvent(Event& event) override;

		void Begin();
		void End();
	};

}

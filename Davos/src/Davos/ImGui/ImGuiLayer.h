#pragma once
#include "Davos/Layer.h"

namespace Davos {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnInit() override;
		void OnCleanUp() override;
		void OnEvent(Event& event) override;

		void OnImGuiRender() override; //**

		void Begin();
		void End();
	};

}

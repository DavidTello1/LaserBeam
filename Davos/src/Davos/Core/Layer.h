#pragma once
#include "TimeStep.h"

namespace Davos {

	class Event;
	
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_DebugName(name) {}
		virtual ~Layer() {}

		virtual void OnInit() {}
		virtual void OnCleanUp() {}
		virtual void OnUpdate(TimeStep dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnProcessLast() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}


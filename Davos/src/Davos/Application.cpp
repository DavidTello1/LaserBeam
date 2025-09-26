#include "dvs_pch.h"
#include "Application.h"

#include "Core/TimeStep.h"

#include "Davos/Renderer/Renderer.h"

#include "ImGui/ImGuiLayer.h"

#include "Davos/Events/Event.h"
#include "Davos/Events/ApplicationEvent.h"

#include <GLFW/glfw3.h> //*** should be in Platform::GetTime

namespace Davos {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		DVS_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(DVS_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_isRunning)
		{
			//float time = Time::GetTime();
			float time = (float)glfwGetTime();
			TimeStep timeStep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timeStep);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnInit();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnInit();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(DVS_BIND_EVENT_FN(Application::OnWindowClose));

		for (std::vector<Layer*>::const_reverse_iterator it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.isHandled)
				break;

			(*it)->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_isRunning = false;
		return true;
	}

}

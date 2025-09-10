#include "dvs_pch.h"
#include "LayerStack.h"

namespace Davos {

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnCleanUp();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerCount, layer);
		m_LayerCount++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerCount, layer);
		if (it != m_Layers.end())
		{
			layer->OnCleanUp();

			m_Layers.erase(it);
			m_LayerCount--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator it = std::find(m_Layers.begin() + m_LayerCount, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnCleanUp();
			m_Layers.erase(it);
		}
	}

}

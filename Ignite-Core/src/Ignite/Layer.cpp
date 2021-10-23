#include "igpch.h"
#include "Ignite/Layer.h"

namespace Ignite
{
	Layer::Layer(const std::string& name) : m_debugName(name)
	{
	}

	Layer::~Layer()
	{
	}

	LayerStack::LayerStack()
	{
		m_layerInsert = m_layers.begin();
	}


	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_layerInsert = m_layers.emplace(m_layerInsert, layer);
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end())
		{
			m_layers.erase(it);
			--m_layerInsert;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end())
		{
			m_layers.erase(it);
		}
	}

	void LayerStack::DeleteLayers()
	{
		for (Layer* layer : m_layers)
		{
			delete layer;
		}
		m_layers.clear();
	}
}

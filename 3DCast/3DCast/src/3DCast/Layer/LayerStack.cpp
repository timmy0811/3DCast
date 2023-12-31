#include "castpch.h"
#include "LayerStack.h"

Cast::LayerStack::LayerStack()
{
}

Cast::LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers) {
		delete layer;
	}
}

void Cast::LayerStack::PushLayer(Layer* layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex++, layer);
}

void Cast::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
}

void Cast::LayerStack::PopLayer(Layer* layer)
{
	auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (iter != m_Layers.end()) {
		m_Layers.erase(iter);
		m_LayerInsertIndex--;
	}
}

void Cast::LayerStack::PopOverlay(Layer* overlay)
{
	auto iter = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if (iter != m_Layers.end()) {
		m_Layers.erase(iter);
	}
}
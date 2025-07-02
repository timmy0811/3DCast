#include "castpch.h"
#include "LayerStack.h"

Cast::LayerStack::LayerStack() = default;

Cast::LayerStack::~LayerStack()
{
	for (const Layer* layer : layers) {
		delete layer;
	}
}

void Cast::LayerStack::PushLayer(Layer* layer)
{
	layers.emplace(layers.begin() + layerInsertIndex++, layer);
}

void Cast::LayerStack::PushOverlay(Layer* overlay)
{
	layers.emplace_back(overlay);
}

void Cast::LayerStack::PopLayer(const Layer* layer)
{
	auto iter = std::find(layers.begin(), layers.end(), layer);
	if (iter != layers.end()) {
		layers.erase(iter);
		layerInsertIndex--;
	}
}

void Cast::LayerStack::PopOverlay(const Layer* overlay)
{
	auto iter = std::find(layers.begin(), layers.end(), overlay);
	if (iter != layers.end()) {
		layers.erase(iter);
	}
}
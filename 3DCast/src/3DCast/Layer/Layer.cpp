#include "castpch.h"
#include "Layer.h"

#include <utility>

Cast::Layer::Layer(std::string  name)
	:debugName(std::move(name))
{
}

Cast::Layer::~Layer() = default;
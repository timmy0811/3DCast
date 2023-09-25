#pragma once

#include "3DCast/Event/Event.h"

namespace Cast {
	static int s_LayerIdentifier = 0;

	class CAST_API Layer
	{
	public:
		Layer(const std::string& name = "Layer_" + std::to_string(s_LayerIdentifier));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}

		virtual void OnImGuiRender() {}

		virtual void OnEvent(Event& e) {}

		inline const std::string& getName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}

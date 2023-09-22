#pragma once

#include "3DCast/Layer/Layer.h"

namespace Cast {
	class CAST_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnUpdate();
		void OnEvent(Event& e);

		void OnAttach();
		void OnDetach();

	private:
		float m_Time = 0.f;
	};
}
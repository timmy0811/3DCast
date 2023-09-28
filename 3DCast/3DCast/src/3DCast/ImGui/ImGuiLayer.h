#pragma once

#include "3DCast/Layer/Layer.h"

#include "3DCast/Event/KeyEvent.h"
#include "3DCast/Event/MouseEvent.h"
#include "3DCast/Event/ApplicationEvent.h"

namespace Cast {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.f;
	};
}
#pragma once

#include "3DCast/Event/Event.h"
#include "3DCast/Core/Timestep.h"
#include "3DCast/Core/Window.h"

namespace Cast
{
	static int s_LayerIdentifier = 0;

	class Layer
	{
	public:
		explicit Layer(std::string name = "Layer_" + std::to_string(s_LayerIdentifier));
		virtual ~Layer();

		inline void SetParentWindow(Ref<Window> window) { ParentWindow = window; }
		inline Ref<Window> GetParentWindow() { return ParentWindow; }

		virtual void OnAttach()
		{
		}

		virtual void OnDetach()
		{
		}

		virtual void OnUpdate(Timestep ts)
		{
		}

		virtual void OnImGuiRender()
		{
		}

		virtual void OnEvent(Event& e)
		{
		}

		[[nodiscard]] inline const std::string& getName() const { return debugName; }

	protected:
		std::string debugName;
		Ref<Window> ParentWindow;
	};
}

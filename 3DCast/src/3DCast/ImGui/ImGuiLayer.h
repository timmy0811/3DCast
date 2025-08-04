#pragma once

#include "3DCast/Layer/Layer.h"

namespace Cast
{
	class ImGuiLayer final : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;

		static void Begin();
		static void End();

	private:
		static void LoadFonts();

	private:
		float time = 0.f;
		inline static int LastWindowWidth = 0;
		inline static int LastWindowHeight = 0;
	};
}

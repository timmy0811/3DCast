#pragma once

namespace Runtime::GUI {
	class ComponentList {
	public:
		enum class ModalResult {
			Success,
			Cancel,
			None
		};

	public:
		ComponentList() = delete;

		static ModalResult OnImGuiRender();
		static void Reset();
		static bool* GetSelection() { return g_Selections; }

	private:
		inline static bool g_ShowAddComponentModal;
		inline static bool g_Selections[128];
		inline static char g_SearchBuffer[128];
	};
}
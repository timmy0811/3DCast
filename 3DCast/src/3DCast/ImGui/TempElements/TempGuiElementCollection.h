#pragma once

#include "3DCast/ImGui/TempElements/TempGuiElement.h"

#include <vector>

namespace Cast::GUI {
	class TempGuiElementCollection {
	public:
		TempGuiElementCollection() = delete;

		static void AddElement(TempGuiElement* element);

		static void OnImGuiRender();
		static void Clear();

	private:
		inline static std::vector<TempGuiElement*> Elements;
	};

	class TempGuiElementCollectionCleanup {
	public:
		~TempGuiElementCollectionCleanup() {
			TempGuiElementCollection::Clear();
		}
	};

	static TempGuiElementCollectionCleanup s_Cleanup;
}
#pragma once

#include "3DCast/ImGui/TempElements/TempGuiElement.h"

#include "3DCast/Misc/UID.h"

namespace Cast::GUI {
	class TempGuiElementCollection {
	public:
		TempGuiElementCollection() = delete;

		static UID AddElement(TempGuiElement* element);
		static void CloseElement(UID elementId);
		static void SetTextAttrib(UID elementId, const char* text);

		static void OnImGuiRender();
		static void Clear();

	private:
		inline static std::unordered_map<UID, TempGuiElement*> Elements;
	};

	class TempGuiElementCollectionCleanup {
	public:
		~TempGuiElementCollectionCleanup() {
			TempGuiElementCollection::Clear();
		}
	};

	static TempGuiElementCollectionCleanup s_Cleanup;
}

#include "castpch.h"
#include "TempGuiElementCollection.h"

#include "imgui.h"
#include "3DCast/Data/GlobalShared.h"
#include "3DCast/ImGui/ImGuiLayer.h"
#include "GLFW/glfw3.h"

Cast::UID Cast::GUI::TempGuiElementCollection::AddElement(TempGuiElement* element)
{
	UID id = UID::Create();
	Elements.insert({id, element});
	return id;
}

void Cast::GUI::TempGuiElementCollection::OnImGuiRender()
{
	for (auto it = Elements.begin(); it != Elements.end(); )
	{
		if (it->second->Render()) {
			delete it->second;
			it = Elements.erase(it);
		}
		else {
			++it;
		}
	}
}

void Cast::GUI::TempGuiElementCollection::CloseElement(const UID elementId)
{
	const auto it = Elements.find(elementId);
	if (it != Elements.end()) {
		delete it->second;
		Elements.erase(it);
	}
}

void Cast::GUI::TempGuiElementCollection::SetTextAttrib(const UID elementId, const char* text)
{
	const auto it = Elements.find(elementId);
	if (it != Elements.end()) {
		it->second->SetTextAttrib(text);
	}
}

void Cast::GUI::TempGuiElementCollection::Clear()
{
	for (const auto& [id, element] : Elements)
	{
		delete element;
	}

	Elements.clear();
}

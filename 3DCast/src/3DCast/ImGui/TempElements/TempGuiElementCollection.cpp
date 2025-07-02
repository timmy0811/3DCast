#include "castpch.h"
#include "TempGuiElementCollection.h"

void Cast::GUI::TempGuiElementCollection::AddElement(TempGuiElement* element)
{
	Elements.push_back(element);
}

void Cast::GUI::TempGuiElementCollection::OnImGuiRender()
{
	for (auto it = Elements.begin(); it != Elements.end(); )
	{
		if ((*it)->Render()) {
			delete* it;
			it = Elements.erase(it);
		}
		else {
			++it;
		}
	}
}

void Cast::GUI::TempGuiElementCollection::Clear()
{
	for (const auto& element : Elements)
	{
		delete element;
	}

	Elements.clear();
}
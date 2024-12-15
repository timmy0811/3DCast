#pragma once

#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Core.h"

namespace Runtime::GUI {
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Cast::Ref<Cast::Scene>& scene);

		void SetContext(const Cast::Ref<Cast::Scene>& scene);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Cast::Entity entity);
		void DrawComponents(Cast::Entity entity);

	private:
		Cast::Ref<Cast::Scene> Context;
		Cast::Entity SelectionContext;
	};
}
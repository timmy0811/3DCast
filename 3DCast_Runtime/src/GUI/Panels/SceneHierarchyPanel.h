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
		void DrawEntityNode(Cast::Ref<Cast::Entity> entity);
		void DrawComponents(Cast::Ref<Cast::Entity> entity);

		void DispatchComponent(int id);

	private:
		Cast::Ref<Cast::Scene> Context;
		Cast::Ref<Cast::Entity> SelectionContext;
	};
}
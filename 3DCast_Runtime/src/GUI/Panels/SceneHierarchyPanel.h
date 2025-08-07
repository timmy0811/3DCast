#pragma once

#include "3DCast/Scene/Scene.h"
#include "3DCast/Scene/Entity.h"
#include "3DCast/Core.h"

namespace Runtime::GUI
{
	class SceneHierarchyPanel
	{
	public:
		enum class Template
		{
			Cube, Plane,
			DirLight, PointLight, SpotLight,
			CustomMesh, Model,
			Camera,
			ParticleSystem, Decal
		};

	public:
		SceneHierarchyPanel() = default;
		explicit SceneHierarchyPanel(const Cast::Ref<Cast::Scene>& scene);

		void SetContext(const Cast::Ref<Cast::Scene>& scene);
		void SetSelectionContext(const Cast::Ref<Cast::Entity>& entity);

		void OnImGuiRender();

		static const char* TemplateToString(const Template templateName)
		{
			switch (templateName)
			{
			case Template::Cube: return "Cube";
			case Template::Plane: return "Plane";
			case Template::DirLight: return "Directional Light";
			case Template::PointLight: return "Point Light";
			case Template::SpotLight: return "Spot Light";
			case Template::CustomMesh: return "Custom Mesh";
			case Template::Model: return "Model";
			case Template::Camera: return "Camera";
			case Template::ParticleSystem: return "Particle System";
			case Template::Decal: return "Decal";
			}
			return "";
		}

	private:
		void DrawTemplateSelector(float width);
		void DrawEntityNode(Cast::Ref<Cast::Entity> entity);
		void DrawComponents(Cast::Ref<Cast::Entity> entity) const;
		void DispatchComponent(int id) const;
		void RemoveEntity();

		void CreateEntityFromTemplate(Template templateName);

	private:
		Cast::Ref<Cast::Scene> Context;
		Cast::Ref<Cast::Entity> SelectionContext;
	};
}

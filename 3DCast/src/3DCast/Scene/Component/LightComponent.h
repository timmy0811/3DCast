#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"

namespace Cast::Component {
	struct LightComponent : public Component
	{
		enum Type
		{
			Directional = 0,
			Point = 1,
			Spot = 2
			//Area = 3
		};

		glm::vec3 EntityPosition;
		glm::vec3 LastEntityPosition;

		Type LightType{ Type::Directional };
		AbstractLight* Light;
		Cast::Ref<Cast::Scene> Scene;

		size_t BufferPos;
		unsigned int BufferIndex;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const DirectionalLight& light, Cast::Ref<Cast::Scene> scene)
			: Light(new DirectionalLight(light)), Scene(scene)
		{
			LightType = Type::Directional;
			SetupDirLight();
		}

		LightComponent(const SpotLight& light, Cast::Ref<Cast::Scene> scene)
			: Light(new SpotLight(light)), Scene(scene)
		{
			LightType = Type::Spot;
			SetupSpotLight();
		}

		LightComponent(const PointLight& light, Cast::Ref<Cast::Scene> scene)
			: Light(new PointLight(light)), Scene(scene)
		{
			LightType = Type::Point;
			SetupPointLight();
		}

		~LightComponent() {
			delete Light;
		}

		void Reallocate() {
			switch (LightType) {
			case Type::Directional:
				SetupDirLight();
				break;
			case Type::Point:
				SetupPointLight();
				break;
			case Type::Spot:
				SetupSpotLight();
				break;
			}
		}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Type:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

				Type oldType = LightType;
				bool changed = ImGui::Combo("##LightType", (int*)&LightType, "Directional\0Point\0Spot\0");

				if (!Light) return;

				PointLight* pointLight;
				SpotLight* spotLight;
				DirectionalLight* dirLight;

				switch (LightType) {
				case Type::Directional:
					if (changed) {
						Scene->ReallocateLights(oldType);
						delete Light;
						Light = new DirectionalLight();
						SetupDirLight();
					}

					dirLight = (DirectionalLight*)Light;
					changed = false;

					ImGui::Text("Direction:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat3("##Direction", &dirLight->direction.x, 0.1f);

					ImGui::Text("Ambient:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Ambient", &dirLight->ambient.r);

					ImGui::Text("Diffuse:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Diffuse", &dirLight->diffuse.x);

					ImGui::Text("Specular:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Specular", &dirLight->specular.x);

					if (changed) {
						Scene->GetDirLightsBuffer()->AddData(dirLight, sizeof(DirectionalLight), (int)BufferPos);
					}

					break;
				case Type::Point:
					if (changed) {
						Scene->ReallocateLights(oldType);
						delete Light;
						Light = new PointLight();
						SetupPointLight();
					}

					pointLight = (PointLight*)Light;
					changed = false;

					ImGui::Text("Ambient:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Ambient", &pointLight->ambient.x);

					ImGui::Text("Diffuse:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Diffuse", &pointLight->diffuse.x);

					ImGui::Text("Specular:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Specular", &pointLight->specular.x);

					ImGui::Text("Constant Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Constant", &pointLight->constant, 0.1f);

					ImGui::Text("Linear Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Linear", &pointLight->linear, 0.01f);

					ImGui::Text("Quadratic Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Quadratic", &pointLight->quadratic, 0.01f);

					changed |= EntityPosition != LastEntityPosition;
					LastEntityPosition = EntityPosition;

					if (changed) {
						pointLight->position = EntityPosition;
						Scene->GetPointLightsBuffer()->AddData(pointLight, sizeof(PointLight), (int)BufferPos);
					}

					break;
				case Type::Spot:
					if (changed) {
						Scene->ReallocateLights(oldType);
						delete Light;
						Light = new SpotLight();
						SetupSpotLight();
					}

					spotLight = (SpotLight*)Light;
					changed = false;

					ImGui::Text("Direction:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat3("##Direction", &spotLight->direction.x, 0.1f);

					ImGui::Text("Ambient:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Ambient", &spotLight->ambient.x);

					ImGui::Text("Diffuse:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Diffuse", &spotLight->diffuse.x);

					ImGui::Text("Specular:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::ColorEdit3("##Specular", &spotLight->specular.x);

					ImGui::Text("Constant Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Constant", &spotLight->constant, 0.002f);

					ImGui::Text("Linear Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Linear", &spotLight->linear, 0.002f);

					ImGui::Text("Quadratic Factor:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Quadratic", &spotLight->quadratic, 0.001f);

					static float cutOff = glm::degrees(glm::acos(spotLight->cutOff));
					ImGui::Text("Outer Cutoff:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Outer Cutoff", &cutOff, 0.2f);
					spotLight->cutOff = glm::cos(glm::radians(cutOff));

					static float outerCutOff = glm::degrees(glm::acos(spotLight->outerCutOff));
					ImGui::Text("Cutoff:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Cutoff Out", &outerCutOff, 0.2f);
					spotLight->outerCutOff = glm::cos(glm::radians(outerCutOff));

					changed |= EntityPosition != LastEntityPosition;
					LastEntityPosition = EntityPosition;

					if (changed) {
						spotLight->position = EntityPosition;
						Scene->GetSpotLightsBuffer()->AddData(spotLight, sizeof(SpotLight), (int)BufferPos);
					}

					break;
				}
			}
		}
	private:
		void SetupDirLight() {
			BufferPos = Scene->GetDirLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(DirectionalLight));
			Scene->GetDirLightsBuffer()->AddData(Light, sizeof(DirectionalLight));
		}

		void SetupSpotLight() {
			BufferPos = Scene->GetSpotLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(SpotLight));
			Scene->GetSpotLightsBuffer()->AddData(Light, sizeof(SpotLight));
		}

		void SetupPointLight() {
			BufferPos = Scene->GetPointLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(PointLight));
			Scene->GetPointLightsBuffer()->AddData(Light, sizeof(PointLight));
		}
	};
}
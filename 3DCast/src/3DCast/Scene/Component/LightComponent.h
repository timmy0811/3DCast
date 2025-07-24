#pragma once

#include "3DCast/Scene/Component/AbstractComponent.h"
#include "3DCast/Data/ShaderDataObjects/Light.h"

#include <imgui.h>


namespace Cast::Component
{
	struct LightComponent final : public Component
	{
		enum Type
		{
			Directional = 0,
			Point = 1,
			Spot = 2
		};

#pragma region DATA
		glm::vec3 EntityPosition{};
		glm::vec3 LastEntityPosition{};

		Type LightType{Directional};
		AbstractLight* Light{};
		Ref<Scene> SceneInstance;

		size_t BufferPos = 0;
		unsigned int BufferIndex = 0;
		bool IsEnvironmentLight = false;
#pragma endregion

#pragma region CONSTRUCTOR
		explicit LightComponent(const bool IsEnvironmentLight = false)
			:IsEnvironmentLight(IsEnvironmentLight)
		{}
		LightComponent(const LightComponent&) = default;

		LightComponent(const DirectionalLight& light, Ref<Scene> scene)
			: Light(new DirectionalLight(light)), SceneInstance(scene)
		{
			LightType = Directional;
			SetupDirLight();
		}

		LightComponent(const SpotLight& light, Ref<Scene> scene)
			: Light(new SpotLight(light)), SceneInstance(scene)
		{
			LightType = Spot;
			SetupSpotLight();
		}

		LightComponent(const PointLight& light, Ref<Scene> scene)
			: Light(new PointLight(light)), SceneInstance(scene)
		{
			LightType = Point;
			SetupPointLight();
		}

		~LightComponent()
		{
			SafeCleanup();
		}
#pragma endregion

#pragma region UTILITY
		void Reallocate()
		{
			switch (LightType)
			{
			case Directional:
				SetupDirLight();
				break;
			case Point:
				SetupPointLight();
				break;
			case Type::Spot:
				SetupSpotLight();
				break;
			}
		}

		void UpdateLightData() const
		{
			switch (LightType)
			{
			case Directional:
				SceneInstance->GetDirLightsBuffer()->AddData((DirectionalLight*)Light, sizeof(DirectionalLight), (int)BufferPos);
				break;
			case Point:
				SceneInstance->GetPointLightsBuffer()->AddData((PointLight*)Light, sizeof(PointLight), (int)BufferPos);
				break;
			case Type::Spot:
				SceneInstance->GetSpotLightsBuffer()->AddData((SpotLight*)Light, sizeof(SpotLight), (int)BufferPos);
				break;
			}

		}

		void SafeCleanup()
		{
			if (Light && SceneInstance)
			{
				switch (LightType)
				{
				case Directional:
					SceneInstance->ReallocateLights(Directional);
					break;
				case Point:
					SceneInstance->ReallocateLights(Point);
					break;
				case Spot:
					SceneInstance->ReallocateLights(Spot);
					break;
				}

				delete Light;
				Light = nullptr;
			}
		}

	private:
		void SetupDirLight()
		{
			BufferPos = SceneInstance->GetDirLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(DirectionalLight));
			SceneInstance->GetDirLightsBuffer()->AddData(Light, sizeof(DirectionalLight));
		}

		void SetupSpotLight()
		{
			BufferPos = SceneInstance->GetSpotLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(SpotLight));
			SceneInstance->GetSpotLightsBuffer()->AddData(Light, sizeof(SpotLight));
		}

		void SetupPointLight()
		{
			BufferPos = SceneInstance->GetPointLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(PointLight));
			SceneInstance->GetPointLightsBuffer()->AddData(Light, sizeof(PointLight));
		}
#pragma endregion

#pragma region OVERRIDE

	public:
		static inline Cast::Component::Type GetType() { return Cast::Component::Type::Light; }
		static inline std::string GetName() { return "Light"; }

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader(
				"Light", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 80.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton("Remove##Light"))
				return {UIResponse::Code::Remove, Cast::Component::Type::Light};

			if (isOpen)
			{
				ImGui::Text("Type:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

				ImGui::BeginDisabled(IsEnvironmentLight);
				const Type oldType = LightType;
				bool changed = ImGui::Combo("##LightType", (int*)&LightType, "Directional\0Point\0Spot\0");
				ImGui::EndDisabled();

				if (!Light) return {};

				PointLight* pointLight;
				SpotLight* spotLight;
				DirectionalLight* dirLight;

				switch (LightType)
				{
				case Type::Directional:
					if (changed)
					{
						// Prevent double free by not manually deleting Light
						SceneInstance->ReallocateLights(oldType);
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

					if (changed)
					{
						SceneInstance->GetDirLightsBuffer()->
						               AddData(dirLight, sizeof(DirectionalLight), (int)BufferPos);
					}

					break;
				case Type::Point:
					if (changed)
					{
						SceneInstance->ReallocateLights(oldType);
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

					if (changed)
					{
						pointLight->position = EntityPosition;
						SceneInstance->GetPointLightsBuffer()->AddData(pointLight, sizeof(PointLight), (int)BufferPos);
					}

					break;
				case Type::Spot:
					if (changed)
					{
						SceneInstance->ReallocateLights(oldType);
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

					if (changed)
					{
						spotLight->position = EntityPosition;
						SceneInstance->GetSpotLightsBuffer()->AddData(spotLight, sizeof(SpotLight), (int)BufferPos);
					}

					break;
				}
			}

			return {};
		}
#pragma endregion
	};
}

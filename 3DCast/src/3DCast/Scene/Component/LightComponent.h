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
		AbstractLightShaderObject* Light{};
		Scene* SceneInstance = nullptr;

		size_t BufferPos = 0;
		unsigned int BufferIndex = 0;
		bool IsEnvironmentLight = false;
#pragma endregion

#pragma region CONSTRUCTOR
		explicit LightComponent(const bool IsEnvironmentLight = false)
			:IsEnvironmentLight(IsEnvironmentLight)
		{}
		LightComponent(const LightComponent&) = default;

		LightComponent(const Type type, Scene* scene)
			: LightType(type), SceneInstance(scene)
		{
			switch (type)
			{
				case Directional: Light = new DirectionalLightShaderObject(); SetupDirLight(); break;
				case Spot: Light = new SpotLightShaderObject(); SetupSpotLight(); break;
				case Point: Light = new PointLightShaderObject(); SetupPointLight(); break;
			}
		}

		LightComponent(const DirectionalLightShaderObject& light, Scene* scene)
			: Light(new DirectionalLightShaderObject(light)), SceneInstance(scene)
		{
			LightType = Directional;
			SetupDirLight();
		}

		LightComponent(const SpotLightShaderObject& light, Scene* scene)
			: Light(new SpotLightShaderObject(light)), SceneInstance(scene)
		{
			LightType = Spot;
			SetupSpotLight();
		}

		LightComponent(const PointLightShaderObject& light, Scene* scene)
			: Light(new PointLightShaderObject(light)), SceneInstance(scene)
		{
			LightType = Point;
			SetupPointLight();
		}

		~LightComponent() override
		{
			SafeCleanup();
		}

		// Move constructor
		LightComponent(LightComponent&& other) noexcept
			: EntityPosition(other.EntityPosition),
			  LastEntityPosition(other.LastEntityPosition),
			  LightType(other.LightType),
			  Light(other.Light),
			  SceneInstance(std::move(other.SceneInstance)),
			  BufferPos(other.BufferPos),
			  BufferIndex(other.BufferIndex),
			  IsEnvironmentLight(other.IsEnvironmentLight)
		{
			other.Light = nullptr;
			other.SceneInstance = nullptr;
		}

		// Move assignment operator
		LightComponent& operator=(LightComponent&& other) noexcept
		{
			if (this != &other)
			{
				SafeCleanup();

				// Transfer all data
				EntityPosition = other.EntityPosition;
				LastEntityPosition = other.LastEntityPosition;
				LightType = other.LightType;
				Light = other.Light;
				SceneInstance = std::move(other.SceneInstance);
				BufferPos = other.BufferPos;
				BufferIndex = other.BufferIndex;
				IsEnvironmentLight = other.IsEnvironmentLight;

				other.Light = nullptr;
				other.SceneInstance = nullptr;
			}
			return *this;
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
				SceneInstance->GetDirLightsBuffer()->AddData((DirectionalLightShaderObject*)Light, sizeof(DirectionalLightShaderObject), (int)BufferPos);
				break;
			case Point:
				SceneInstance->GetPointLightsBuffer()->AddData((PointLightShaderObject*)Light, sizeof(PointLightShaderObject), (int)BufferPos);
				break;
			case Type::Spot:
				SceneInstance->GetSpotLightsBuffer()->AddData((SpotLightShaderObject*)Light, sizeof(SpotLightShaderObject), (int)BufferPos);
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
			BufferIndex = (unsigned int)(BufferPos / sizeof(DirectionalLightShaderObject));
			SceneInstance->GetDirLightsBuffer()->AddData(Light, sizeof(DirectionalLightShaderObject));
		}

		void SetupSpotLight()
		{
			BufferPos = SceneInstance->GetSpotLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(SpotLightShaderObject));
			SceneInstance->GetSpotLightsBuffer()->AddData(Light, sizeof(SpotLightShaderObject));
		}

		void SetupPointLight()
		{
			BufferPos = SceneInstance->GetPointLightsBuffer()->GetSize();
			BufferIndex = (unsigned int)(BufferPos / sizeof(PointLightShaderObject));
			SceneInstance->GetPointLightsBuffer()->AddData(Light, sizeof(PointLightShaderObject));
		}
#pragma endregion

#pragma region OVERRIDE

	public:
		static inline Cast::Component::Type GetType() { return Cast::Component::Type::Light; }
		static inline std::string GetName() { return "Light"; }

		UIResponse OnImGuiRender() override
		{
			const bool isOpen = ImGui::CollapsingHeader(
				ICON_FA_LIGHTBULB "  Light", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
			ImGui::SameLine();

			const float xOffset = ImGui::GetContentRegionAvail().x - 30.0f;
			if (xOffset > 0.0f)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
			}

			if (ImGui::SmallButton(ICON_FA_TRASH_CAN "##Light"))
				return {UIResponse::Code::Remove, Cast::Component::Type::Light};

			if (isOpen)
			{
				ImGui::Text("Type:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);

				ImGui::BeginDisabled(IsEnvironmentLight);
				const Type oldType = LightType;
				bool changed = ImGui::Combo("##LightType", (int*)&LightType, "Directional\0Point\0Spot\0");
				ImGui::EndDisabled();

				if (!Light) return {};

				PointLightShaderObject* pointLight;
				SpotLightShaderObject* spotLight;
				DirectionalLightShaderObject* dirLight;

				switch (LightType)
				{
				case Type::Directional:
					if (changed)
					{
						// Prevent double free by not manually deleting Light
						SceneInstance->ReallocateLights(oldType);
						delete Light;
						Light = new DirectionalLightShaderObject();
						SetupDirLight();
					}

					dirLight = (DirectionalLightShaderObject*)Light;
					changed = false;

					ImGui::Text("Direction");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat3("##Direction", &dirLight->direction.x, 0.1f);

					ImGui::Separator();

					ImGui::Text("Ambient");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Ambient", &dirLight->ambient.r);

					ImGui::Text("Diffuse");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Diffuse", &dirLight->diffuse.x);

					ImGui::Text("Specular");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Specular", &dirLight->specular.x);

					if (changed)
					{
						SceneInstance->GetDirLightsBuffer()->
						               AddData(dirLight, sizeof(DirectionalLightShaderObject), (int)BufferPos);
					}

					break;
				case Type::Point:
					if (changed)
					{
						SceneInstance->ReallocateLights(oldType);
						delete Light;
						Light = new PointLightShaderObject();
						SetupPointLight();
					}

					pointLight = (PointLightShaderObject*)Light;
					changed = false;

					ImGui::Text("Ambient");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Ambient", &pointLight->ambient.x);

					ImGui::Text("Diffuse");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Diffuse", &pointLight->diffuse.x);

					ImGui::Text("Specular");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Specular", &pointLight->specular.x);

					ImGui::Separator();

					ImGui::Text("Constant Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Constant", &pointLight->constant, 0.1f);

					ImGui::Text("Linear Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Linear", &pointLight->linear, 0.01f);

					ImGui::Text("Quadratic Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Quadratic", &pointLight->quadratic, 0.01f);

					changed |= EntityPosition != LastEntityPosition;
					LastEntityPosition = EntityPosition;

					if (changed)
					{
						pointLight->position = EntityPosition;
						SceneInstance->GetPointLightsBuffer()->AddData(pointLight, sizeof(PointLightShaderObject), (int)BufferPos);
					}

					break;
				case Type::Spot:
					if (changed)
					{
						SceneInstance->ReallocateLights(oldType);
						delete Light;
						Light = new SpotLightShaderObject();
						SetupSpotLight();
					}

					spotLight = (SpotLightShaderObject*)Light;
					changed = false;

					ImGui::Text("Direction");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat3("##Direction", &spotLight->direction.x, 0.1f);

					ImGui::Separator();

					ImGui::Text("Ambient");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Ambient", &spotLight->ambient.x);

					ImGui::Text("Diffuse");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Diffuse", &spotLight->diffuse.x);

					ImGui::Text("Specular");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::ColorEdit3("##Specular", &spotLight->specular.x);

					ImGui::Separator();

					ImGui::Text("Constant Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Constant", &spotLight->constant, 0.002f);

					ImGui::Text("Linear Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Linear", &spotLight->linear, 0.002f);

					ImGui::Text("Quadratic Factor");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Quadratic", &spotLight->quadratic, 0.001f);

					static float cutOff = glm::degrees(glm::acos(spotLight->cutOff));
					ImGui::Text("Outer Cutoff");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Outer Cutoff", &cutOff, 0.2f);
					spotLight->cutOff = glm::cos(glm::radians(cutOff));

					static float outerCutOff = glm::degrees(glm::acos(spotLight->outerCutOff));
					ImGui::Text("Cutoff");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET_1);
					changed |= ImGui::DragFloat("##Cutoff Out", &outerCutOff, 0.2f);
					spotLight->outerCutOff = glm::cos(glm::radians(outerCutOff));

					changed |= EntityPosition != LastEntityPosition;
					LastEntityPosition = EntityPosition;

					if (changed)
					{
						spotLight->position = EntityPosition;
						SceneInstance->GetSpotLightsBuffer()->AddData(spotLight, sizeof(SpotLightShaderObject), (int)BufferPos);
					}

					break;
				}

				ImGui::Dummy(ImVec2(0.f, DUMMYSPACE_AFTER_COMPONENT));
			}

			return {};
		}
#pragma endregion
	};
}

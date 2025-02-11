#pragma once

#include "3DCast/Renderer/Camera/Camera.h"
#include "3DCast/Scene/DataObjects/ShaderDataObjects.h"
#include "3DCast/Scene/Scene.h"

#include <API/texture/Texture.h>
#include <API/core/IndexBuffer.h>
#include <API/core/VertexArray.h>
#include <API/core/VertexBufferLayout.h>
#include <API/core/Buffer.h>
#include <API/core/Shader.h>

#include <Vendor/glm/glm.hpp>
#include <Vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>

#include <string>

#include "imgui.h"
#include "3DCast/Misc/Helper.h"

#define SAMELINE_WIDGET_OFFSET ImGui::GetWindowWidth() / 3

namespace Cast::Component {
	struct Component
	{
		virtual ~Component() = default;
		virtual void OnImGuiRender() {};
		virtual void Print() {};
	};

	struct TagComponent : public Component
	{
		std::string Tag;

		TagComponent() { Tag.reserve(MAX_TAG_SIZE); };
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) {
			Tag = tag.empty() ? "Untagged" : tag;
			Tag.reserve(MAX_TAG_SIZE);
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Text("Name:");
			ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
			ImGui::InputText("##Tag", Tag.data(), MAX_TAG_SIZE);
		}

	private:
		constexpr static size_t MAX_TAG_SIZE = 96;
	};

	struct TransformComponent : public Component
	{
		bool isValid;
		size_t bufferPosition;
		unsigned int bufferIndex;
		glm::mat4 Transform{ 1.0f };
		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };
		Cast::Ref<API::Core::Buffer> transformRegistry;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		}

		bool Register(Cast::Ref<API::Core::Buffer> transformRegistry)
		{
			this->transformRegistry = transformRegistry;

			if (transformRegistry) {
				bufferPosition = transformRegistry->GetSize();
				transformRegistry->AddData(&Transform, sizeof(glm::mat4));
				bufferIndex = (unsigned int)(bufferPosition / sizeof(glm::mat4));
				isValid = true;
				return true;
			}

			return false;
		}

		inline glm::mat4 GetTransform() const
		{
			return Transform;
		}

		inline glm::vec3 GetTranslation() const
		{
			return glm::vec3(Transform[3]);
		}

		glm::vec3 GetScale() const
		{
			return glm::vec3(glm::length(glm::vec3(Transform[0])),
				glm::length(glm::vec3(Transform[1])),
				glm::length(glm::vec3(Transform[2])));
		}

		glm::vec3 GetRotation() const
		{
			glm::vec3 scale = GetScale();

			glm::mat3 rotationMatrix = glm::mat3(
				Transform[0] / scale.x,
				Transform[1] / scale.y,
				Transform[2] / scale.z
			);

			return glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));
		}

		void UpdateTransformMatrix()
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) *
				glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)) *
				glm::scale(glm::mat4(1.0f), scale);
		}

		virtual void OnImGuiRender() override
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Translation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Translation", &translation.x, 0.1f))
					UpdateTransformMatrix();

				ImGui::Text("Scale:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Scale", &scale.x, 0.1f))
					UpdateTransformMatrix();

				ImGui::Text("Rotation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				if (ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f))
					UpdateTransformMatrix();

				if (transformRegistry)
					transformRegistry->AddData(&Transform, sizeof(glm::mat4), (int)bufferPosition);
			}
		}
	};

	struct LightComponent : public Component
	{
		enum Type
		{
			Directional = 0,
			Point = 1,
			Spot = 2
			//Area = 3
		};

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

					if (changed) Scene->GetDirLightsBuffer()->AddData(dirLight, sizeof(DirectionalLight), (int)BufferPos);

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

					ImGui::Text("Position:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat3("##Position", &pointLight->position.x, 0.1f);

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

					if (changed) Scene->GetPointLightsBuffer()->AddData(pointLight, sizeof(PointLight), (int)BufferPos);

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

					ImGui::Text("Position:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat3("##Position", &spotLight->position.x, 0.1f);

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

					static float cutOff = glm::degrees(spotLight->cutOff);
					ImGui::Text("Outer Cutoff:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Outer Cutoff", &cutOff, 0.2f);
					spotLight->cutOff = glm::cos(glm::radians(cutOff));

					static float outerCutOff = glm::degrees(spotLight->outerCutOff);
					ImGui::Text("Cutoff:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					changed |= ImGui::DragFloat("##Cutoff Out", &outerCutOff, 0.2f);
					spotLight->outerCutOff = glm::cos(glm::radians(outerCutOff));

					if (changed) Scene->GetSpotLightsBuffer()->AddData(spotLight, sizeof(SpotLight), (int)BufferPos);

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

	struct MeshComponent : public Component
	{
		std::string MeshPath;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: MeshPath(path) {}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Path:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), MeshPath.c_str());

				if (ImGui::InputText("##MeshPath", buffer, sizeof(buffer)))
				{
					MeshPath = std::string(buffer);
				}
			}
		}
	};

	struct CustomMeshComponent : public Component
	{
		Cast::Ref<API::Core::VertexBuffer> vb;
		Cast::Ref<API::Core::IndexBuffer> ib;
		Cast::Ref<API::Core::VertexBufferLayout> vbLayout;
		Cast::Ref<API::Core::VertexArray> va;

		CustomMeshComponent() = default;
		CustomMeshComponent(const CustomMeshComponent&) = default;
		CustomMeshComponent(int vert_count, size_t stride) {
			vb.reset(API::Core::VertexBuffer::Create(vert_count, stride));
			vbLayout.reset(API::Core::VertexBufferLayout::Create());
			va.reset(API::Core::VertexArray::Create());

			va->AddBuffer(*vb, *vbLayout);
			va->SetVBCount(vert_count);
		}

		void AddIndices(unsigned int* indices, unsigned int count) {
			ib.reset(API::Core::IndexBuffer::Create(indices, count));
		}
	};

	struct ShaderComponent : public Component
	{
		std::string Identifier;
		Cast::Ref<API::Core::Shader> Shader;

		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
		ShaderComponent(const std::string& path_vertex, const std::string& path_frag, const std::string& id)
			: Identifier(id) {
			Shader.reset(API::Core::Shader::Create(path_vertex, path_frag));
		}
	};

	struct CameraComponent : public Component
	{
		Renderer::Camera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Renderer::Camera& camera)
			: Camera(camera) {}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3 position = Camera.GetPosition();
				glm::vec3 rotation = Camera.GetRotation();

				ImGui::Text("Position:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				ImGui::DragFloat3("##Position", &position.x, 0.1f);

				ImGui::Text("Rotation:");
				ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
				ImGui::DragFloat3("##Rotation", &rotation.x, 0.1f);

				Camera.SetPosition(position);
				Camera.SetRotation(rotation);
			}
		}
	};

	struct RasterizableComponent : public Component
	{
		bool Renderable{ true };

		RasterizableComponent() = default;
		RasterizableComponent(const RasterizableComponent&) = default;
		RasterizableComponent(bool enable)
			: Renderable(enable) {}
	};

	struct MaterialComponent : public Component
	{
		unsigned int Shader;

		/* Bitmask:
		0: Diffuse
		1: Specular
		2: Ambient
		3: Emmissive
		4: Shine
		5: Opacity
		6: Reflectance
		*/
		uint16_t textureEnabled;
		bool isDeffered = true;

		glm::vec3 ambient{ 1.f };
		glm::vec3 diffuse{ 1.f };
		glm::vec3 specular{ 1.f };
		float shine;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(const unsigned int shaderId)
			: Shader(shaderId) {}

		virtual void OnImGuiRender() override {
			if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
				// Ambient
				ImGui::Checkbox("Use deferred Rendering", &isDeffered);

				bool textureBitEnabled = Helper::isBitSet(textureEnabled, 2);

				if (!textureBitEnabled) {
					ImGui::Text("Ambient:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					ImGui::DragFloat3("##Ambient", &ambient.x, 1.f);
				}
				else {
					ImGui::Button("Load Ambient Texture");
				}

				ImGui::Checkbox("Use Ambient-Map", &textureBitEnabled);
				Helper::setBit(textureEnabled, 2, textureBitEnabled);

				ImGui::Separator();

				// Diffuse
				textureBitEnabled = Helper::isBitSet(textureEnabled, 0);

				if (!textureBitEnabled) {
					ImGui::Text("Diffuse:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					ImGui::DragFloat3("##Diffuse", &diffuse.x, 1.f);
				}
				else {
					ImGui::Button("Load Diffuse Texture");
				}

				ImGui::Checkbox("Use Diffuse-Map", &textureBitEnabled);
				Helper::setBit(textureEnabled, 0, textureBitEnabled);

				ImGui::Separator();

				// Specular
				textureBitEnabled = Helper::isBitSet(textureEnabled, 1);

				if (!textureBitEnabled) {
					ImGui::Text("Specular:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					ImGui::DragFloat3("##Specular", &specular.x, 1.f);
				}
				else {
					ImGui::Button("Load Specular Texture");
				}

				ImGui::Checkbox("Use Specular-Map", &textureBitEnabled);
				Helper::setBit(textureEnabled, 1, textureBitEnabled);

				ImGui::Separator();

				// Shine
				textureBitEnabled = Helper::isBitSet(textureEnabled, 4);

				if (!textureBitEnabled) {
					ImGui::Text("Shininess:");
					ImGui::SameLine(SAMELINE_WIDGET_OFFSET);
					ImGui::DragFloat("##Shininess", &shine, 0.1f);
				}
				else {
					ImGui::Button("Load Shine Texture");
				}

				ImGui::Checkbox("Use Shine-Map", &textureBitEnabled);
				Helper::setBit(textureEnabled, 4, textureBitEnabled);

				ImGui::Separator();
			}
		}
	};

	struct PBRMaterialComponent : public Component
	{
		bool temp;

		PBRMaterialComponent() = default;
		PBRMaterialComponent(const PBRMaterialComponent&) = default;
	};

	struct PBRComponent : public Component
	{
		bool Renderable{ true };

		PBRComponent() = default;
		PBRComponent(const PBRComponent&) = default;
		PBRComponent(bool enable)
			: Renderable(enable) {}
	};

	struct TextureComponent : public Component
	{
		Cast::Ref<API::Texture::Texture> texture;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
		TextureComponent(const std::string& path, const bool flipUV = false) {
			texture.reset(API::Texture::Texture::Create(path, flipUV));
		}
	};
}
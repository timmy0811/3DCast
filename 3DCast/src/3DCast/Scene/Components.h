#pragma once

#include "3DCast/Renderer/Camera/Camera.h"

#include <API/texture/Texture.h>
#include <API/core/IndexBuffer.h>
#include <API/core/VertexArray.h>
#include <API/core/VertexBufferLayout.h>
#include <API/core/Shader.h>

#include <Vendor/glm/glm.hpp>
#include <Vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtx/euler_angles.hpp>

#include <string>

namespace Cast::Component {
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) {
			Tag = tag.empty() ? "Untagged" : tag;
		}
	};

	struct TransformComponent
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
		{
			Transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		}

		inline glm::mat4 GetTransform() const
		{
			return Transform;
		}

		inline glm::vec3 GetTranslation() const
		{
			return glm::vec3(Transform[3]);
		}

		inline glm::vec3 GetScale() const
		{
			return glm::vec3(glm::length(Transform[0]), glm::length(Transform[1]), glm::length(Transform[2]));
		}

		inline glm::vec3 GetRotation() const
		{
			const glm::vec3 scale = GetScale();

			glm::highp_mat3 rotationMatrix = glm::mat3(
				Transform[0] / scale.x,
				Transform[1] / scale.y,
				Transform[2] / scale.z
			);

			glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(rotationMatrix));

			return glm::degrees(eulerAngles);
		}
	};

	struct LightComponent
	{
		enum class Type
		{
			Directional = 0,
			Point = 1,
			Spot = 2,
			Area = 3
		};

		glm::vec3 Color{ 1.0f };
		float Intensity{ 1.0f };
		Type LightType{ Type::Directional };

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(Type type, const glm::vec3& color, float intensity)
			: LightType(type), Color(color), Intensity(intensity) {}
	};

	struct MeshComponent
	{
		std::string MeshPath;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: MeshPath(path) {}
	};

	struct CustomMeshComponent
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

	struct MaterialComponent
	{
		std::string MaterialPath;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(const std::string& path)
			: MaterialPath(path) {}
	};

	struct ShaderComponent
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

	struct CameraComponent
	{
		Renderer::Camera Camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Renderer::Camera& camera)
			: Camera(camera) {}
	};

	struct RasterizableComponent
	{
		bool Renderable{ true };

		RasterizableComponent() = default;
		RasterizableComponent(const RasterizableComponent&) = default;
		RasterizableComponent(bool enable)
			: Renderable(enable) {}
	};

	struct PBRComponent
	{
		bool Renderable{ true };

		PBRComponent() = default;
		PBRComponent(const PBRComponent&) = default;
		PBRComponent(bool enable)
			: Renderable(enable) {}
	};

	struct TextureComponent
	{
		Cast::Ref<API::Texture::Texture> texture;

		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
		TextureComponent(const std::string& path, const bool flipUV = false) {
			texture.reset(API::Texture::Texture::Create(path, flipUV));
		}
	};
}
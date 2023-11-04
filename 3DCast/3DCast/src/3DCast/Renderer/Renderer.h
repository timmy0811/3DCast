#pragma once

#include "API/core/RendererCommand.h"
#include "API/core/Shader.h"

#include "Camera/Camera.h"

namespace Cast::Renderer{
	class RendererContext {
	public:
		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<API::Core::VertexArray>& va, const std::shared_ptr<API::Core::Shader>& shader);
		static void Submit(const std::shared_ptr<API::Core::VertexArray>& va, const std::shared_ptr<API::Core::IndexBuffer>& ib, const std::shared_ptr<API::Core::Shader>& shader);

		inline static API::Core::RendererAPI::API_ENUM GetAPI() { return API::Core::RendererAPI::GetAPI(); }

	public:
		static inline int FramebufferOriginId;

	private:
		struct SceneData {
			glm::mat4 ViewProjectionMat;
		};

		static SceneData* m_SceneData;
	};
}

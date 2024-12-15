#pragma once

#include "API/core/RendererCommand.h"
#include "API/core/Shader.h"

#include "Camera/Camera.h"

namespace Cast::Renderer {
	class RendererContext {
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::Shader>& shader, const glm::mat4& transform = glm::mat4(1.f));
		static void Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::IndexBuffer>& ib, const Ref<API::Core::Shader>& shader,
			const glm::mat4& transform = glm::mat4(1.f));

		inline static API::Core::RendererAPI::API_ENUM GetAPI() { return API::Core::RendererAPI::GetAPI(); }

	public:
		static inline int framebufferOriginId;

	private:
		struct SceneDataCache {
			glm::mat4 viewProjectionMat;
		};

		static SceneDataCache* sceneDataCache;
	};
}

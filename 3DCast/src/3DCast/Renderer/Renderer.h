#pragma once

#include "API/core/RendererCommand.h"
#include "API/core/Buffer.h"
#include "API/core/Shader.h"

#include "Camera/Camera.h"

namespace Cast::Renderer {
	class RendererContext {
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::Shader> shader);
		static void Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::IndexBuffer> ib, Ref<API::Core::Shader> shader);
		static void Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::Buffer> ib, Ref<API::Core::Shader> shader);

		inline static API::Core::RendererAPI::API_ENUM GetAPI() { return API::Core::RendererAPI::GetAPI(); }

	public:
		static inline int framebufferOriginId;

	private:
		struct SceneDataCache {
			glm::mat4 viewProjectionMat;
			glm::mat4 viewMat;
			glm::mat4 projectionMat;
		};

		static SceneDataCache* sceneDataCache;
	};
}

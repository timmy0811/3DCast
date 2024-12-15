#include "castpch.h"
#include "Renderer.h"

namespace Cast::Renderer {
	RendererContext::SceneDataCache* RendererContext::sceneDataCache = new SceneDataCache;
}

void Cast::Renderer::RendererContext::Init()
{
	// API::Core::RenderCommand::Init(); Obsolete
}

void Cast::Renderer::RendererContext::OnWindowResize(uint32_t width, uint32_t height)
{
	API::Core::RenderCommand::SetViewport(0, 0, width, height);
}

void Cast::Renderer::RendererContext::BeginScene(Camera& camera)
{
	sceneDataCache->viewProjectionMat = camera.GetViewProjectionMat();
}

void Cast::Renderer::RendererContext::EndScene()
{
}

void Cast::Renderer::RendererContext::Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::Shader>& shader, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", sceneDataCache->viewProjectionMat);
	shader->SetUniformMat4f("u_Transform", transform);

	va->Bind();
	API::Core::RenderCommand::Draw(va, va->GetVBCount());
}

void Cast::Renderer::RendererContext::Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::IndexBuffer>& ib, const Ref<API::Core::Shader>& shader, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", sceneDataCache->viewProjectionMat);
	shader->SetUniformMat4f("u_Transform", transform);

	va->Bind();
	ib->Bind();
	API::Core::RenderCommand::DrawIndexed(va, ib);
}
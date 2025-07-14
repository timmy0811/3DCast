#include "castpch.h"
#include "Renderer.h"

namespace Cast::Renderer {
	RendererContext::SceneDataCache* RendererContext::sceneDataCache = new SceneDataCache;
}

void Cast::Renderer::RendererContext::Init()
{
	// API::Core::RenderCommand::Init(); Obsolete
}

void Cast::Renderer::RendererContext::OnWindowResize(const uint32_t width, const uint32_t height)
{
	API::Core::RenderCommand::SetViewport(0, 0, width, height);
}

void Cast::Renderer::RendererContext::BeginScene(const Camera& camera)
{
	// sceneDataCache->viewProjectionMat = camera.GetViewProjectionMat();
	// sceneDataCache->viewMat = camera.GetViewMat();
	// sceneDataCache->projectionMat = camera.GetProjectionMat();
}

void Cast::Renderer::RendererContext::EndScene()
{
}

void Cast::Renderer::RendererContext::Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::Shader> shader)
{
	shader->Bind();
	va->Bind();

	API::Core::RenderCommand::Draw(va, va->GetVBCount());
}

void Cast::Renderer::RendererContext::Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::IndexBuffer> ib, Ref<API::Core::Shader> shader)
{
	shader->Bind();
	va->Bind();
	ib->Bind();

	API::Core::RenderCommand::DrawIndexed(va, ib);
}

void Cast::Renderer::RendererContext::Submit(Ref<API::Core::VertexArray> va, Ref<API::Core::Buffer> ib, Ref<API::Core::Shader> shader)
{
	shader->Bind();
	va->Bind();
	ib->Bind();

	API::Core::RenderCommand::DrawIndexed(va, ib);
}
#include "castpch.h"
#include "Renderer.h"

namespace Cast::Renderer {
	RendererContext::SceneData* RendererContext::m_SceneData = new SceneData;
}

void Cast::Renderer::RendererContext::BeginScene(Camera& camera)
{
	m_SceneData->ViewProjectionMat = camera.GetViewProjectionMat();
}

void Cast::Renderer::RendererContext::EndScene()
{
}

void Cast::Renderer::RendererContext::Submit(const std::shared_ptr<API::Core::VertexArray>& va, const std::shared_ptr<API::Core::Shader>& shader)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", m_SceneData->ViewProjectionMat);

	va->Bind();
	API::Core::RenderCommand::DrawIndexed(va);
}

void Cast::Renderer::RendererContext::Submit(const std::shared_ptr<API::Core::VertexArray>& va, const std::shared_ptr<API::Core::IndexBuffer>& ib, const std::shared_ptr<API::Core::Shader>& shader)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", m_SceneData->ViewProjectionMat);

	va->Bind();
	ib->Bind();
	API::Core::RenderCommand::DrawIndexed(va, ib);
}
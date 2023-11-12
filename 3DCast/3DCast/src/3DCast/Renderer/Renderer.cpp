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

void Cast::Renderer::RendererContext::Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::Shader>& shader, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", m_SceneData->ViewProjectionMat);
	shader->SetUniformMat4f("u_Transform", transform);

	va->Bind();
	API::Core::RenderCommand::DrawIndexed(va);
}

void Cast::Renderer::RendererContext::Submit(const Ref<API::Core::VertexArray>& va, const Ref<API::Core::IndexBuffer>& ib, const Ref<API::Core::Shader>& shader, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", m_SceneData->ViewProjectionMat);
	shader->SetUniformMat4f("u_Transform", transform);

	va->Bind();
	ib->Bind();
	API::Core::RenderCommand::DrawIndexed(va, ib);
}
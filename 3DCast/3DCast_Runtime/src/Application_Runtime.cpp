#include "runtimepch.h"
#include <3DCast.h>
#include <3DCast/EntryPoint.h>
#include <imgui.h>

#include <API/texture/Texture2D.h>

#include <vendor/glm/glm.hpp>

#include "Config.h"

class ExampleLayer : public Cast::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.f, 1.f, -1.f, 1.f)
	{
		// Test Graphics
		shaderTexture.reset(API::Core::Shader::Create("../3DCast/ressources/shader/common/shader_texture.vert",
			"../3DCast/ressources/shader/common/shader_texture.frag"));

		texture.reset(API::Texture::Texture2D::Create("img/test.png"));
		texture2.reset(API::Texture::Texture2D::Create("img/test2.png"));

		float vertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f, 0.f, 0.f,
				 0.5f, -0.5f, 0.0f, 0.f, 1.f,
				 0.5f, 0.5f, 0.0f, 1.f, 1.f,
				 -0.5f, 0.5f, 0.0f, 1.f, 0.f
		};

		constexpr int faces = 1;
		unsigned int offset = 0;
		unsigned int* indices = new unsigned int[6];

		for (size_t i = 0; i < faces * 6; i += 6) {
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		ib.reset(API::Core::IndexBuffer::Create(indices, faces * 6));
		vb.reset(API::Core::VertexBuffer::Create(4, sizeof(float) * 5));

		vb->AddVertexData(vertices, sizeof(vertices));

		delete[] indices;

		vbLayout.reset(API::Core::VertexBufferLayout::Create());
		vbLayout->Push(API::Core::ShaderDataType::Float3);
		vbLayout->Push(API::Core::ShaderDataType::Float2);

		va.reset(API::Core::VertexArray::Create());
		va->AddBuffer(*vb, *vbLayout);

		texture->Bind();
		texture2->Bind(1);

		shaderTexture->Bind();
		shaderTexture->SetUniformMat4f("u_ViewProjection", m_Camera.GetViewProjectionMat());
		shaderTexture->SetUniform4f("u_Color", 0.8f, 0.1f, 0.5f, 1.0f);
	}

	void OnUpdate(Cast::Timestep ts) override {
		if (Cast::Input::IsKeyPressed(CAST_KEY_LEFT)) {
			m_CameraPosition.x -= 0.05f;
		}
		if (Cast::Input::IsKeyPressed(CAST_KEY_RIGHT)) {
			m_CameraPosition.x += 0.05f;
		}
		if (Cast::Input::IsKeyPressed(CAST_KEY_UP)) {
			m_CameraPosition.y -= 0.05f;
		}
		if (Cast::Input::IsKeyPressed(CAST_KEY_DOWN)) {
			m_CameraPosition.y += 0.05f;
		}

		m_Camera.SetPosition(m_CameraPosition);

		API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		API::Core::RenderCommand::Clear();

		Cast::Renderer::RendererContext::BeginScene(m_Camera);

		shaderTexture->Bind();
		shaderTexture->SetUniform1i("u_Texture", texture->GetBoundPort());
		Cast::Renderer::RendererContext::Submit(va, ib, shaderTexture);

		shaderTexture->SetUniform1i("u_Texture", texture2->GetBoundPort());
		Cast::Renderer::RendererContext::Submit(va, ib, shaderTexture);

		Cast::Renderer::RendererContext::EndScene();
	}

	virtual void OnImGuiRender() override {
	}

	void OnEvent(Cast::Event& e) override {
		Cast::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Cast::KeyPressedEvent>(CAST_BIND_EVENT_FUNC(ExampleLayer::OnKeyPressedEvent));
	}

	bool OnKeyPressedEvent(Cast::KeyPressedEvent& event) {
		return false;
	}

private:
	Cast::Ref<API::Texture::Texture2D> texture;
	Cast::Ref<API::Texture::Texture2D> texture2;
	Cast::Ref<API::Core::Shader> shaderTexture;
	Cast::Ref<API::Core::VertexBuffer> vb;
	Cast::Ref<API::Core::IndexBuffer> ib;
	Cast::Ref<API::Core::VertexBufferLayout> vbLayout;
	Cast::Ref<API::Core::VertexArray> va;

	Cast::Renderer::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0.f, 0.f, 0.f };
	glm::vec3 m_TrianglePosition = { 0.f, 0.f, 0.f };
};

class Application_Runtime : public Cast::Application {
public:
	Application_Runtime()
		:Application(Cast::WindowProperties("3DCast Rendering Engine", Runtime::conf.WIN_WIDTH, Runtime::conf.WIN_HEIGHT))
	{
		PushLayer(new ExampleLayer());
	}
	~Application_Runtime() {}
};

Cast::Application* Cast::CreatApplication() {
	return new Application_Runtime();
}
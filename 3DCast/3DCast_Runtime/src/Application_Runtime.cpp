#include <3DCast.h>
#include <3DCast/EntryPoint.h>
#include <imgui.h>

#include "Config.h"

class ExampleLayer : public Cast::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.f, 1.f, -1.f, 1.f) 
	{
		// Test Graphics
		shader.reset(API::Core::Shader::Create("../3DCast/ressources/shader/common/shader_single_color.vert",
			"../3DCast/ressources/shader/common/shader_single_color.frag"));

		unsigned int* indices = new unsigned int[3];

		for (int i = 0; i < 3; i++) {
			indices[i] = i;
		}

		float vertices[3 * 3] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f
		};

		ib.reset(API::Core::IndexBuffer::Create(indices, 3));
		vb.reset(API::Core::VertexBuffer::Create(3, sizeof(float) * 3));

		vb->AddVertexData(vertices, sizeof(float) * 9);

		delete[] indices;

		vbLayout.reset(API::Core::VertexBufferLayout::Create());
		vbLayout->Push(API::Core::ShaderDataType::Float3);

		va.reset(API::Core::VertexArray::Create());
		va->AddBuffer(*vb, *vbLayout);

		shader->Bind();
		shader->SetUniformMat4f("u_ViewProjection", m_Camera.GetViewProjectionMat());
		shader->SetUniform4f("u_Color", 0.8f, 0.1f, 0.5f, 1.0f);
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

		API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		API::Core::RenderCommand::Clear();

		Cast::Renderer::RendererContext::BeginScene(m_Camera);

		Cast::Renderer::RendererContext::Submit(va, ib, shader);

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
	std::shared_ptr<API::Core::Shader> shader;
	std::shared_ptr<API::Core::VertexBuffer> vb;
	std::shared_ptr<API::Core::IndexBuffer> ib;
	std::shared_ptr<API::Core::VertexBufferLayout> vbLayout;
	std::shared_ptr<API::Core::VertexArray> va;

	Cast::Renderer::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0.f, 0.f, 0.f };
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
#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <GLEW/glew.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::s_Instance = nullptr;
}

Cast::Application::Application()
	:m_Camera(-1.f, 1.f, -1.f, 1.f)
{
	CAST_CORE_ASSERT(!s_Instance, "Application is a singleton and cannot be instanced multiple times!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);

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

Cast::Application::~Application()
{ }

void Cast::Application::Run()
{
	while (m_Running) {
		API::Core::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		API::Core::RenderCommand::Clear();

		Renderer::RendererContext::BeginScene(m_Camera);

		static float angle = 0.5;
		angle += 0.5;
		m_Camera.SetZRotation(angle);

		Renderer::RendererContext::Submit(va, ib, shader);

		Renderer::RendererContext::EndScene();

		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack) {
			layer->OnImGuiRender();
		}
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}

	LOG_INFO("Terminating Application");
}

void Cast::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

	// LOG_CORE_TRACE("{0}", e.ToString());
	for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin();) {
		(*--iter)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Cast::Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Cast::Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Cast::Application::PopLayer(Layer* layer)
{
	m_LayerStack.PopLayer(layer);
	layer->OnDetach();
}

void Cast::Application::PopOverlay(Layer* overlay)
{
	m_LayerStack.PopOverlay(overlay);
	overlay->OnDetach();
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}
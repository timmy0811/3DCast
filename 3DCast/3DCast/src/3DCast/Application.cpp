#include "castpch.h"
#include "Application.h"

#include "3DCast/Log.h"
#include "Input.h"

#include <GLEW/glew.h>

#include "OpenGL_util/core/Renderer.h"

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast {
	Application* Application::s_Instance = nullptr;
}

Cast::Application::Application()
{
	CAST_CORE_ASSERT(!s_Instance, "Application is a singleton and cannot be instanced multiple times!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);

	// Test Graphics
	shader.reset(new GL::Core::Shader("C:/Git/3DCast/3DCast/GLWrapperLib/OpenGL_util/shader/universal/shader_single_color.vert",
		"C:/Git/3DCast/3DCast/GLWrapperLib/OpenGL_util/shader/universal/shader_single_color.frag"));

	unsigned int* indices = new unsigned int[3];

	for (int i = 0; i < 3; i++) {
		indices[i] = i;
	}

	float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
	};

	ib = std::make_unique<GL::Core::IndexBuffer>(indices, 3);
	vb = std::make_unique<GL::Core::VertexBuffer>(3, sizeof(float) * 3);

	vb->AddVertexData(vertices, sizeof(float) * 9);

	delete[] indices;

	vbLayout = std::make_unique<GL::Core::VertexBufferLayout>();
	vbLayout->Push<float>(3);	// Position

	va = std::make_unique<GL::Core::VertexArray>();
	va->AddBuffer(*vb, *vbLayout);

	shader->Bind();
	shader->SetUniform4f("u_Color", 0.8, 0.1, 0.5, 1.0);
}

Cast::Application::~Application()
{
}

void Cast::Application::Run()
{
	while (m_Running) {
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}

		shader->Bind();
		va->Bind();
		ib->Bind();
		GL::Core::GLContext::Draw(*va, *ib, *shader, GL_TRIANGLES);

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
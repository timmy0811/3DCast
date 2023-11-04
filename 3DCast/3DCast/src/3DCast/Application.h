#pragma once

#include "Core.h"
#include "Window.h"

#include "3DCast/Event/Event.h"
#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Layer/LayerStack.h"
#include "3DCast/ImGui/ImGuiLayer.h"
#include "Renderer/Camera/OrthographicCamera.h"

#include "API/core/Shader.h"
#include "API/core/Framebuffer.h"
#include "API/core/VertexArray.h"
#include "API/core/IndexBuffer.h"

namespace Cast {
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		static Application* s_Instance;

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		// Rendering Context
		std::shared_ptr<API::Core::Shader> shader;
		std::shared_ptr<API::Core::VertexBuffer> vb;
		std::shared_ptr<API::Core::IndexBuffer> ib;
		std::shared_ptr<API::Core::VertexBufferLayout> vbLayout;
		std::shared_ptr<API::Core::VertexArray> va;

		Renderer::OrthographicCamera m_Camera;
	};

	Application* CreatApplication();
}

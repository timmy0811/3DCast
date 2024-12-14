#pragma once

#include "Core.h"
#include "Window.h"

#include "3DCast/Event/Event.h"
#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Layer/LayerStack.h"
#include "3DCast/ImGui/ImGuiLayer.h"

#include "3DCast/Core/Timestep.h"

namespace Cast {
	class Application
	{
	public:
		Application(const WindowProperties& properties);
		~Application() = default;

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline static Application& Get() { return *instance; }
		inline Window& GetWindow() { return *window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		static Application* instance;

		std::unique_ptr<Window> window;
		ImGuiLayer* imGuiLayer;
		bool running = true;
		LayerStack layerStack;

		float lastFrameTime = 0.f;
	};

	Application* CreatApplication();
}

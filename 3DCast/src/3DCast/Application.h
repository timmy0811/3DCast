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

		void Close();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline static Application& Get() { return *Instance; }
		inline Window& GetWindow() { return *AppWindow; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* Instance;

		std::shared_ptr<Window> AppWindow;
		ImGuiLayer* GuiLayer;
		bool Running = true;
		bool Minimized = false;
		LayerStack LayerStack;

		float LastFrameTime = 0.f;
	};

	Application* CreateApplication();
}

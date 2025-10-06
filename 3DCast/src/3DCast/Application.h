#pragma once

#include "Core/Window.h"

#include "3DCast/Event/Event.h"
#include "3DCast/Event/ApplicationEvent.h"
#include "3DCast/Layer/LayerStack.h"
#include "3DCast/Gui/ImGuiLayer.h"

namespace Cast
{
	class Application
	{
	public:
		explicit Application(const WindowProperties& properties);
		~Application() = default;

		void Run();
		void OnEvent(Event& e);

		void Close();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		static Application& Get()
		{
			if (!Instance) throw std::runtime_error("Application instance is not initialized!");
			return *Instance;
		}

		[[nodiscard]] Window& GetWindow() const { return *AppWindow; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(const WindowResizeEvent& e);

	private:
		static Application* Instance;

		std::shared_ptr<Window> AppWindow;
		ImGuiLayer* GuiLayer;
		bool Running = true;
		bool Minimized = false;
		LayerStack LStack;

		float LastFrameTime = 0.f;
	};

	Application* CreateApplication();
}

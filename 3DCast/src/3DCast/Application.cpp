#include "castpch.h"
#include "Application.h"

#include "3DCast/Core/Log.h"
#include "3DCast/Input/Input.h"

#include "3DCast/Renderer/Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BIND_EVENT_FUNC(x) std::bind(&Cast::Application::x, this, std::placeholders::_1)

namespace Cast
{
    Application* Application::Instance = nullptr;
}

Cast::Application::Application(const WindowProperties& properties)
{
    CAST_CORE_ASSERT(!Instance, "Application is a singleton and cannot be instanced multiple times!");
    Instance = this;

    AppWindow = std::unique_ptr(Window::Create(properties));
    AppWindow->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

    Renderer::RendererContext::Init();

    GuiLayer = new ImGuiLayer();
    PushOverlay(GuiLayer);
}

void Cast::Application::Run()
{
    while (Running)
    {
        const auto time = static_cast<float>(glfwGetTime());
        const Timestep timestep = time - LastFrameTime;
        LastFrameTime = time;


        API::Core::RenderCommand::Clear();

        if (!Minimized)
        {
            for (Layer* layer : LStack)
            {
                layer->OnUpdate(timestep);
            }
        }

        Cast::ImGuiLayer::Begin();
        for (Layer* layer : LStack)
        {
            layer->OnImGuiRender();
        }
        Cast::ImGuiLayer::End();

        AppWindow->OnUpdate();
    }

    LOG_INFO("Terminating Application");
}

void Cast::Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OnWindowResize));

    for (auto iter = LStack.end(); iter != LStack.begin();)
    {
        (*--iter)->OnEvent(e);
        if (e.handled)
            break;
    }
}

void Cast::Application::Close()
{
    Running = false;
}

void Cast::Application::PushLayer(Layer* layer)
{
    layer->SetParentWindow(AppWindow);
    LStack.PushLayer(layer);
    layer->OnAttach();
}

void Cast::Application::PushOverlay(Layer* overlay)
{
    LStack.PushOverlay(overlay);
    overlay->OnAttach();
}

void Cast::Application::PopLayer(Layer* layer)
{
    LStack.PopLayer(layer);
    layer->OnDetach();
}

void Cast::Application::PopOverlay(Layer* overlay)
{
    LStack.PopOverlay(overlay);
    overlay->OnDetach();
}

bool Cast::Application::OnWindowClose(WindowCloseEvent& e)
{
    Running = false;
    return true;
}

bool Cast::Application::OnWindowResize(const WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        Minimized = true;
        return false;
    }

    Minimized = false;
    Renderer::RendererContext::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

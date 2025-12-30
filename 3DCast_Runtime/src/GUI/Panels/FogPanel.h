#pragma once

#include <functional>

namespace Runtime::GUI
{
    class FogPanel
    {
    public:
        FogPanel() = default;
        ~FogPanel() = default;

        void Open();
        void OnImGuiRender();

        void SetOnChangeCallback(std::function<void()> callback);

    private:
        bool IsOpen = false;
        std::function<void()> OnChangeCallback;
    };
}


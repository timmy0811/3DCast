#pragma once
#include <string>
#include <vector>

namespace Runtime::GUI {
    class DiagnosticsPanel {
    public:
        DiagnosticsPanel() = delete;

        static void OnImGuiRender(float dt);
    };
}

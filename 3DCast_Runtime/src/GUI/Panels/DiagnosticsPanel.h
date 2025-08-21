#pragma once
#include <string>
#include <vector>

namespace Runtime::GUI {
    class DiagnosticsPanel {
    public:
        DiagnosticsPanel() = delete;

        static void Open();

        static void OnImGuiRender(float dt);

    private:
#ifdef CAST_DEBUG
        static inline bool IsOpen = true;
#else
        static inline bool IsOpen = false;
#endif
    };
}

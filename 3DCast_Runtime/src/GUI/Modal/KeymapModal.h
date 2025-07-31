#pragma once

namespace Runtime::GUI {
    class Keymap {
    public:
        Keymap() = delete;

        static void OnImGuiRender();
        inline static void Open() {g_ShowKeymapModal = true;}

    private:
        inline static bool g_ShowKeymapModal;
    };
}

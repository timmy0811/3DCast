#pragma once

#include <string>

#include "imgui.h"


namespace Cast::Font
{
    // Define the font size as a configurable variable
    inline ImFont* fa900_64px;

    inline void LoadFonts()
    {
        const ImGuiIO& io = ImGui::GetIO();
        // Font Awesome range (PUA). For FA5/6 solid:
        static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

        // Option A: separate FA font (push when needed)
        fa900_64px = io.Fonts->AddFontFromFileTTF(".../fa-solid-900.ttf", 64.0f, nullptr, icons_ranges);

    }
}
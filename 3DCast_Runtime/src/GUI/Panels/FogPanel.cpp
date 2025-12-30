#include "FogPanel.h"

#include "imgui.h"
#include "Data/SharedEditorData.h"

#include <3DCast/Misc/Icon.h>
#include <3DCast/Renderer/Skybox.h>
#include <vendor/glm/gtc/type_ptr.hpp>

void Runtime::GUI::FogPanel::Open()
{
    IsOpen = true;
}

void Runtime::GUI::FogPanel::OnImGuiRender()
{
    if (!IsOpen)
        return;

    ImGui::Begin(ICON_FA_SMOG " Fog", &IsOpen);

    bool changed = false;

    ImGui::TextUnformatted("Enable Fog");
    TWOTHIRD_INPUT
    if (ImGui::Checkbox("##EnableFog", &EditorContext.Fog.Enabled))
    {
        changed = true;
    }

    ImGui::BeginDisabled(!EditorContext.Fog.Enabled);

    // Fog Colors Section
    ImGui::SeparatorText("Color Gradient");
    ImGui::Text("Bottom Color");
    TWOTHIRD_INPUT
    if (ImGui::ColorEdit3("##FogBottomColor", glm::value_ptr(EditorContext.Fog.ColorBottom)))
    {
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Color of fog at the lower height bound");
        ImGui::EndTooltip();
    }

    ImGui::Text("Top Color");
    TWOTHIRD_INPUT
    if (ImGui::ColorEdit3("##FogTopColor", glm::value_ptr(EditorContext.Fog.ColorTop)))
    {
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Color of fog at the upper height bound");
        ImGui::EndTooltip();
    }

    const bool isClearColorMode = EditorContext.Skybox.GetRenderMode() == Cast::Renderer::Skybox::RenderMode::ClearColor;
    ImGui::BeginDisabled(!isClearColorMode);
    ImGui::SetCursorPosX(GUIWIN_WTHIRD);
    if (ImGui::Button("Use Skybox Color", ImVec2(GUIWIN_WTHIRD * 2.f - GUIWIN_ENDELEMENT_PADDING, 0)))
    {
        const glm::vec4& skyboxColor = EditorContext.Skybox.GetClearColor();
        EditorContext.Fog.ColorBottom = glm::vec3(skyboxColor);
        EditorContext.Fog.ColorTop = glm::vec3(skyboxColor);
        changed = true;
    }
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::BeginTooltip();
        if (isClearColorMode)
        {
            ImGui::Text("Set both fog colors to match the skybox clear color");
        }
        else
        {
            ImGui::Text("Only available when Skybox is in 'Clear Color' mode");
        }
        ImGui::EndTooltip();
    }

    // Fog Angle Gradient Section
    ImGui::Text("Min. Angle");
    ImGui::SameLine(GUIWIN_WQUARTER);
    ImGui::SetNextItemWidth(GUIWIN_WQUARTER - GUIWIN_ELEMENT_PADDING);
    if (ImGui::DragFloat("##AngleMin", &EditorContext.Fog.AngleMin, 0.5f, -90.0f, 90.0f, "%.1f°"))
    {
        if (EditorContext.Fog.AngleMin >= EditorContext.Fog.AngleMax)
        {
            EditorContext.Fog.AngleMin = EditorContext.Fog.AngleMax - 0.1f;
        }
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Vertical viewing angle (degrees) where bottom fog color applies");
        ImGui::Text("Negative = below horizon, Positive = above horizon");
        ImGui::EndTooltip();
    }

    ImGui::SameLine(GUIWIN_WQUARTER * 2.f);
    ImGui::Text("Max. Angle");
    ImGui::SameLine(GUIWIN_WQUARTER * 3.f);
    ImGui::SetNextItemWidth(GUIWIN_WQUARTER - GUIWIN_ENDELEMENT_PADDING);
    if (ImGui::DragFloat("##AngleMax", &EditorContext.Fog.AngleMax, 0.5f, -90.0f, 90.0f, "%.1f°"))
    {
        // Ensure max is greater than min
        if (EditorContext.Fog.AngleMax <= EditorContext.Fog.AngleMin)
        {
            EditorContext.Fog.AngleMax = EditorContext.Fog.AngleMin + 0.1f;
        }
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Vertical viewing angle (degrees) where top fog color applies");
        ImGui::Text("Negative = below horizon, Positive = above horizon");
        ImGui::EndTooltip();
    }

    // Fog Distance Section
    ImGui::SeparatorText("Behaviour");
    ImGui::Text("Start Distance");
    TWOTHIRD_INPUT
    if (ImGui::DragFloat("##FogStartDistance", &EditorContext.Fog.StartDistance, 0.5f, 0.0f, 1000.0f, "%.1f"))
    {
        if (EditorContext.Fog.StartDistance >= EditorContext.Fog.EndDistance)
        {
            EditorContext.Fog.StartDistance = EditorContext.Fog.EndDistance - 0.1f;
        }
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Distance from camera where fog begins");
        ImGui::EndTooltip();
    }

    ImGui::Text("End Distance");
    TWOTHIRD_INPUT
    if (ImGui::DragFloat("##FogEndDistance", &EditorContext.Fog.EndDistance, 0.5f, 0.1f, 2000.0f, "%.1f"))
    {
        if (EditorContext.Fog.EndDistance <= EditorContext.Fog.StartDistance)
        {
            EditorContext.Fog.EndDistance = EditorContext.Fog.StartDistance + 0.1f;
        }
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Distance from camera where fog fully tints geometry");
        ImGui::EndTooltip();
    }

    ImGui::Text("Falloff");
    TWOTHIRD_INPUT
    if (ImGui::RadioButton("Linear", !EditorContext.Fog.UseExponential))
    {
        EditorContext.Fog.UseExponential = false;
        changed = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Exponential", EditorContext.Fog.UseExponential))
    {
        EditorContext.Fog.UseExponential = true;
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Linear: Uniform fog increase with distance");
        ImGui::Text("Exponential: More natural, gradual fog buildup");
        ImGui::EndTooltip();
    }

    ImGui::BeginDisabled(!EditorContext.Fog.UseExponential);
    ImGui::Text("Steepness");
    TWOTHIRD_INPUT
    if (ImGui::SliderFloat("##FogDensity", &EditorContext.Fog.Density, 0.5f, 5.0f, "%.2f"))
    {
        changed = true;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Controls the steepness of the exponential curve");
        ImGui::Text("Lower = more gradual, Higher = steeper falloff");
        ImGui::EndTooltip();
    }
    ImGui::EndDisabled();

    ImGui::EndDisabled();

    if (changed && OnChangeCallback)
    {
        OnChangeCallback();
    }

    ImGui::End();
}

void Runtime::GUI::FogPanel::SetOnChangeCallback(std::function<void()> callback)
{
    OnChangeCallback = std::move(callback);
}


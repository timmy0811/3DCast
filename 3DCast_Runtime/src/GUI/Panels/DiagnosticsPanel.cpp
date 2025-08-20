#include <runtimepch.h>
#include "DiagnosticsPanel.h"

#include <ctime>
#include <3DCast/Misc/Icon.h>

#include "imgui.h"
#include "Data/SharedEditorData.h"

void Runtime::GUI::DiagnosticsPanel::OnImGuiRender(float dt)
{
    ImGui::Begin(ICON_FA_STETHOSCOPE " Diagnostics");

    const int fps = (int)(1.0f / dt);
    static int maxFPS = 0;
    static int minFPS = fps;
    maxFPS = std::max(maxFPS, fps);
    minFPS = std::min(minFPS, fps);

    static int maxFPSDisplay = fps;
    static int minFPSDisplay = fps;

    static time_t startTime = time(nullptr);
    if (difftime(time(nullptr), startTime) >= 1)
    {
        startTime = time(nullptr);
        maxFPSDisplay = maxFPS;
        minFPSDisplay = minFPS;
        maxFPS = 0;
        minFPS = fps;
    }

    ImGui::Text("Frames per second");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%d", fps);

    ImGui::Text("Max FPS");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%d", maxFPSDisplay);

    ImGui::Text("Min FPS");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%d", minFPSDisplay);

    ImGui::Text("Frametime");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%.2f", dt * 1000.f);

    ImGui::Text("Camera Position");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%f, %f, %f", Runtime::EditorContext.ActiveCamera->GetPosition().x,
                Runtime::EditorContext.ActiveCamera->GetPosition().y,
                Runtime::EditorContext.ActiveCamera->GetPosition().z);

    ImGui::Text("Camera Rotation");
    ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
    ImGui::Text("%f, %f, %f", Runtime::EditorContext.ActiveCamera->GetRotation().x,
                Runtime::EditorContext.ActiveCamera->GetRotation().y,
                Runtime::EditorContext.ActiveCamera->GetRotation().z);

    if (ImGui::CollapsingHeader("Transform Registry"))
    {
        ImGui::Text("Registered Transforms");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::Shared.ActiveScene->GetTransformRegistry()->GetTransformsCount());

        ImGui::Text("Storage Buffer Size");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d bytes", Cast::Shared.ActiveScene->GetTransformRegistry()->GetSSAOSize());
    }

    if (ImGui::CollapsingHeader("Texture Registry"))
    {
        ImGui::Text("Cached Textures");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetTextureCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetProxyCount());

        ImGui::Text("Cached Paths");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetPathCacheCount());
    }

    if (ImGui::CollapsingHeader("Shader Registry"))
    {
        ImGui::Text("Cached Shaders");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::ShaderCacheRegistryInstance.GetShaderCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::ShaderCacheRegistryInstance.GetProxyCount());
    }

    if (ImGui::CollapsingHeader("Material Registry"))
    {
        ImGui::Text("Registered Materials");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::MaterialCacheRegistryInstance.GetMaterialCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d", Cast::MaterialCacheRegistryInstance.GetProxyCount());
    }

    if (ImGui::CollapsingHeader("Deferred Sampler Store"))
    {
        ImGui::Text("Stored Diffuse Textures");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetDiffuseTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetDiffuseTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetDiffuseTextureSSAOCapacity());

        ImGui::Text("Stored Specular Textures");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetSpecularTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetSpecularTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetSpecularTextureSSAOCapacity());

        ImGui::Text("Stored Parallax Textures");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d    (%d of %d bytes used))", Cast::DeferredSamplerStoreInstance.GetParallaxTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetParallaxTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetParallaxTextureSSAOCapacity());

        ImGui::Text("Stored Normal Textures");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetNormalTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetNormalTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetNormalTextureSSAOCapacity());

        ImGui::Text("Stored Materials");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetMaterialCount(),
                    Cast::DeferredSamplerStoreInstance.GetCustomMaterialSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetCustomMaterialSSAOCapacity());

        ImGui::Text("Mapping SSAO Size");
        ImGui::SameLine(SAMELINE_WIDGET_OFFSET_HALF);
        ImGui::Text("%d of %d bytes used", Cast::DeferredSamplerStoreInstance.GetSamplerMappingsSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetSamplerMappingsSSAOCapacity());
    }

    ImGui::End();
}

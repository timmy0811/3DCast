#include <runtimepch.h>
#include "DiagnosticsPanel.h"

#include <ctime>
#include <3DCast/Misc/Icon.h>

#include "imgui.h"
#include "Data/SharedEditorData.h"
#include "3DCast/Memory/Batching/BatchManager.h"

void Runtime::GUI::DiagnosticsPanel::Open()
{
    IsOpen = true;
}

void Runtime::GUI::DiagnosticsPanel::OnImGuiRender(const float dt)
{
    if (!IsOpen)
        return;

    ImGui::Begin(ICON_FA_STETHOSCOPE " Diagnostics", &IsOpen);

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
    ImGui::SameLine(GUIWIN_WTHIRD);
    ImGui::Text("%d", fps);

    ImGui::Text("Max FPS");
    ImGui::SameLine(GUIWIN_WTHIRD);
    ImGui::Text("%d", maxFPSDisplay);

    ImGui::Text("Min FPS");
    ImGui::SameLine(GUIWIN_WTHIRD);
    ImGui::Text("%d", minFPSDisplay);

    ImGui::Text("Frametime");
    ImGui::SameLine(GUIWIN_WTHIRD);
    ImGui::Text("%.2f", dt * 1000.f);

    if (Cast::Shared.ActiveScene)
    {
        ImGui::Text("Camera Position");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%f, %f, %f", Runtime::EditorContext.ActiveCamera.value()->GetPosition().x,
                    Runtime::EditorContext.ActiveCamera.value()->GetPosition().y,
                    Runtime::EditorContext.ActiveCamera.value()->GetPosition().z);

        ImGui::Text("Camera Rotation");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%f, %f, %f", Runtime::EditorContext.ActiveCamera.value()->GetRotation().x,
                    Runtime::EditorContext.ActiveCamera.value()->GetRotation().y,
                    Runtime::EditorContext.ActiveCamera.value()->GetRotation().z);
    }

    if (ImGui::CollapsingHeader("Shader Registry"))
    {
        ImGui::Text("Cached Shaders");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::ShaderCacheRegistryInstance.GetShaderCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::ShaderCacheRegistryInstance.GetProxyCount());
    }

    if (Cast::Shared.ActiveScene)
    {
        if (ImGui::CollapsingHeader("Transform Registry"))
    {
        ImGui::Text("Registered Transforms");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::Shared.ActiveScene->GetTransformRegistry()->GetTransformsCount());

        ImGui::Text("Storage Buffer Size");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d bytes", Cast::Shared.ActiveScene->GetTransformRegistry()->GetSSAOSize());
    }

    if (ImGui::CollapsingHeader("Texture Registry"))
    {
        ImGui::Text("Cached Textures");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetTextureCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetProxyCount());

        ImGui::Text("Cached Paths");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::TextureCacheRegistryInstance.GetPathCacheCount());
    }

    if (ImGui::CollapsingHeader("Material Registry"))
    {
        ImGui::Text("Registered Materials");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::MaterialCacheRegistryInstance.GetMaterialCount());

        ImGui::Text("Defined Proxies");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::MaterialCacheRegistryInstance.GetProxyCount());
    }

    if (ImGui::CollapsingHeader("Batch Manager"))
    {
        // Basic stats
        ImGui::Text("Batch Storages");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu", Cast::Memory::BatchMemoryHandler.GetBatchStorageCount());

        ImGui::Text("Indexed Batch Storages");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu", Cast::Memory::BatchMemoryHandler.GetBatchStorageIndexedCount());

        ImGui::Text("Total Batch Objects");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu", Cast::Memory::BatchMemoryHandler.GetTotalObjectCount());

        ImGui::Text("Bulk Display Objects");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu", Cast::Memory::BatchMemoryHandler.GetBulkObjectCount());

        ImGui::Separator();

        const size_t totalStdUsed = Cast::Memory::BatchMemoryHandler.GetTotalBatchStorageMemoryUsed();
        const size_t totalStdCapacity = Cast::Memory::BatchMemoryHandler.GetTotalBatchStorageMemoryCapacity();
        const float stdUsagePercent = totalStdCapacity > 0 ? (float)totalStdUsed / (float)totalStdCapacity * 100.0f : 0;

        ImGui::Text("Default Storage Size");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu bytes", Cast::Memory::BatchMemoryHandler.GetBatchStorageSize());

        ImGui::Text("Memory Used");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu of %zu bytes (%.1f%%)", totalStdUsed, totalStdCapacity, stdUsagePercent);

        const size_t totalIdxUsed = Cast::Memory::BatchMemoryHandler.GetTotalBatchStorageIndexedMemoryUsed();
        const size_t totalIdxCapacity = Cast::Memory::BatchMemoryHandler.GetTotalBatchStorageIndexedMemoryCapacity();
        const float idxUsagePercent = totalIdxCapacity > 0 ? (float)totalIdxUsed / (float)totalIdxCapacity * 100.0f : 0;

        ImGui::Text("Max Indices");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d", Cast::Memory::BatchMemoryHandler.GetMaxIndices());

        ImGui::Text("Indexed Memory Used");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%zu of %zu bytes (%.1f%%)", totalIdxUsed, totalIdxCapacity, idxUsagePercent);

        if (ImGui::TreeNode("Standard Batch Storages"))
        {
            for (size_t i = 0; i < Cast::Memory::BatchMemoryHandler.GetBatchStorageCount(); i++)
            {
                const size_t used = Cast::Memory::BatchMemoryHandler.GetBatchStorageMemoryUsed(i);
                const size_t capacity = Cast::Memory::BatchMemoryHandler.GetBatchStorageMemoryCapacity(i);
                const float usagePercent = capacity > 0 ? (float)used / (float)capacity * 100.0f : 0;
                const size_t objectCount = Cast::Memory::BatchMemoryHandler.GetBatchStorageObjectCount(i);

                ImGui::Text("Storage #%zu", i);
                ImGui::SameLine(GUIWIN_WTHIRD);
                ImGui::Text("%zu objects", objectCount);

                ImGui::SetCursorPosX(GUIWIN_WTHIRD);
                ImGui::Text("%zu of %zu bytes (%.1f%%)", used, capacity, usagePercent);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Indexed Batch Storages"))
        {
            for (size_t i = 0; i < Cast::Memory::BatchMemoryHandler.GetBatchStorageIndexedCount(); i++)
            {
                const size_t used = Cast::Memory::BatchMemoryHandler.GetBatchStorageIndexedMemoryUsed(i);
                const size_t capacity = Cast::Memory::BatchMemoryHandler.GetBatchStorageIndexedMemoryCapacity(i);
                const float usagePercent = capacity > 0 ? (float)used / (float)capacity * 100.0f : 0;
                const size_t objectCount = Cast::Memory::BatchMemoryHandler.GetBatchStorageIndexedObjectCount(i);

                ImGui::Text("Storage #%zu", i);
                ImGui::SameLine(GUIWIN_WTHIRD);
                ImGui::Text("%zu objects", objectCount);

                ImGui::SetCursorPosX(GUIWIN_WTHIRD);
                ImGui::Text("%zu of %zu bytes (%.1f%%)", used, capacity, usagePercent);
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Deferred Sampler Store"))
    {
        ImGui::Text("Stored Diffuse Textures");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetDiffuseTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetDiffuseTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetDiffuseTextureSSAOCapacity());

        ImGui::Text("Stored Specular Textures");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetSpecularTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetSpecularTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetSpecularTextureSSAOCapacity());

        ImGui::Text("Stored Parallax Textures");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d    (%d of %d bytes used))", Cast::DeferredSamplerStoreInstance.GetParallaxTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetParallaxTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetParallaxTextureSSAOCapacity());

        ImGui::Text("Stored Normal Textures");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetNormalTextureCount(),
                    Cast::DeferredSamplerStoreInstance.GetNormalTextureSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetNormalTextureSSAOCapacity());

        ImGui::Text("Stored Materials");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d    (%d of %d bytes used)", Cast::DeferredSamplerStoreInstance.GetMaterialCount(),
                    Cast::DeferredSamplerStoreInstance.GetCustomMaterialSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetCustomMaterialSSAOCapacity());

        ImGui::Text("Mapping SSAO Size");
        ImGui::SameLine(GUIWIN_WTHIRD);
        ImGui::Text("%d of %d bytes used", Cast::DeferredSamplerStoreInstance.GetSamplerMappingsSSAOSize(),
                    Cast::DeferredSamplerStoreInstance.GetSamplerMappingsSSAOCapacity());
    }
    }

    ImGui::End();
}

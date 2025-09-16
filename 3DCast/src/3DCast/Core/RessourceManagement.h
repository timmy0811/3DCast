#pragma once

#include "3DCast/Data/GlobalShared.h"
#include "3DCast/Data/ShaderDataObjects/Vertex.h"
#include "3DCast/Memory/Batching/BatchManager.h"
#include "3DCast/Scene/Registry/DeferredSamplerStore.h"
#include "3DCast/Scene/Registry/TextureCacheRegistry.h"

namespace Cast
{
    inline void ResetSceneContext()
    {
        MaterialCacheRegistryInstance = MaterialCacheRegistry();
        TextureCacheRegistryInstance = TextureCacheRegistry();

        DeferredSamplerStoreInstance = DeferredSamplerStore();
        Cast::DeferredSamplerStoreInstance.InitAfterDriverSetup();

        Memory::BatchMemoryHandler = Memory::BatchManager();
        Memory::BatchMemoryHandler.Init(sizeof(Cast::Memory::BatchVertexShaderObject) * MAX_BATCH_VERTICES, MAX_BATCH_INDICES);

        Shared.VertexEntities.clear();
    }
}

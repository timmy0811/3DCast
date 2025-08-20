#include "MaterialCacheRegistry.h"
#include "3DCast/Core/Log.h"

namespace Cast
{
    // Global instance
    MaterialCacheRegistry MaterialCacheRegistryInstance;
}

Cast::MaterialCacheRegistry::MaterialCacheRegistry()
{
}

void Cast::MaterialCacheRegistry::Clear()
{
    LOG_CORE_INFO("Clearing material registry ({} materials removed)", Materials.size());
    Materials.clear();
    ProxyIds.clear();
}

Cast::UID Cast::MaterialCacheRegistry::Create()
{
    const Material defaultMaterial{};
    Materials[defaultMaterial.id] = defaultMaterial;

    LOG_CORE_INFO("Created new material with ID {}", defaultMaterial.id);

    return defaultMaterial.id;
}

Cast::UID Cast::MaterialCacheRegistry::Add(const Material& material)
{
    Materials[material.id] = material;

    LOG_CORE_INFO("Added material with ID {}", material.id);

    return material.id;
}

Cast::UID Cast::MaterialCacheRegistry::Remove(const std::string& proxy)
{
    if (!HasMaterial(proxy))
    {
        LOG_CORE_WARN("Attempted to remove non-existent material '{}'", proxy);
        return UID::None();
    }

    const UID id = ProxyIds[proxy];
    Materials.erase(id);
    ProxyIds.erase(proxy);

    LOG_CORE_INFO("Removed material '{}' with ID {}", proxy, id.GetID());

    return id;
}

Cast::UID Cast::MaterialCacheRegistry::Remove(const UID id)
{
    if (!HasMaterial(id))
    {
        LOG_CORE_WARN("Attempted to remove non-existent material with ID {}", id.GetID());
        return UID::None();
    }

    for (auto it = ProxyIds.begin(); it != ProxyIds.end();)
    {
        if (it->second == id)
            it = ProxyIds.erase(it);
        else
            ++it;
    }

    Materials.erase(id);
    LOG_CORE_INFO("Removed material with ID {}", id.GetID());

    return id;
}

bool Cast::MaterialCacheRegistry::HasMaterial(const std::string& proxy) const
{
    if (ProxyIds.find(proxy) == ProxyIds.end())
        return false;

    return Materials.find(ProxyIds.at(proxy)) != Materials.end();
}

bool Cast::MaterialCacheRegistry::HasMaterial(const UID id) const
{
    return Materials.find(id) != Materials.end();
}

bool Cast::MaterialCacheRegistry::AddProxy(UID textureId, const std::string& proxyName)
{
    ProxyIds.insert({proxyName, textureId});
    if (Materials.find(textureId) == Materials.end())
    {
        LOG_CORE_ERROR("Attempted to add proxy for non-existent texture with ID: {0}", textureId.GetID());
        return false;
    }

    return true;
}

Cast::Material& Cast::MaterialCacheRegistry::GetHandle(const std::string& proxy)
{
    if (!HasMaterial(proxy))
    {
        LOG_CORE_ERROR("Attempted to get handle for non-existent material '{}'", proxy);
        static Material defaultMaterial{};
        return defaultMaterial;
    }

    return Materials.at(ProxyIds.at(proxy));
}

Cast::Material& Cast::MaterialCacheRegistry::GetHandle(const UID id)
{
    if (!HasMaterial(id))
    {
        LOG_CORE_ERROR("Attempted to get handle for non-existent material with ID {}", id.GetID());
        static Material defaultMaterial{};
        return defaultMaterial;
    }

    return Materials.at(id);
}

Cast::Material Cast::MaterialCacheRegistry::Get(const std::string& proxy) const
{
    if (!HasMaterial(proxy))
    {
        LOG_CORE_ERROR("Attempted to get non-existent material '{}'", proxy);
        return Material{};
    }

    return Materials.at(ProxyIds.at(proxy));
}

Cast::Material Cast::MaterialCacheRegistry::Get(const UID id) const
{
    if (!HasMaterial(id))
    {
        LOG_CORE_ERROR("Attempted to get non-existent material with ID {}", id.GetID());
        return Material{};
    }

    return Materials.at(id);
}

bool Cast::MaterialCacheRegistry::Edit(const std::string& proxy, const Material& material)
{
    if (!HasMaterial(proxy))
    {
        LOG_CORE_ERROR("Cannot edit non-existent material '{}'", proxy);
        return false;
    }

    Materials[ProxyIds.at(proxy)] = material;

    return true;
}

bool Cast::MaterialCacheRegistry::Edit(const UID id, const Material& material)
{
    if (!HasMaterial(id))
    {
        LOG_CORE_ERROR("Cannot edit non-existent material with ID {}", id.GetID());
        return false;
    }

    Materials[id] = material;

    return true;
}

Cast::UID Cast::MaterialCacheRegistry::GetMaterialId(const std::string& proxy) const
{
    if (!HasMaterial(proxy))
    {
        LOG_CORE_WARN("Attempted to get index for non-existent material '{}', returning 0", proxy);
        return UID::None();
    }

    return ProxyIds.at(proxy);
}

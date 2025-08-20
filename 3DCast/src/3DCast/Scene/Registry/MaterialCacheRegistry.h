#pragma once

#include <string>
#include <unordered_map>

#include "3DCast/Data/ShaderDataObjects/Material.h"
#include "3DCast/Misc/UID.h"

namespace Cast
{
    struct Material
    {
        UID id = UID::Create();
        CustomMaterialShaderObject shaderObject{};
    };

    class MaterialCacheRegistry
    {
    public:
        MaterialCacheRegistry();
        ~MaterialCacheRegistry() = default;

        void Clear();

        UID Create();

        UID Add(const Material& material);

        UID Remove(const std::string& proxy);
        UID Remove(UID id);

        bool Edit(const std::string& proxy, const Material& material);
        bool Edit(UID id, const Material& material);

        [[nodiscard]] bool HasMaterial(const std::string& proxy) const;
        [[nodiscard]] bool HasMaterial(UID id) const;

        bool AddProxy(UID textureId, const std::string& proxyName);

        Material& GetHandle(const std::string& proxy);
        Material& GetHandle(UID id);

        [[nodiscard]] Material Get(const std::string& proxy) const;
        [[nodiscard]] Material Get(UID id) const;

        [[nodiscard]] UID GetMaterialId(const std::string& proxy) const;
        [[nodiscard]] inline const std::unordered_map<std::string, UID>& GetMaterialNames() const { return ProxyIds; }

#pragma region ANALYTICS
        [[nodiscard]] inline unsigned int GetMaterialCount() const { return static_cast<unsigned int>(Materials.size()); }
        [[nodiscard]] inline unsigned int GetProxyCount() const { return static_cast<unsigned int>(ProxyIds.size()); }
#pragma endregion

    private:
        std::unordered_map<UID, Material> Materials;
        std::unordered_map<std::string, UID> ProxyIds;
    };

    extern MaterialCacheRegistry MaterialCacheRegistryInstance;
}

#include "castpch.h"
#include "TextureCacheRegistry.h"

namespace Cast
{
    TextureCacheRegistry TextureCacheRegistryInstance{};
}

Cast::TextureCacheRegistry::TextureCacheRegistry()
{
    Textures.reserve(0x1000);
}

Cast::UID Cast::TextureCacheRegistry::Add(Ref<API::Texture::Texture> texture, const bool useCached)
{
    if (useCached)
    {
        const auto it = PathCache.find(texture->GetPath());
        if (it != PathCache.end())
        {
            const auto itTex = Textures.find(it->second);
            if (itTex != Textures.end())
            {
                itTex->second.second++;
                return itTex->first;
            }
            else
            {
                LOG_CORE_ERROR(
                    "Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
                return UID::None();
            }
        }
    }

    UID id = UID::Create();
    Textures.insert({id, {texture, 1}});
    PathCache.insert({texture->GetPath(), id});

    return id;
}

Cast::UID Cast::TextureCacheRegistry::AddFromFile(const std::string& path, bool const flipUV)
{
    const auto itId = PathCache.find(path);
    if (itId != PathCache.end())
    {
        const auto itTex = Textures.find(itId->second);
        if (itTex != Textures.end())
        {
            return itTex->first;
        }
        else
        {
            LOG_CORE_ERROR(
                "Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
            return UID::None();
        }
    }

    LOG_CORE_TRACE("Loading uncached texture from file: {0}", path);
    auto texture = Ref<API::Texture::Texture>(
        API::Texture::Texture::Create(path, API::Texture::TextureFilter::LINEAR, flipUV));
    if (texture->GetError())
        return UID::None();

    UID id = UID::Create();
    Textures.insert({id, {texture, 1}});
    PathCache.insert({path, id});

    return id;
}

void Cast::TextureCacheRegistry::Remove(UID textureId, const bool force)
{
    const auto texIt = Textures.find(textureId);
    if (texIt == Textures.end())
    {
        return;
    }

    texIt->second.second--;

    if (texIt->second.second == 0 || force)
    {
        const std::string texturePath = texIt->second.first->GetPath();

        const auto pathIt = PathCache.find(texturePath);
        if (pathIt != PathCache.end() && pathIt->second == textureId)
            PathCache.erase(pathIt);

        for (auto it = ProxyIds.begin(); it != ProxyIds.end();)
        {
            if (it->second == textureId)
                it = ProxyIds.erase(it);
            else
                ++it;
        }

        Textures.erase(texIt);
        LOG_CORE_INFO("Texture with ID {} removed from registry", textureId);
    }
    else
    {
        LOG_CORE_TRACE("Texture with ID {} use count decreased to {}", textureId, texIt->second.second);
    }
}

void Cast::TextureCacheRegistry::Remove(std::string proxy, const bool force)
{
    const auto proxyIt = ProxyIds.find(proxy);
    if (proxyIt == ProxyIds.end())
    {
        return;
    }

    const UID textureId = proxyIt->second;
    ProxyIds.erase(proxyIt);

    Remove(textureId, force);
}

bool Cast::TextureCacheRegistry::AddProxy(UID textureId, const std::string& proxyName)
{
    ProxyIds.insert({proxyName, textureId});
    if (Textures.find(textureId) == Textures.end())
    {
        LOG_CORE_ERROR("Attempted to add proxy for non-existent texture with ID: {0}", textureId.GetID());
        return false;
    }

    return true;
}

Cast::Ref<API::Texture::Texture> Cast::TextureCacheRegistry::GetHandleByPath(const std::string& path)
{
    const auto it = ProxyIds.find(path);
    if (it != ProxyIds.end())
    {
        const auto itTex = Textures.find(it->second);
        if (itTex != Textures.end())
            return itTex->second.first;
        else
        {
            LOG_CORE_ERROR(
                "Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
            return nullptr;
        }
    }
    return nullptr;
}

Cast::Ref<API::Texture::Texture> Cast::TextureCacheRegistry::GetHandleByProxy(const std::string& proxy)
{
    const auto it = PathCache.find(proxy);
    if (it != PathCache.end())
    {
        const auto itTex = Textures.find(it->second);
        if (itTex != Textures.end())
            return itTex->second.first;
        else
        {
            LOG_CORE_ERROR(
                "Texture identifier found without matching texture. Consider removing it. Returning NULL handle.");
            return nullptr;
        }
    }
    return nullptr;
}

Cast::Ref<API::Texture::Texture> Cast::TextureCacheRegistry::GetHandle(const UID textureId)
{
    const auto it = Textures.find(textureId);
    if (it != Textures.end())
        return it->second.first;
    return nullptr;
}

Cast::UID Cast::TextureCacheRegistry::GetTextureIdByPath(const std::string& path) const
{
    const auto it = PathCache.find(path);
    if (it != PathCache.end())
        return it->second;

    return UID::None();
}

Cast::UID Cast::TextureCacheRegistry::GetTextureIdByProxy(const std::string& path) const
{
    const auto it = ProxyIds.find(path);
    if (it != ProxyIds.end())
        return it->second;

    return UID::None();
}

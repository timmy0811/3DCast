#include <castpch.h>
#include "Skybox.h"

bool Cast::Renderer::Skybox::AddCubemap(const std::string& name, const std::string& dirPath, const std::string& fileFormat)
{
    if (IsCubemapLoaded(name))
    {
        LOG_CORE_WARN("Cubemap already exists: {0}. Ignoring.", name);
        return false;
    }

    LOG_CORE_INFO("Adding cubemap {}", name);
    Cubemaps[name].reset(API::Texture::Cubemap::Create(dirPath, fileFormat));
    return true;
}

void Cast::Renderer::Skybox::RemoveCubemap(const std::string& name)
{
    if (!IsCubemapLoaded(name))
    {
        LOG_CORE_WARN("Cubemap not found: {0}. Ignoring.", name);
        return;
    }

    Cubemaps.erase(name);
}

void Cast::Renderer::Skybox::SetActiveCubemap(const std::string& name)
{
    if (!IsCubemapLoaded(name))
    {
        LOG_CORE_WARN("Cubemap not found for setting as active: {0}. Ignoring.", name);
        return;
    }

    LOG_CORE_INFO("Setting cubemap {} as active", name);
    ActiveCubemap = Cubemaps[name];
    ActiveCubemapName = name;
}

void Cast::Renderer::Skybox::BindCurrentCubemap(const unsigned int slot) const
{
    ActiveCubemap->Bind(slot);
}

void Cast::Renderer::Skybox::UnbindCurrentCubemap() const
{
    ActiveCubemap->Unbind();
}

void Cast::Renderer::Skybox::Render() const
{
    if (ActiveCubemap && Mode == RenderMode::Cubemap)
        ActiveCubemap->Render();
}

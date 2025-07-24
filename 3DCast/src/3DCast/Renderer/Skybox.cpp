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

void Cast::Renderer::Skybox::CalculateEnvironmentLightForCurrentCubemap()
{
    if (!ActiveCubemap)
    {
        LOG_CORE_WARN("Cannot calculate environment light: No active cubemap");
        return;
    }

    if (CubemapLightCache.find(ActiveCubemapName) != CubemapLightCache.end())
    {
        LOG_CORE_INFO("Using cached environment light for {}", ActiveCubemapName);
        return;
    }

    LOG_CORE_INFO("Calculating environment light for cubemap: {}", ActiveCubemapName);

    const API::Texture::Cubemap::Face brightestFace = ActiveCubemap->GetBrightestFace();

    EnvironmentLight light;
    light.specular = ActiveCubemap->GetFaceAverageColor(brightestFace);
    light.diffuse = ActiveCubemap->GetCubemapAverageColor();
    light.ambient = ActiveCubemap->GetCubemapAverageColorExceptFaces({brightestFace, API::Texture::Cubemap::NY});

    glm::vec2 brightestPoint = ActiveCubemap->GetFaceBrightestPoint(brightestFace);
    glm::vec3 direction;

    // Don't map to -1.f - 1.f due to projection warping
    brightestPoint = brightestPoint - 0.5f;

    switch (brightestFace) {
    case API::Texture::Cubemap::Face::PX: // +X face
        direction = glm::vec3(-0.5f, brightestPoint.y, brightestPoint.x);
        break;
    case API::Texture::Cubemap::Face::NX: // -X face
        direction = glm::vec3(0.5f, brightestPoint.y, -brightestPoint.x);
        break;
    case API::Texture::Cubemap::Face::PY: // +Y face
        direction = glm::vec3(-brightestPoint.x, -0.5f, -brightestPoint.y);
        break;
    case API::Texture::Cubemap::Face::NY: // -Y face
        direction = glm::vec3(-brightestPoint.x, 0.5f, brightestPoint.y);
        break;
    case API::Texture::Cubemap::Face::PZ: // +Z face
        direction = glm::vec3(-brightestPoint.x, brightestPoint.y, -0.5f);
        break;
    case API::Texture::Cubemap::Face::NZ: // -Z face
        direction = glm::vec3(brightestPoint.x, brightestPoint.y, 0.5f);
        break;
    default:
        LOG_CORE_ERROR("No cubemap face could be calculated as the  brightest");
    }

    direction = glm::normalize(direction);

    light.direction = direction;

    CubemapLightCache[ActiveCubemapName] = light;
}

void Cast::Renderer::Skybox::Render() const
{
    if (ActiveCubemap && Mode == RenderMode::Cubemap)
        ActiveCubemap->Render();
}

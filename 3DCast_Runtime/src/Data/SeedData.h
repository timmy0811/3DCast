#pragma once
#include "3DCast.h"

namespace Runtime
{
    inline void SetupApplicationSeed()
    {
        // This function is intended to set up seed data for the application.
        // It can be used to initialize default values, configurations, or any
        // other necessary data that the application needs to start with.
    }

    inline void SetupSceneSeed()
    {
#pragma region Material
        // Gold material
        Cast::Material goldMaterial;
        goldMaterial.isSystemMaterial = true;
        goldMaterial.shaderObject.diffuseColor = {1.0f, 0.84f, 0.0f};
        goldMaterial.shaderObject.specularColor = {0.5f, 0.4f, 0.0f};
        goldMaterial.shaderObject.shininess = 65.0f;
        goldMaterial.shaderObject.reflectance = 0.85f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(goldMaterial), "Gold");

        // Copper material
        Cast::Material copperMaterial;
        copperMaterial.isSystemMaterial = true;
        copperMaterial.shaderObject.diffuseColor = {0.85f, 0.53f, 0.1f};
        copperMaterial.shaderObject.specularColor = {0.475f, 0.32f, 0.27f};
        copperMaterial.shaderObject.shininess = 55.0f;
        copperMaterial.shaderObject.reflectance = 0.75f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(copperMaterial), "Copper");

        // Plastic material
        Cast::Material plasticMaterial;
        plasticMaterial.isSystemMaterial = true;
        plasticMaterial.shaderObject.diffuseColor = {0.9f, 0.9f, 0.9f};
        plasticMaterial.shaderObject.specularColor = {0.25f, 0.25f, 0.25f};
        plasticMaterial.shaderObject.shininess = 25.0f;
        plasticMaterial.shaderObject.reflectance = 0.4f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(plasticMaterial), "Plastic");

        // Marble material
        Cast::Material marbleMaterial;
        marbleMaterial.isSystemMaterial = true;
        marbleMaterial.shaderObject.diffuseColor = {0.9f, 0.9f, 0.9f};
        marbleMaterial.shaderObject.specularColor = {0.4f, 0.4f, 0.4f};
        marbleMaterial.shaderObject.shininess = 50.0f;
        marbleMaterial.shaderObject.reflectance = 0.6f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(marbleMaterial), "Marble");

        // Glass material
        Cast::Material glassMaterial;
        glassMaterial.isSystemMaterial = true;
        glassMaterial.shaderObject.diffuseColor = {0.1f, 0.1f, 0.1f};
        glassMaterial.shaderObject.specularColor = {0.5f, 0.5f, 0.5f};
        glassMaterial.shaderObject.shininess = 96.0f;
        glassMaterial.shaderObject.reflectance = 0.95f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(glassMaterial), "Glass");

        // Rubber material
        Cast::Material rubberMaterial;
        rubberMaterial.isSystemMaterial = true;
        rubberMaterial.shaderObject.diffuseColor = {0.3f, 0.3f, 0.3f};
        rubberMaterial.shaderObject.specularColor = {0.01f, 0.01f, 0.01f};
        rubberMaterial.shaderObject.shininess = 8.0f;
        rubberMaterial.shaderObject.reflectance = 0.15f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(rubberMaterial), "Rubber");

        // Concrete material
        Cast::Material concreteMaterial;
        concreteMaterial.isSystemMaterial = true;
        concreteMaterial.shaderObject.diffuseColor = {0.6f, 0.6f, 0.6f};
        concreteMaterial.shaderObject.specularColor = {0.05f, 0.05f, 0.05f};
        concreteMaterial.shaderObject.shininess = 3.0f;
        concreteMaterial.shaderObject.reflectance = 0.25f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(concreteMaterial), "Concrete");

        // Leather material
        Cast::Material leatherMaterial;
        leatherMaterial.isSystemMaterial = true;
        leatherMaterial.shaderObject.diffuseColor = {0.4f, 0.2f, 0.1f};
        leatherMaterial.shaderObject.specularColor = {0.05f, 0.025f, 0.015f};
        leatherMaterial.shaderObject.shininess = 15.0f;
        leatherMaterial.shaderObject.reflectance = 0.3f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(leatherMaterial), "Leather");

        // Fabric material
        Cast::Material fabricMaterial;
        fabricMaterial.isSystemMaterial = true;
        fabricMaterial.shaderObject.diffuseColor = {0.5f, 0.5f, 0.8f};
        fabricMaterial.shaderObject.specularColor = {0.025f, 0.025f, 0.025f};
        fabricMaterial.shaderObject.shininess = 2.0f;
        fabricMaterial.shaderObject.reflectance = 0.1f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(fabricMaterial), "Fabric");

        // Glossy Red material
        Cast::Material glossyRedMaterial;
        glossyRedMaterial.isSystemMaterial = true;
        glossyRedMaterial.shaderObject.diffuseColor = {0.8f, 0.1f, 0.1f};
        glossyRedMaterial.shaderObject.specularColor = {0.7f, 0.3f, 0.3f};
        glossyRedMaterial.shaderObject.shininess = 85.0f;
        glossyRedMaterial.shaderObject.reflectance = 0.9f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(glossyRedMaterial), "GlossyRed");

        // Light Blue material
        Cast::Material lightBlueMaterial;
        lightBlueMaterial.isSystemMaterial = true;
        lightBlueMaterial.shaderObject.diffuseColor = {0.3f, 0.6f, 1.0f};
        lightBlueMaterial.shaderObject.specularColor = {0.5f, 0.7f, 0.9f};
        lightBlueMaterial.shaderObject.shininess = 80.0f;
        lightBlueMaterial.shaderObject.reflectance = 0.85f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(lightBlueMaterial), "LightBlue");
#pragma endregion
    }

    inline void SetupPreviewSceneSeed()
    {
#pragma region Entity
        const Cast::Ref<Cast::Entity> lightEntity = Cast::Shared.ActiveScene->CreateEntity("Light");
        auto& lightComp = lightEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), &Cast::Shared.ActiveScene.value());
        lightComp.CastShadows = true;
        Cast::Shared.ActiveScene->SetActiveShadowDirectionalLight(lightEntity->GetEntityHandle());
        auto& transformComp = lightEntity->GetComponent<Cast::Component::TransformComponent>();
        transformComp.translation.y = 3.f;
        transformComp.UpdateTransformMatrix();
        transformComp.UpdateOnGPUMem();
        transformComp.UpdateBBox();

        Cast::Create::Cube("Cube_1", Cast::Shared.ActiveScene.value());
#pragma endregion
    }
}

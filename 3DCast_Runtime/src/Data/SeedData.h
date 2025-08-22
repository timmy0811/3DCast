#pragma once
#include "3DCast.h"

namespace Runtime
{
    inline void SetupSeedData()
    {
        // This function is intended to set up seed data for the application.
        // It can be used to initialize default values, configurations, or any
        // other necessary data that the application needs to start with.

#pragma region Entity
        const Cast::Ref<Cast::Entity> lightEntity = Cast::Shared.ActiveScene->CreateEntity("Light");
        lightEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), Cast::Shared.ActiveScene);
        auto& transformComp = lightEntity->GetComponent<Cast::Component::TransformComponent>();
        transformComp.translation.y = 3.f;
        transformComp.UpdateTransformMatrix();
        transformComp.UpdateOnGPUMem();
        transformComp.UpdateBBox();

        Cast::Create::Cube("Cube_1", Cast::Shared.ActiveScene.get());
#pragma endregion

#pragma region Material
        // Gold material
        Cast::Material goldMaterial;
        goldMaterial.shaderObject.diffuseColor = {1.0f, 0.84f, 0.0f};
        goldMaterial.shaderObject.specularColor = {1.0f, 0.8f, 0.0f};
        goldMaterial.shaderObject.shininess = 65.0f;
        goldMaterial.shaderObject.reflectance = 0.85f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(goldMaterial), "Gold");
        
        // Copper material
        Cast::Material copperMaterial;
        copperMaterial.shaderObject.diffuseColor = {0.85f, 0.53f, 0.1f};
        copperMaterial.shaderObject.specularColor = {0.95f, 0.64f, 0.54f};
        copperMaterial.shaderObject.shininess = 55.0f;
        copperMaterial.shaderObject.reflectance = 0.75f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(copperMaterial), "Copper");
        
        // Plastic material
        Cast::Material plasticMaterial;
        plasticMaterial.shaderObject.diffuseColor = {0.9f, 0.9f, 0.9f};
        plasticMaterial.shaderObject.specularColor = {0.5f, 0.5f, 0.5f};
        plasticMaterial.shaderObject.shininess = 25.0f;
        plasticMaterial.shaderObject.reflectance = 0.4f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(plasticMaterial), "Plastic");
        
        // Marble material
        Cast::Material marbleMaterial;
        marbleMaterial.shaderObject.diffuseColor = {0.9f, 0.9f, 0.9f};
        marbleMaterial.shaderObject.specularColor = {0.8f, 0.8f, 0.8f};
        marbleMaterial.shaderObject.shininess = 50.0f;
        marbleMaterial.shaderObject.reflectance = 0.6f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(marbleMaterial), "Marble");
        
        // Glass material
        Cast::Material glassMaterial;
        glassMaterial.shaderObject.diffuseColor = {0.1f, 0.1f, 0.1f};
        glassMaterial.shaderObject.specularColor = {1.0f, 1.0f, 1.0f};
        glassMaterial.shaderObject.shininess = 96.0f;
        glassMaterial.shaderObject.reflectance = 0.95f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(glassMaterial), "Glass");
        
        // Rubber material
        Cast::Material rubberMaterial;
        rubberMaterial.shaderObject.diffuseColor = {0.3f, 0.3f, 0.3f};
        rubberMaterial.shaderObject.specularColor = {0.02f, 0.02f, 0.02f};
        rubberMaterial.shaderObject.shininess = 8.0f;
        rubberMaterial.shaderObject.reflectance = 0.15f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(rubberMaterial), "Rubber");
        
        // Concrete material
        Cast::Material concreteMaterial;
        concreteMaterial.shaderObject.diffuseColor = {0.6f, 0.6f, 0.6f};
        concreteMaterial.shaderObject.specularColor = {0.1f, 0.1f, 0.1f};
        concreteMaterial.shaderObject.shininess = 3.0f;
        concreteMaterial.shaderObject.reflectance = 0.25f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(concreteMaterial), "Concrete");
        
        // Leather material
        Cast::Material leatherMaterial;
        leatherMaterial.shaderObject.diffuseColor = {0.4f, 0.2f, 0.1f};
        leatherMaterial.shaderObject.specularColor = {0.1f, 0.05f, 0.03f};
        leatherMaterial.shaderObject.shininess = 15.0f;
        leatherMaterial.shaderObject.reflectance = 0.3f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(leatherMaterial), "Leather");
        
        // Fabric material
        Cast::Material fabricMaterial;
        fabricMaterial.shaderObject.diffuseColor = {0.5f, 0.5f, 0.8f};
        fabricMaterial.shaderObject.specularColor = {0.05f, 0.05f, 0.05f};
        fabricMaterial.shaderObject.shininess = 2.0f;
        fabricMaterial.shaderObject.reflectance = 0.1f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(fabricMaterial), "Fabric");
        
        // Ceramic material
        Cast::Material ceramicMaterial;
        ceramicMaterial.shaderObject.diffuseColor = {1.0f, 1.0f, 1.0f};
        ceramicMaterial.shaderObject.specularColor = {0.7f, 0.7f, 0.7f};
        ceramicMaterial.shaderObject.shininess = 70.0f;
        ceramicMaterial.shaderObject.reflectance = 0.7f;
        Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(ceramicMaterial), "Ceramic");
#pragma endregion
    }
}

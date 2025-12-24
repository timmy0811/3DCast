#pragma once

#include "3DCast/Core.h"
#include <API/advanced/CascadedShadowMap.h>
#include <vendor/glm/glm.hpp>
#include <array>

namespace Cast {
    class Scene;
    namespace Renderer {
        class Camera;
    }
}

namespace Cast::Renderer {

    class CSMProcessor {
    public:
        CSMProcessor();
        ~CSMProcessor() = default;

        void Init(const std::array<glm::ivec2, API::Advanced::CascadedShadowMap::CascadeCount>& cascadeSizes);
        void OnRender(const Scene& scene, const Camera& camera);

        [[nodiscard]] const glm::mat4* GetCascadeLightVP() const { return CascadeLightVP; }
        [[nodiscard]] const float* GetCascadeSplits() const { return CascadeSplits; }
        [[nodiscard]] const glm::vec3& GetShadowLightDir() const { return ShadowLightDir; }
        [[nodiscard]] bool HasShadowMapData() const { return HasShadowMapDataValue; }

        [[nodiscard]] Ref<API::Advanced::CascadedShadowMap> GetShadowMap() const { return CascadedShadowMap; }

    private:
        Ref<API::Advanced::CascadedShadowMap> CascadedShadowMap;

        static constexpr int CascadeCount = API::Advanced::CascadedShadowMap::CascadeCount;
        glm::mat4 CascadeLightVP[CascadeCount]{};
        float CascadeSplits[CascadeCount]{};
        glm::vec3 ShadowLightDir{0.0f, -1.0f, 0.0f};
        bool HasShadowMapDataValue = false;
    };

}

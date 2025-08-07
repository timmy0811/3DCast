#pragma once

#include <vendor/glm/glm.hpp>
#include "3DCast/Misc/Structs.h"

namespace Math
{
    inline glm::vec3 MousePositionToRayDirection(const ImVec2& mousePos, const ImVec2& viewportMin, const ImVec2& viewportMax,
                                                 const glm::mat4& projection, const glm::mat4& view)
    {
        const ImVec2 viewportPos = viewportMin;
        const auto viewportSize = ImVec2(
            viewportMax.x - viewportMin.x,
            viewportMax.y - viewportMin.y
        );

        const float x = ((mousePos.x - viewportPos.x) / viewportSize.x) * 2.0f - 1.0f;
        const float y = 1.0f - ((mousePos.y - viewportPos.y) / viewportSize.y) * 2.0f;

        const auto rayClip = glm::vec4(x, y, -1.0f, 1.0f);
        const glm::mat4 invProj = glm::inverse(projection);
        glm::vec4 rayEye = invProj * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        const glm::mat4 invView = glm::inverse(view);
        const glm::vec4 rayWorld = invView * rayEye;
        const glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

        return rayDirection;
    }

    inline bool RayIntersectsAABB(const glm::vec3& origin, const glm::vec3& dir, const Cast::BoundingBox& box, float& maxDist)
    {
        const glm::vec3& minB = box.Min();
        const glm::vec3& maxB = box.Max();

        float t0 = 0.0f;
        float t1 = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i)
        {
            if (std::abs(dir[i]) < 1e-8f)
            {
                if (origin[i] < minB[i] || origin[i] > maxB[i])
                    return false;
            }
            else
            {
                const float invD = 1.0f / dir[i];
                float tNear = (minB[i] - origin[i]) * invD;
                float tFar  = (maxB[i] - origin[i]) * invD;

                if (tNear > tFar) std::swap(tNear, tFar);

                t0 = tNear  > t0 ? tNear  : t0;
                t1 = tFar   < t1 ? tFar   : t1;

                if (t0 > t1)
                    return false;
            }
        }

        if (t1 < 0.0f)
            return false;

        maxDist = t0 >= 0.0f ? t0 : t1;
        return true;
    }
}
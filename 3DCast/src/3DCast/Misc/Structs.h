#pragma once

#include <vendor/glm/glm.hpp>

namespace Cast
{
    struct BoundingBox
    {
        BoundingBox()
        {
            SetSize(1.0f);
        }

        inline void SetSize(const float s)
        {
            const float r = s * 0.5f;
            Radius_ = r;
            Min_ = Center_ - glm::vec3(r, r, r);
            Max_ = Center_ + glm::vec3(r, r, r);
        }

        inline void SetCenter(const glm::vec3& center)
        {
            Center_ = center;
            Min_ = Center_ - glm::vec3(Radius_, Radius_, Radius_);
            Max_ = Center_ + glm::vec3(Radius_, Radius_, Radius_);
        }

        [[nodiscard]] inline const glm::vec3& Min() const {return Min_; }
        [[nodiscard]] inline const glm::vec3& Max() const {return Max_; }
        [[nodiscard]] inline const glm::vec3& Center() const {return Center_; }
        [[nodiscard]] inline float Radius() const {return Radius_; }

    private:
        glm::vec3 Min_{0.0f, 0.0f, 0.0f};
        glm::vec3 Max_{0.0f, 0.0f, 0.0f};

        glm::vec3 Center_{0.0f, 0.0f, 0.0f};
        float Radius_ = 0.0f;

        friend class Serialization::SceneSerializer;
    };
}
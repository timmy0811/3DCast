#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Component/Typedefinition.h"
#include "3DCast/Misc/Icon.h"

#include <string>

#define SAMELINE_WIDGET_OFFSET_1 (ImGui::GetWindowWidth() * 0.33333f - 15.f)
#define SAMELINE_WIDGET_OFFSET_2 (ImGui::GetWindowWidth() * 0.66666f - 15.f)
#define SAMELINE_WIDGET_OFFSET_HALF (ImGui::GetWindowWidth() * 0.5f - 15.f)
#define DUMMYSPACE_AFTER_COMPONENT 15.f

namespace Cast
{
    class Entity;
}

namespace Cast::Component
{
    struct Component
    {
        // Important!: To avoid entt calling the destructor when moving components,
        // implement your own copy and move constructors/operators in derived classes.
        //
        // Component(Component&& other) noexcept = delete;
        // Component& operator=(Component&& other) noexcept = delete;

        virtual ~Component() = default;

        virtual UIResponse OnImGuiRender() { return {}; };

        virtual void Print()
        {
        };

        virtual void OnAfterEntitySetBehaviour()
        {
        };

        void SetEntity(Ref<Entity> entity) { EntityNode = entity; }

        static std::string GetName()
        {
            return "Component";
        }

    protected:
        Ref<Entity> EntityNode = nullptr;

        friend class Entity;
    };
}

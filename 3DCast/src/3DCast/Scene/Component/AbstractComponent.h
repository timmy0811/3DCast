#pragma once

#include "3DCast/Core.h"
#include "3DCast/Scene/Component/Typedefinition.h"
#include "3DCast/Misc/Icon.h"

#include <string>

#define GUIWIN_WSIXTH (ImGui::GetWindowWidth() * 0.16667f)
#define GUIWIN_WQUARTER (ImGui::GetWindowWidth() * 0.25f)
#define GUIWIN_WTHIRD (ImGui::GetWindowWidth() * 0.33333f)
#define GUIWIN_WHalf (ImGui::GetWindowWidth() * 0.5f)

#define GUIWIN_ELEMENT_PADDING 15.f
#define GUIWIN_ENDELEMENT_PADDING 25.f
#define DUMMYSPACE_AFTER_COMPONENT 8.f

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

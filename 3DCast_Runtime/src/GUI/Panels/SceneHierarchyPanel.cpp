#include "SceneHierarchyPanel.h"

#include "Data/SharedEditorData.h"
#include "GUI/Modal/ComponentListModal.h"

#include <3DCast/Scene/Component/Component.h>
#include <3DCast/Scene/Component/Typedefinition.h>
#include <3DCast/Misc/Icon.h>

#include <imgui.h>

Runtime::GUI::SceneHierarchyPanel::SceneHierarchyPanel(const Cast::Ref<Cast::Scene>& scene)
{
	SetContext(scene);
}

void Runtime::GUI::SceneHierarchyPanel::SetContext(const Cast::Ref<Cast::Scene>& scene)
{
	Context = scene;
}

void Runtime::GUI::SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin(ICON_FA_FOLDER_TREE " Scene Hierarchy");

	const ImVec2 windowSize = ImGui::GetWindowSize();
	const ImVec2 availableRegion = ImGui::GetContentRegionAvail();
	const ImGuiStyle& style = ImGui::GetStyle();

	constexpr float buttonHeight = 20.0f;
	constexpr float padding = 10.0f;
	const float childHeight = availableRegion.y - (buttonHeight + padding);

	// Begin scrollable region
	ImGui::BeginChild("EntityList", ImVec2(0, childHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

	for (const auto& [handle, entity] : Context->GetEntityDescriptors())
	{
		if (!entity->IsChild())
			DrawEntityNode(entity);
	}

	ImGui::EndChild();

	if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
	{
		SelectionContext = nullptr;
		Context->SetEditorSelectionContext(nullptr);
	}

	ImGui::SetCursorPosY(windowSize.y - buttonHeight - padding - 3.f);
	if (ImGui::Button(ICON_FA_PLUS " New Entity", {ImGui::GetContentRegionAvail().x * 0.75f - 5.f, 0.f}))
	{
		Context->CreateEntity("New Entity", false);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("An entity that can carry multiple components to give it functionality");
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	ImGui::BeginDisabled(!SelectionContext);
	if (ImGui::Button(ICON_FA_TRASH_CAN, {ImGui::GetContentRegionAvail().x - 5.f, 0.f}))
	{
		RemoveEntity();
	}
	ImGui::EndDisabled();

	ImGui::End();

	static bool drawAddComponentModal = false;

	ImGui::Begin(ICON_FA_SLIDERS " Properties");
	ImGui::Checkbox("Render View", &Cast::Shared.ActiveScene->GetInRenderView());
	ImGui::SameLine();
	ImGui::BeginDisabled(!SelectionContext);
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f);
	if (ImGui::Button("Remove Entity"))
	{
		RemoveEntity();
	}
	ImGui::EndDisabled();
	ImGui::Separator();

	if (SelectionContext)
	{
		DrawComponents(SelectionContext);
		ImGui::Separator();

		const float size = ImGui::CalcTextSize(ICON_FA_SHAPES " Add Component").x + style.FramePadding.x * 2.0f;

		const float off = (availableRegion.x - size) * 0.5f;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button(ICON_FA_SHAPES " Add Component"))
		{
			GUI::ComponentList::Reset();
			drawAddComponentModal = true;
		}
	}

	ImGui::End();

	if (drawAddComponentModal)
	{
		const auto res = ComponentList::OnImGuiRender();
		drawAddComponentModal = res == ComponentList::ModalResult::None;

		if (res == ComponentList::ModalResult::Success)
		{
			const auto selections = ComponentList::GetSelection();
			for (size_t i = 0; i < Cast::Component::AddableComponentIds.size(); i++)
			{
				if (selections[i])
					DispatchComponent(i);
			}
		}
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawEntityNode(Cast::Ref<Cast::Entity> entity)
{
	const std::string& tag = entity->GetComponent<Cast::Component::TagComponent>().Tag;

	const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
		((SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
		(entity->HasChildren()
			 ? ImGuiTreeNodeFlags_OpenOnArrow
			 : ImGuiTreeNodeFlags_Leaf);

	ImGui::PushID((void*)(uint64_t)(uint32_t)*entity);

	const bool isOpen = ImGui::TreeNodeEx("##EntityNode", flags, "%s", tag.c_str());

	if (ImGui::IsItemClicked())
	{
		SelectionContext = entity;
		Context->SetEditorSelectionContext(entity);
	}

	ImGui::PopID();

	if (isOpen)
	{
		for (const auto& child : entity->GetChildren())
		{
			DrawEntityNode(child);
		}

		ImGui::TreePop();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawComponents(Cast::Ref<Cast::Entity> entity) const
{
	auto& registry = Context->GetRegistry();
	auto entityHandle = entity->GetEntityHandle();

	// Directly calling Components is faster than using CallBacks -> Append for new components
	// Alternative:
	// for (const auto& ImGuiCallback : Context->GetComponentImGuiCallbacks())
	// 	auto [action, component] = ImGuiCallback(Context->GetRegistry(), entity->GetEntityHandle());
	if (entity->HasComponent<Cast::Component::TagComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::TagComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::TagComponent>();
	}

	if (entity->HasComponent<Cast::Component::TransformComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::TransformComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::TransformComponent>();
	}

	if (entity->HasComponent<Cast::Component::CameraComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::CameraComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::CameraComponent>();
	}

	if (entity->HasComponent<Cast::Component::LightComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::LightComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::LightComponent>();
	}

	if (entity->HasComponent<Cast::Component::MeshComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::MeshComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::MeshComponent>();
	}

	if (entity->HasComponent<Cast::Component::CustomMeshComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::CustomMeshComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::CustomMeshComponent>();
	}

	if (entity->HasComponent<Cast::Component::MaterialComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::MaterialComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::MaterialComponent>();
	}

	if (entity->HasComponent<Cast::Component::ShaderComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::ShaderComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::ShaderComponent>();
	}

	if (entity->HasComponent<Cast::Component::PBRMaterialComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::PBRMaterialComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::PBRMaterialComponent>();
	}

	if (entity->HasComponent<Cast::Component::RasterizableComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::RasterizableComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::RasterizableComponent>();
	}

	if (entity->HasComponent<Cast::Component::PBRComponent>())
	{
		auto& component = entity->GetComponent<Cast::Component::PBRComponent>();
		if (component.OnImGuiRender().action == Cast::Component::UIResponse::Remove)
			entity->RemoveComponent<Cast::Component::PBRComponent>();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DispatchComponent(const int id) const
{
	switch (id)
	{
	case 0:
		SelectionContext->AddComponents<Cast::Component::TransformComponent>();
		break;
	case 1:
		SelectionContext->AddComponents<Cast::Component::CameraComponent>();
		break;
	case 2:
		SelectionContext->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLight(), Cast::Shared.ActiveScene);
		break;
	case 3:
		SelectionContext->AddComponents<Cast::Component::MeshComponent>();
		break;
	case 4:
		SelectionContext->AddComponents<Cast::Component::CustomMeshComponent>();
		break;
	case 5:
		SelectionContext->AddComponents<Cast::Component::MaterialComponent>();
		break;
	case 6:
		LOG_CORE_WARN("Script component not implemented yet");
		break;
	case 7:
		LOG_CORE_WARN("Behaviour component not implemented yet");
		break;
	case 8:
		LOG_CORE_WARN("Rigid-Body component not implemented yet");
		break;
	case 9:
		LOG_CORE_WARN("Collider component not implemented yet");
		break;
	case 10:
		LOG_CORE_WARN("Audio component not implemented yet");
		break;
	case 11:
		LOG_CORE_WARN("Paricle component not implemented yet");
		break;
	case 12:
		LOG_CORE_WARN("Animation component not implemented yet");
		break;
	case 13:
		LOG_CORE_WARN("UI component not implemented yet");
		break;
	case 14:
		SelectionContext->AddComponents<Cast::Component::ShaderComponent>();
		break;
	case 15:
		SelectionContext->AddComponents<Cast::Component::PBRMaterialComponent>();
		break;
	case 16:
		SelectionContext->AddComponents<Cast::Component::RasterizableComponent>();
		break;
	case 17:
		SelectionContext->AddComponents<Cast::Component::PBRComponent>();
		break;
	default:
		LOG_CORE_WARN("Trying to add component with unknown id.");
	}
}

void Runtime::GUI::SceneHierarchyPanel::RemoveEntity()
{
	Context->RemoveEntityBulkOptimized(SelectionContext);
	if (SelectionContext->IsChild())
		SelectionContext->GetParent()->RemoveChild(SelectionContext);

	SelectionContext = nullptr;
	Context->SetEditorSelectionContext(nullptr);
}

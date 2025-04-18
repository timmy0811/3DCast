#include "SceneHierarchyPanel.h"

#include "Data/SharedEditorData.h"
#include "GUI/Modal/ComponentListModal.h"

#include <3DCast/Scene/Component/Component.h>
#include <3DCast/Scene/Component/Typedefinition.h>
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
	ImGui::Begin("Scene Hierarchy");

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 availableRegion = ImGui::GetContentRegionAvail();
	ImGuiStyle& style = ImGui::GetStyle();

	float buttonHeight = 20.0f;
	float padding = 10.0f;
	float childHeight = availableRegion.y - (buttonHeight + padding);

	// Begin scrollable region
	ImGui::BeginChild("EntityList", ImVec2(0, childHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

	for (Cast::Ref<Cast::Entity> handle : Context->GetEntityDescriptors())
	{
		if (!handle->IsChild())
			DrawEntityNode(handle);
	}

	ImGui::EndChild();

	if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
		SelectionContext = {};

	ImGui::SetCursorPosY(windowSize.y - buttonHeight - padding - 3.f);
	if (ImGui::Button("New Entity", { ImGui::GetContentRegionAvail().x * 0.75f - 5.f, 0.f })) {
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
	if (ImGui::Button("Remove", { ImGui::GetContentRegionAvail().x - 5.f, 0.f })) {
		Context->RemoveEntity(*SelectionContext);
		SelectionContext = {};
	}
	ImGui::EndDisabled();

	ImGui::End();

	static bool drawAddComponentModal = false;

	ImGui::Begin("Properties");
	ImGui::Checkbox("Render View", &Cast::Shared.ActiveScene->GetInRenderView());
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f);
	if (ImGui::Button("Remove Entity")) {
		Context->RemoveEntity(*SelectionContext);
		SelectionContext = {};
	}
	ImGui::Separator();

	if (SelectionContext)
	{
		DrawComponents(SelectionContext);
		ImGui::Separator();

		float size = ImGui::CalcTextSize("Add Component").x + style.FramePadding.x * 2.0f;

		float off = (availableRegion.x - size) * 0.5f;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::Button("Add Component")) {
			GUI::ComponentList::Reset();
			drawAddComponentModal = true;
		}
	}

	ImGui::End();

	if (drawAddComponentModal)
	{
		auto res = GUI::ComponentList::OnImGuiRender();
		drawAddComponentModal = res == GUI::ComponentList::ModalResult::None;

		if (res == GUI::ComponentList::ModalResult::Success)
		{
			auto selections = GUI::ComponentList::GetSelection();
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
	std::string& tag = entity->GetComponent<Cast::Component::TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
		((SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
		(entity->HasChildren() ? ImGuiTreeNodeFlags_OpenOnArrow
			: ImGuiTreeNodeFlags_Leaf);

	ImGui::PushID((void*)(uint64_t)(uint32_t)*entity);

	bool isOpen = ImGui::TreeNodeEx("##EntityNode", flags, "%s", tag.c_str());

	if (ImGui::IsItemClicked())
		SelectionContext = entity;

	ImGui::PopID();

	if (isOpen)
	{
		for (auto& child : entity->GetChildren())
			DrawEntityNode(child);

		ImGui::TreePop();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawComponents(Cast::Ref<Cast::Entity> entity)
{
	for (auto ImGuiCallback : Context->GetComponentImGuiCallbacks())
	{
		Cast::Component::UIResponse res = ImGuiCallback(Context->GetRegistry(), entity->GetEntityHandle());
		if (res.action == Cast::Component::UIResponse::Remove)
		{
			switch (res.component)
			{
			case Cast::Component::Type::Transform:
				entity->RemoveComponent<Cast::Component::TransformComponent>();
				break;
			case Cast::Component::Type::Camera:
				entity->RemoveComponent<Cast::Component::CameraComponent>();
				break;
			case Cast::Component::Type::Light:
				entity->RemoveComponent<Cast::Component::LightComponent>();
				break;
			case Cast::Component::Type::Mesh:
				entity->RemoveComponent<Cast::Component::MeshComponent>();
				break;
			case Cast::Component::Type::CustomMesh:
				entity->RemoveComponent<Cast::Component::CustomMeshComponent>();
				break;
			case Cast::Component::Type::Material:
				entity->RemoveComponent<Cast::Component::MaterialComponent>();
				break;
			case Cast::Component::Type::Script:
				LOG_CORE_WARN("Script component not implemented yet");
				break;
			case Cast::Component::Type::Behaviour:
				LOG_CORE_WARN("Behaviour component not implemented yet");
				break;
			case Cast::Component::Type::RigidBody:
				LOG_CORE_WARN("Rigid-Body component not implemented yet");
				break;
			case Cast::Component::Type::Collider:
				LOG_CORE_WARN("Collider component not implemented yet");
				break;
			case Cast::Component::Type::Audio:
				LOG_CORE_WARN("Audio component not implemented yet");
				break;
			case Cast::Component::Type::Particle:
				LOG_CORE_WARN("Paricle component not implemented yet");
				break;
			case Cast::Component::Type::Animation:
				LOG_CORE_WARN("Animation component not implemented yet");
				break;
			case Cast::Component::Type::UI:
				LOG_CORE_WARN("UI component not implemented yet");
				break;
			case Cast::Component::Type::Shader:
				entity->RemoveComponent<Cast::Component::ShaderComponent>();
				break;
			case Cast::Component::Type::Tag:
				entity->RemoveComponent<Cast::Component::TagComponent>();
				break;
			case Cast::Component::Type::PBRMat:
				entity->RemoveComponent<Cast::Component::PBRMaterialComponent>();
				break;
			case Cast::Component::Type::Raster:
				entity->RemoveComponent<Cast::Component::RasterizableComponent>();
				break;
			case Cast::Component::Type::PBR:
				entity->RemoveComponent<Cast::Component::PBRComponent>();
				break;
			default:
				LOG_CORE_WARN("Trying to remove component with unknown id.");
				break;
			}
		}
	}
}

void Runtime::GUI::SceneHierarchyPanel::DispatchComponent(int id)
{
	switch (id) {
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
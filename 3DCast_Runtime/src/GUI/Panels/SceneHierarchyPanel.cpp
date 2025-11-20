#include "SceneHierarchyPanel.h"

#include "Data/SharedEditorData.h"
#include "GUI/Modal/ComponentListModal.h"

#include <3DCast/Scene/Component/Component.h>
#include <3DCast/Scene/Component/Typedefinition.h>
#include <3DCast/Misc/Icon.h>

#include <imgui.h>

#include "imgui_internal.h"

Runtime::GUI::SceneHierarchyPanel::SceneHierarchyPanel()
{
	EditorContext.SelectedEntity = &SelectionContext;
}

void Runtime::GUI::SceneHierarchyPanel::SetSelectionContext(const Cast::Ref<Cast::Entity>& entity)
{
	SelectionContext = entity;
}

void Runtime::GUI::SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin(ICON_FA_FOLDER_TREE " Scene Hierarchy");

	const ImVec2 windowSize = ImGui::GetWindowSize();
	const ImGuiStyle& style = ImGui::GetStyle();

	if (Cast::Shared.ActiveScene)
	{
		const ImVec2 availableRegion = ImGui::GetContentRegionAvail();
		constexpr float buttonHeight = 20.0f;
		constexpr float padding = 10.0f;
		const float childHeight = availableRegion.y - (buttonHeight + padding);

		// Begin scrollable region
		ImGui::BeginChild("EntityList", ImVec2(0, childHeight), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

		for (const auto& [handle, entity] :Cast::Shared.ActiveScene->GetEntityDescriptors())
		{
			if (!entity->IsChild())
				DrawEntityNode(entity);
		}

		ImGui::EndChild();

		if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
		{
			SelectionContext = nullptr;
			Cast::Shared.ActiveScene->SetEditorSelectionContext(nullptr);
		}

		ImGui::SetCursorPosY(windowSize.y - buttonHeight - padding - 3.f);

		constexpr float cellDiv = 1.f / 5.f;
		const float cellWidth = (ImGui::GetContentRegionAvail().x - 10.f) * cellDiv;

		if (ImGui::Button("New Entity", {cellWidth * 3.f, 0.f}))
		{
			Cast::Shared.ActiveScene->CreateEntity("New Entity", false);
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("An entity that can carry multiple components to give it functionality");
			ImGui::EndTooltip();
		}

		ImGui::SameLine();

		DrawTemplateSelector(cellWidth);

		ImGui::SameLine();

		ImGui::BeginDisabled(!SelectionContext);
		if (ImGui::Button(ICON_FA_TRASH_CAN, {cellWidth, 0.f}))
		{
			RemoveEntity();
		}
		ImGui::EndDisabled();
	}
	else
	{
		const ImVec2 availableRegion = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPos(ImVec2(availableRegion.x * 0.5, availableRegion.y * 0.5f));
		ImGui::Text("No scene opened");
	}

	ImGui::End();

	static bool drawAddComponentModal = false;

	ImGui::Begin(ICON_FA_SLIDERS " Properties");

	if (Cast::Shared.ActiveScene)
	{
		const ImVec2 availableRegion = ImGui::GetContentRegionAvail();
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
	}
	else
	{
		const ImVec2 availableRegion = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPos(ImVec2(availableRegion.x * 0.5, availableRegion.y * 0.5f));
		ImGui::Text("No scene opened");
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
					DispatchComponent((int)i);
			}
		}
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawTemplateSelector(float width)
{
	if (ImGui::Button(ICON_FA_WAND_MAGIC_SPARKLES, {width, 0.f}))
	{
		ImGui::OpenPopup("EntityTemplatesPopup");
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Create an entity from a predefined template");
		ImGui::EndTooltip();
	}

	if (ImGui::BeginPopup("EntityTemplatesPopup"))
	{
		ImGui::Text("Templates");
		ImGui::Separator();

		if (ImGui::BeginMenu("Primitive"))
		{
			if (ImGui::MenuItem("Cube")) CreateEntityFromTemplate(Template::Cube);
			if (ImGui::MenuItem("Plane")) CreateEntityFromTemplate(Template::Plane);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional Light")) CreateEntityFromTemplate(Template::DirLight);
			if (ImGui::MenuItem("Point Light")) CreateEntityFromTemplate(Template::PointLight);
			if (ImGui::MenuItem("Spot Light")) CreateEntityFromTemplate(Template::SpotLight);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Mesh"))
		{
			if (ImGui::MenuItem("Custom Mesh")) CreateEntityFromTemplate(Template::CustomMesh);
			if (ImGui::MenuItem("Model")) CreateEntityFromTemplate(Template::Model);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Effect"))
		{
			if (ImGui::MenuItem("Particle System")) CreateEntityFromTemplate(Template::ParticleSystem);
			if (ImGui::MenuItem("Decal")) CreateEntityFromTemplate(Template::Decal);

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawEntityNode(const Cast::Ref<Cast::Entity>& entity)
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
		Cast::Shared.ActiveScene->SetEditorSelectionContext(entity);
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

void EndButtonDropDown()
{
	ImGui::PopStyleColor(3);
	ImGui::EndPopup();
}

void Runtime::GUI::SceneHierarchyPanel::DrawComponents(const Cast::Ref<Cast::Entity>& entity)
{
	// Directly calling Components is faster than using CallBacks -> Append for new components
	// Alternative:
	//auto& registry =Cast::Shared.ActiveScene->GetRegistry();
	//auto entityHandle = entity->GetEntityHandle();
	// for (const auto& ImGuiCallback :Cast::Shared.ActiveScene->GetComponentImGuiCallbacks())
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
		SelectionContext->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), &Cast::Shared.ActiveScene.value());
		break;
	case 3:
		SelectionContext->AddComponents<Cast::Component::MeshComponent>(Cast::MeshNodeType::Root);
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
		LOG_CORE_WARN("Particle component not implemented yet");
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
	if (EditorContext.EnvironmentLightEntity && *SelectionContext.get() == *EditorContext.EnvironmentLightEntity)
	{
		LOG_CORE_WARN("You cannot remove the Environment Light directly. Use the Skybox Panel to disable it.");
		return;
	}

	Cast::Shared.ActiveScene->RemoveEntityBulkOptimized(SelectionContext);
	if (SelectionContext->IsChild())
		SelectionContext->GetParent()->RemoveChild(SelectionContext);

	SelectionContext = nullptr;
	Cast::Shared.ActiveScene->SetEditorSelectionContext(nullptr);
}

void Runtime::GUI::SceneHierarchyPanel::CreateEntityFromTemplate(const Template templateName)
{
	Cast::Ref<Cast::Entity> entity = nullptr;
	if (templateName != Template::Cube && templateName != Template::Plane)
		entity =Cast::Shared.ActiveScene->CreateEntity(TemplateToString(templateName), false);

	switch (templateName)
	{
		case Template::Cube:
			Cast::Create::Cube("Cube", Cast::Shared.ActiveScene.value());
			break;
		case Template::Plane:
			Cast::Create::Plane("Plane", Cast::Shared.ActiveScene.value());
			break;
	case Template::DirLight:
			entity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), &Cast::Shared.ActiveScene.value());
			break;
		case Template::SpotLight:
			entity->AddComponents<Cast::Component::LightComponent>(Cast::SpotLightShaderObject(), &Cast::Shared.ActiveScene.value());
			break;
		case Template::PointLight:
			entity->AddComponents<Cast::Component::LightComponent>(Cast::PointLightShaderObject(), &Cast::Shared.ActiveScene.value());
			break;
		case Template::CustomMesh:
			entity->AddComponents<Cast::Component::CustomMeshComponent>();
			entity->AddComponents<Cast::Component::MaterialComponent>();
			break;
	case Template::Model:
			entity->AddComponents<Cast::Component::MeshComponent>(Cast::MeshNodeType::Root);
			break;
		case Template::Camera:
			entity->AddComponents<Cast::Component::CameraComponent>();
			break;
		default: ;
	}

	SelectionContext = entity;
	Cast::Shared.ActiveScene->SetEditorSelectionContext(entity);
}

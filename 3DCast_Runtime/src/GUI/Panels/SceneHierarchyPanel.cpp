#include "SceneHierarchyPanel.h"

#include "Data/SharedEditorData.h"

#include <imgui.h>

#include <3DCast/Scene/Components.h>

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

	for (auto entity : Context->GetRegistry().view<entt::entity>())
	{
		Cast::Entity handle{ entity , Context.get() };
		DrawEntityNode(handle);
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		SelectionContext = {};

	ImGui::End();

	ImGui::Begin("Properties");
	ImGui::Checkbox("Render View", &EditorContext.ActiveScene->GetInRenderView());
	ImGui::Separator();

	if (SelectionContext)
	{
		DrawComponents(SelectionContext);
	}

	ImGui::End();
}

void Runtime::GUI::SceneHierarchyPanel::DrawEntityNode(Cast::Entity entity)
{
	std::string& tag = entity.GetComponent<Cast::Component::TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ((SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool isOpen = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		SelectionContext = entity;
	}

	if (isOpen)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		isOpen = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
		if (isOpen)
			ImGui::TreePop();
		ImGui::TreePop();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawComponents(Cast::Entity entity)
{
	for (auto ImGuiCallback : Context->GetComponentImGuiCallbacks())
	{
		ImGuiCallback(Context->GetRegistry(), entity.GetEntityHandle());
	}
}
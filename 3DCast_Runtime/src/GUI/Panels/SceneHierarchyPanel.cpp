#include "SceneHierarchyPanel.h"

#include "Data/SharedEditorData.h"
#include <3DCast/Scene/Component/Component.h>

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
			DrawEntityNode(*handle);
	}

	ImGui::EndChild();

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		SelectionContext = {};

	ImGui::SetCursorPosY(windowSize.y - buttonHeight - padding);
	if (ImGui::Button("New Entity", { ImGui::GetContentRegionAvail().x, buttonHeight })) {
		Context->CreateEntity("New Entity");
	}

	ImGui::End();

	ImGui::Begin("Properties");
	ImGui::Checkbox("Render View", &Cast::Shared.ActiveScene->GetInRenderView());
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f);
	if (ImGui::Button("Remove Entity")) {
		Context->RemoveEntity(SelectionContext);
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
			LOG_CORE_INFO("Add Component");
		}
	}

	ImGui::End();
}

void Runtime::GUI::SceneHierarchyPanel::DrawEntityNode(Cast::Entity entity)
{
	std::string& tag = entity.GetComponent<Cast::Component::TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ((SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | (entity.HasChildren() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf);

	bool isOpen = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		SelectionContext = entity;
	}

	/*if (ImGui::IsItemHovered()) {
		ImGui::SameLine(ImGui::GetWindowWidth() - 78.0);
		ImGui::Button("remove", { 70.f, 12.f });
	}*/

	if (isOpen)
	{
		//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		//isOpen = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
		for (auto& child : entity.GetChildren())
		{
			DrawEntityNode(*child);
		}
		ImGui::TreePop();
		//ImGui::TreePop();
	}
}

void Runtime::GUI::SceneHierarchyPanel::DrawComponents(Cast::Entity entity)
{
	for (auto ImGuiCallback : Context->GetComponentImGuiCallbacks())
	{
		ImGuiCallback(Context->GetRegistry(), entity.GetEntityHandle());
	}
}
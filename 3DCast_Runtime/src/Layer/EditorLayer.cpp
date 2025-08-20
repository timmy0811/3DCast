#include "EditorLayer.h"

#include "Data/SharedEditorData.h"
#include "Config.h"
#include "GUI/Panels/EventConsole.h"
#include "GUI/Theme.h"
#include "GUI/Modal/KeymapModal.h"
#include "Application/KeymapLayout.h"

#include <3DCast/ImGui/TempElements/TempGuiElementCollection.h>
#include <3DCast/Math/Collision.h>

#include <imgui_internal.h>
#include <ctime>
#include <memory>

#include "GUI/Panels/DiagnosticsPanel.h"

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
	ViewportPbr = Runtime::PBRViewport(this);
	ViewportRasterization = Runtime::RasterizationViewport(this);
}

void EditorLayer::OnAttach()
{
	Cast::Application::Get().GetWindow().SetRawMouseInput(true);

	Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::NewDark);

	Cast::Shared.ActiveScene = Cast::CreateRef<Cast::Scene>();
	const Cast::Ref<Cast::Entity> cameraEntity = Cast::Shared.ActiveScene->CreateEntity("Camera");

	Runtime::EditorContext.ActiveCamera = std::make_shared<Cast::Renderer::PerspectiveCamera>(
		glm::radians(90.f), 1.5f, 0.1f, 100.f);
	Runtime::EditorContext.ActiveCamera->SetPosition(glm::vec3(4.0f, 3.0f, 4.0f));
	Runtime::EditorContext.ActiveCamera->SetRotation(glm::vec3(-20.0f, -135.0f, 0.0f));
	cameraEntity->AddComponents<Cast::Component::CameraComponent>(*Runtime::EditorContext.ActiveCamera);

	ViewportPbr.Init();
	ViewportRasterization.Init();

	Cast::DeferredSamplerStoreInstance.InitAfterDriverSetup();

	SkyboxPanel.SetContext(Cast::Shared.ActiveScene);
	SkyboxPanel.SetSkybox(&Runtime::EditorContext.Skybox);

	SampleContent();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(const Cast::Timestep ts)
{
	const float CameraSpeedCorrected = CameraSpeed * ts;
	glm::vec3 cameraPosition = Runtime::EditorContext.ActiveCamera->GetPosition();
	if (ViewportRasterization.IsViewportFocused())
	{
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_L))
		{
			cameraPosition -= Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
		}
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_R))
		{
			cameraPosition += Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
		}
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_UP))
		{
			cameraPosition.y += CameraSpeedCorrected;
		}
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_DOWN))
		{
			cameraPosition.y -= CameraSpeedCorrected;
		}
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_FW))
		{
			cameraPosition += Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
		}
		if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_BW))
		{
			cameraPosition -= Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
		}

		Runtime::EditorContext.ActiveCamera->SetPosition(cameraPosition);
	}

	ViewportPbr.OnUpdate(ts);
	ViewportRasterization.OnUpdate(ts);

	Cast::Shared.ActiveScene->OnUpdate();
	Runtime::EditorContext.Skybox.SetActiveCubemapViewProjectionMatrix(Runtime::EditorContext.ActiveCamera->GetViewMat(),
		Runtime::EditorContext.ActiveCamera->GetProjectionMat());

	Render();

	SceneHierarchyPanel.SetContext(Cast::Shared.ActiveScene);

	DeltaTime = ts.GetSeconds();
}

void EditorLayer::OnImGuiRender()
{
#pragma region DOCKSPACE
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	const bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	const ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
#pragma endregion

#pragma region MENU_BAR
	static bool showRasterSettings = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit")) Cast::Application::Get().Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Editor"))
		{
			if (ImGui::MenuItem("Keymap"))
			{
				Runtime::GUI::Keymap::Open();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Rasterizer Settings"))
			{
				showRasterSettings = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("World"))
		{
			if (ImGui::MenuItem("Configure Skybox"))
			{
				SkyboxPanel.Open();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Extras"))
		{
			if (ImGui::MenuItem("Find Viewport"))
			{
				ViewportRasterization.FindWindow();
			}

			if (ImGui::BeginMenu("Theme"))
			{
				if (ImGui::MenuItem("Azure Light"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Azure);
				}
				if (ImGui::MenuItem("NewDark (Default)"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::NewDark);
				}
				if (ImGui::MenuItem("Mocha"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Mocha);
				}
				if (ImGui::MenuItem("Glass"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Glass);
				}
				if (ImGui::MenuItem("MaterialYou"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::MaterialYou);
				}
				if (ImGui::MenuItem("Dark"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Dark);
				}
				if (ImGui::MenuItem("Darker"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Darker);
				}
				if (ImGui::MenuItem("Fluent"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::Fluent);
				}
				if (ImGui::MenuItem("FluentLight"))
				{
					Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::FluentLight);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
#pragma endregion

#pragma region WINDOW_SETTINGS_VIEW
	if (showRasterSettings)
	{
		ImGui::OpenPopup(ICON_FA_GEARS " Raster Settings");
		showRasterSettings = false;
	}

	if (ImGui::BeginPopupModal(ICON_FA_GEARS " Raster Settings"))
	{
		ImGui::Text("Parallax Scale");
		ImGui::SameLine();
		ImGui::DragFloat("Parallax Scale", &Runtime::EditorContext.ViewSettings.ParallaxScale, 0.002f, 0.0f, 1.5f);

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
#pragma endregion

	ViewportRasterization.OnImGuiRender();
	ViewportPbr.OnImGuiRender();

	SceneHierarchyPanel.OnImGuiRender();
	SkyboxPanel.OnImGuiRender();
	TerminalPanel.OnImGuiRender();

	Runtime::GUI::DiagnosticsPanel::OnImGuiRender(DeltaTime);
	Runtime::GUI::EventConsole::OnImGuiRender();
	Runtime::GUI::Keymap::OnImGuiRender();
	Cast::GUI::TempGuiElementCollection::OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));
	dispatcher.Dispatch<Cast::MouseButtonPressedEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMousePressed));

	ViewportPbr.OnEvent(e);
	ViewportRasterization.OnEvent(e);
}

bool EditorLayer::OnMouseScrolled(const Cast::MouseScrolledEvent& e)
{
	if (ViewportRasterization.IsViewportHovered())
	{
		if (e.GetYOffset() < 0.f) CameraSpeed *= 0.9f;
		else if (e.GetYOffset() > 0.f) CameraSpeed *= 1.1f;
	}

	return false;
}

bool EditorLayer::OnMousePressed(const Cast::MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == CAST_MOUSE_BUTTON_LEFT &&
		ViewportRasterization.IsViewportHovered() &&
		!ViewportRasterization.IsGizmoScaleU() &&
		!Runtime::RasterizationViewport::IsHoveringGizmo())
	{
		const Cast::Ref<Cast::Renderer::Camera> camera = Runtime::EditorContext.ActiveCamera;

		const glm::vec3 rayDir = Math::MousePositionToRayDirection(
			ImGui::GetMousePos(),
			ViewportRasterization.GetViewportBounds()[0],
			ViewportRasterization.GetViewportBounds()[1],
			camera->GetProjectionMat(),
			camera->GetViewMat()
		);

		const Cast::Ref<Cast::Entity> selection = Cast::Shared.ActiveScene->RaycastSelection(
			camera->GetPosition(), rayDir);

		SceneHierarchyPanel.SetSelectionContext(selection);
		Cast::Shared.ActiveScene->SetEditorSelectionContext(selection);
	}

	return false;
}

void EditorLayer::Render()
{
	ViewportRasterization.OnRender();
	ViewportPbr.OnRender();
}

void EditorLayer::SampleContent()
{
	const Cast::Ref<Cast::Entity> lightEntity = Cast::Shared.ActiveScene->CreateEntity("Light");
	lightEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLightShaderObject(), Cast::Shared.ActiveScene);
	auto& transformComp = lightEntity->GetComponent<Cast::Component::TransformComponent>();
	transformComp.translation.y = 3.f;
	transformComp.UpdateTransformMatrix();
	transformComp.UpdateOnGPUMem();
	transformComp.UpdateBBox();

	Cast::Material material;
	material.shaderObject.diffuseColor = {0.7f, 0.2f, 1.0f};
	material.shaderObject.specularColor = {0.0f, 0.0f, 1.0f};
	Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(material), "Wood");

	Cast::Material material2;
	material2.shaderObject.diffuseColor = {0.1f, 0.8f, 1.0f};
	material2.shaderObject.specularColor = {0.0f, 0.0f, 1.0f};
	Cast::MaterialCacheRegistryInstance.AddProxy(Cast::MaterialCacheRegistryInstance.Add(material2), "Steel");

	Cast::Material material3;
	material3.shaderObject.diffuseColor = {0.3f, 0.2f, 0.1f};
	material3.shaderObject.specularColor = {0.0f, 0.0f, 1.0f};
	Cast::MaterialCacheRegistryInstance.Add(material3);

	Cast::Create::Cube("Cube_1", Cast::Shared.ActiveScene.get());
}

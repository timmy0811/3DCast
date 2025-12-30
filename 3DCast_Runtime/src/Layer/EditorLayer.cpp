#include "EditorLayer.h"

#include "Data/SharedEditorData.h"
#include "Config.h"
#include "GUI/Panels/EventConsole.h"
#include "GUI/Theme.h"
#include "GUI/Modal/KeymapModal.h"
#include "Application/KeymapLayout.h"

#include <3DCast/Gui/TempElements/TempGuiElementCollection.h>
#include <3DCast/Math/Collision.h>

#include <imgui_internal.h>
#include <memory>

#include "3DCast/Gui/TempElements/Elements/NotificationModal.h"
#include "Data/SeedData.h"
#include "GUI/Modal/Menus.h"
#include "GUI/Panels/DiagnosticsPanel.h"

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), Serializer(nullptr)
{
	ViewportPbr = Runtime::PBRViewport(this);
	ViewportRasterization = Runtime::RasterizationViewport(this);
}

void EditorLayer::OnAttach()
{
	Cast::Application::Get().GetWindow().SetRawMouseInput(true);

	Runtime::GUI::Theme::ApplyTheme(Runtime::GUI::Theme::NewDark);

	Cast::Shared.ActiveScene.emplace();
	const Cast::Ref<Cast::Entity> cameraEntity = Cast::Shared.ActiveScene->CreateEntity("Camera", ICON_FA_VIDEO);

	auto& cameraComponent = cameraEntity->AddComponents<Cast::Component::CameraComponent>();
	cameraComponent.Camera = new Cast::Renderer::PerspectiveCamera(glm::radians(85.f), 1.5f, 0.1f, 100.f);
	cameraComponent.ownsCamera = true;
	dynamic_cast<Cast::Renderer::PerspectiveCamera*>(cameraComponent.Camera)->SetFOV(85.0f);

	Cast::Renderer::Camera* cam = cameraEntity->GetComponent<Cast::Component::CameraComponent>().Camera;
	cam->SetPosition(glm::vec3(4.0f, 3.0f, 4.0f));
	cam->SetRotation(glm::vec3(-20.0f, -135.0f, 0.0f));
	Runtime::EditorContext.ActiveCamera.emplace(cam);

	ViewportPbr.Init();
	ViewportRasterization.Init();

	Cast::DeferredSamplerStoreInstance.InitAfterDriverSetup();

	Runtime::SetupSceneSeed();
	Runtime::SetupPreviewSceneSeed();

	SkyboxPanel.SetSkybox(&Runtime::EditorContext.Skybox);

	FogPanel.SetOnChangeCallback([]() {
		Runtime::RasterizationViewport::UpdateFogUniforms();
	});
	Runtime::RasterizationViewport::UpdateFogUniforms();

	Serializer.SetScene(&Cast::Shared.ActiveScene.value());
	Serializer.SetSkyboxCallback(&Runtime::EditorContext.Skybox);
	Serializer.SetActiveCameraCallback(&Runtime::EditorContext.ActiveCamera);
	Serializer.SetUseEnvironmentLightingCallback(SkyboxPanel.GetIsUsingEnvironmentMappingRef());
	Serializer.SetEnvironmentLightEntityCallback(SkyboxPanel.GetEnvironmentLightEntityRef());
	Serializer.SetEnvironmentLightComponentCallback(SkyboxPanel.GetEnvironmentLightComponentRef());
	Serializer.SetRenderModeCallback(SkyboxPanel.GetRenderModeRef());
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(const Cast::Timestep ts)
{
	if (Cast::Shared.ActiveScene)
	{
		const float CameraSpeedCorrected = CameraSpeed * ts;
		glm::vec3 cameraPosition = Runtime::EditorContext.ActiveCamera.value()->GetPosition();
		if (ViewportRasterization.IsViewportFocused())
		{
			if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_L))
			{
				cameraPosition -= Runtime::EditorContext.ActiveCamera.value()->GetRight() * CameraSpeedCorrected;
			}
			if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_R))
			{
				cameraPosition += Runtime::EditorContext.ActiveCamera.value()->GetRight() * CameraSpeedCorrected;
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
				cameraPosition += Runtime::EditorContext.ActiveCamera.value()->GetForward() * CameraSpeedCorrected;
			}
			if (Runtime::Application::Keymap::IsActionActive(Runtime::Application::KEY_ACTION::CAMERA_BW))
			{
				cameraPosition -= Runtime::EditorContext.ActiveCamera.value()->GetForward() * CameraSpeedCorrected;
			}

			Runtime::EditorContext.ActiveCamera.value()->SetPosition(cameraPosition);
		}

		ViewportPbr.OnUpdate(ts);
		ViewportRasterization.OnUpdate(ts);

		Cast::Shared.ActiveScene->OnUpdate();
		Runtime::EditorContext.Skybox.SetActiveCubemapViewProjectionMatrix(Runtime::EditorContext.ActiveCamera.value()->GetViewMat(),
			Runtime::EditorContext.ActiveCamera.value()->GetProjectionMat());

		Render();
	}

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
	bool showRasterSettings = false;
	bool showViewportSettings = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Scene", "Ctrl+O"))
			{
				const std::string sceneFile = Cast::Util::OpenFileDialogue(SceneSavePath.empty() ? (std::string(DATA_DIR) + "scene/") : Cast::Util::ExtractDirectory(SceneSavePath),
					"3DCast Scene", "3dc");
				if (!sceneFile.empty())
				{
					SceneSavePath = sceneFile;
					SceneFilename = Cast::Util::ExtractFilename(sceneFile);

					if (Cast::Shared.ActiveScene)
						CloseScene();

					Cast::Shared.ActiveScene.emplace();
					Runtime::SetupSceneSeed();
					Serializer.SetScene(&Cast::Shared.ActiveScene.value());
					if (Serializer.Deserialize(SceneSavePath))
					{
						SkyboxPanel.UpdateEnvironmentLight();
						ParentWindow->SetTitle(Cast::Util::ExtractFilename(SceneFilename, false).c_str());
					}
				}
			}

			ImGui::BeginDisabled(!Cast::Shared.ActiveScene || SceneSavePath.empty());
			if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save Scene", "Ctrl+S"))
			{
				Serializer.Serialize(SceneSavePath);
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!Cast::Shared.ActiveScene);
			if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save Scene As", "Ctrl+Shift+S"))
			{
				const std::string sceneFile = Cast::Util::SaveFileDialogue(SceneSavePath.empty() ? (std::string(DATA_DIR) + "scene/") : Cast::Util::ExtractDirectory(SceneSavePath),
					SceneFilename.empty() ? "new_scene.3dc" : SceneFilename, "3DCast Scene", "3dc");
				if (!sceneFile.empty())
				{
					SceneFilename = Cast::Util::ExtractFilename(sceneFile);
					SceneSavePath = sceneFile;
					Serializer.Serialize(sceneFile);
					ParentWindow->SetTitle(Cast::Util::ExtractFilename(SceneFilename, false).c_str());
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_FOLDER_CLOSED "  Close Scene"))
			{
				CloseScene();
			}
			ImGui::EndDisabled();

			if (ImGui::MenuItem(ICON_FA_XMARK "  Exit", "Ctrl+Esc"))
				Cast::Application::Get().Close();

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

			if (ImGui::MenuItem("Viewport Resolution"))
			{
				showViewportSettings = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("World"))
		{
			if (ImGui::MenuItem("Skybox"))
			{
				SkyboxPanel.Open();
			}

			if (ImGui::MenuItem("Fog"))
			{
				FogPanel.Open();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Diagnostics"))
			{
				Runtime::GUI::DiagnosticsPanel::Open();
			}
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

	ImGui::End();
#pragma endregion

	if (Cast::Shared.ActiveScene)
	{
		ViewportRasterization.OnImGuiRender();
		ViewportPbr.OnImGuiRender();
	}

	SceneHierarchyPanel.OnImGuiRender();
	SkyboxPanel.OnImGuiRender();
	FogPanel.OnImGuiRender();
	TerminalPanel.OnImGuiRender();

	Runtime::GUI::DiagnosticsPanel::OnImGuiRender(DeltaTime);
	Runtime::GUI::EventConsole::OnImGuiRender();
	Runtime::GUI::Keymap::OnImGuiRender();
	Runtime::GUI::PopupRasterSettings(showRasterSettings);
	Runtime::GUI::PopupViewportSettings(showViewportSettings, [this]() {
		ViewportRasterization.OnResizeCallback();
	});
	Cast::GUI::TempGuiElementCollection::OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	if (Cast::Shared.ActiveScene)
	{
		Cast::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));
		dispatcher.Dispatch<Cast::MouseButtonPressedEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMousePressed));

		ViewportPbr.OnEvent(e);
		ViewportRasterization.OnEvent(e);
	}
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
		const Cast::Renderer::Camera* camera = Runtime::EditorContext.ActiveCamera.value();

		const auto imageBounds = ViewportRasterization.GetImageDisplayBounds();
		const glm::vec3 rayDir = Math::MousePositionToRayDirection(
			ImGui::GetMousePos(),
			imageBounds[0],
			imageBounds[1],
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

void EditorLayer::CloseScene() const
{
	Cast::ResetSceneContext();
	Cast::Shared.ActiveScene->Shutdown();
	Cast::Shared.ActiveScene.reset();

	ParentWindow->ResetToDefaultTitle();
}

void EditorLayer::Render()
{
	ViewportRasterization.OnRender();
	ViewportPbr.OnRender();
}

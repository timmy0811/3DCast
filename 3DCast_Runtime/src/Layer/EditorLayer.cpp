#include "EditorLayer.h"

#include "Data/SharedEditorData.h"
#include "Config.h"

#include "GUI/Panels/EventConsole.h"
#include "GUI/Theme.h"

#include <3DCast/ImGui/TempElements/TempGuiElementCollection.h>

#include <imgui_internal.h>
#include <ctime>

#include <memory>

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
	Runtime::EditorContext.ActiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	cameraEntity->AddComponents<Cast::Component::CameraComponent>(*Runtime::EditorContext.ActiveCamera);

	ViewportPbr.Init();
	ViewportRasterization.Init();

	Cast::SamplerRegistry.InitAfterDriverSetup();

	// Sample Content
	SampleContent();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(const Cast::Timestep ts)
{
	const float CameraSpeedCorrected = CameraSpeed * ts;
	glm::vec3 cameraPosition = Runtime::EditorContext.ActiveCamera->GetPosition();
	if (Cast::Input::IsKeyPressed(CAST_KEY_A))
	{
		cameraPosition -= Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_D))
	{
		cameraPosition += Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_UP))
	{
		cameraPosition.y += CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_DOWN))
	{
		cameraPosition.y -= CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_W))
	{
		cameraPosition += Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_S))
	{
		cameraPosition -= Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
	}

	Runtime::EditorContext.ActiveCamera->SetPosition(cameraPosition);

	ViewportPbr.OnUpdate(ts);
	ViewportRasterization.OnUpdate(ts);

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
	static bool showParallaxSettings = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Cast::Application::Get().Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Parallax Settings"))
			{
				showParallaxSettings = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Extras"))
		{
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
			else if (ImGui::MenuItem("Find Viewport"))
			{
				ViewportRasterization.FindWindow();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
#pragma endregion

#pragma region WINDOW_SETTINGS_VIEW
	if (showParallaxSettings)
	{
		ImGui::OpenPopup("ParallaxSettings");
		showParallaxSettings = false;
	}

	if (ImGui::BeginPopupModal("ParallaxSettings"))
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

#pragma region WINDOW_DIAGNOSTICS
	ImGui::Begin("Diagnostics");

	const int fps = (int)(1.0f / DeltaTime);
	static int maxFPS = 0;
	static int minFPS = fps;
	maxFPS = std::max(maxFPS, fps);
	minFPS = std::min(minFPS, fps);

	static int maxFPSDisplay = fps;
	static int minFPSDisplay = fps;

	static time_t startTime = time(nullptr);
	if (difftime(time(nullptr), startTime) >= 1)
	{
		startTime = time(nullptr);
		maxFPSDisplay = maxFPS;
		minFPSDisplay = minFPS;
		maxFPS = 0;
		minFPS = fps;
	}

	ImGui::Text("FPS: %d", fps);
	ImGui::Text("Max FPS: %d", maxFPSDisplay);
	ImGui::Text("Min FPS: %d", minFPSDisplay);
	ImGui::Text("Frametime: %.2f", DeltaTime * 1000.f);

	ImGui::Text("Camera Position: %f, %f, %f", Runtime::EditorContext.ActiveCamera->GetPosition().x,
	            Runtime::EditorContext.ActiveCamera->GetPosition().y,
	            Runtime::EditorContext.ActiveCamera->GetPosition().z);

	ImGui::End();
#pragma endregion

	ViewportRasterization.OnImGuiRender();
	ViewportPbr.OnImGuiRender();

	SceneHierarchyPanel.OnImGuiRender();
	Runtime::GUI::EventConsole::OnImGuiRender();
	Cast::GUI::TempGuiElementCollection::OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));

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

void EditorLayer::Render()
{
	ViewportRasterization.OnRender();
	// ViewportPbr.OnRender();
}

void EditorLayer::SampleContent()
{
	// Light
	const Cast::Ref<Cast::Entity> lightEntity = Cast::Shared.ActiveScene->CreateEntity("Light");
	lightEntity->AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLight(), Cast::Shared.ActiveScene);

	//Cast::Ref<Cast::Entity> meshEntity = Cast::Shared.ActiveScene->CreateEntity("Mesh");
	//meshEntity->AddComponents<Cast::Component::MeshComponent>();
	//Cast::Create::Cube("Cube_1", Runtime::EditorContext.ActiveScene.get());

	Cast::Create::Cube("Cube_1", Cast::Shared.ActiveScene.get());
}

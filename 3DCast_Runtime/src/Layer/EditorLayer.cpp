#include "EditorLayer.h"

#include "Data/SharedEditorData.h"
#include "Config.h"
#include "GUI/ImGuiStyle.h"

#include <imgui_internal.h>
#include <time.h>

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
{
	ViewportPbr = Runtime::PBRViewport(this);
	ViewportRasterization = Runtime::RasterizationViewport(this);
}

void EditorLayer::OnAttach()
{
	Cast::Application::Get().GetWindow().SetRawMouseInput(true);

	Runtime::SetupImGuiStyle(true, 0.3f);

	Runtime::EditorContext.ActiveScene = Cast::CreateRef<Cast::Scene>();
	Cast::Entity cameraEntity = Runtime::EditorContext.ActiveScene->CreateEntity("Camera");

	Runtime::EditorContext.ActiveCamera.reset(new Cast::Renderer::PerspectiveCamera(glm::radians(90.f), 1.5f, 0.1f, 100.f));
	Runtime::EditorContext.ActiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	cameraEntity.AddComponents<Cast::Component::CameraComponent>(*Runtime::EditorContext.ActiveCamera);

	ViewportPbr.Init();
	ViewportRasterization.Init();

	Cast::g_TextureManager.InitAfterDriverSetup();

	// Sample Content
	SampleContent();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Cast::Timestep ts)
{
	float CameraSpeedCorrected = CameraSpeed * ts;
	glm::vec3 cameraPosition = Runtime::EditorContext.ActiveCamera->GetPosition();
	if (Cast::Input::IsKeyPressed(CAST_KEY_A)) {
		cameraPosition -= Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_D)) {
		cameraPosition += Runtime::EditorContext.ActiveCamera->GetRight() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_UP)) {
		cameraPosition.y += CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_DOWN)) {
		cameraPosition.y -= CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_W)) {
		cameraPosition += Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
	}
	if (Cast::Input::IsKeyPressed(CAST_KEY_S)) {
		cameraPosition -= Runtime::EditorContext.ActiveCamera->GetForward() * CameraSpeedCorrected;
	}

	Runtime::EditorContext.ActiveCamera->SetPosition(cameraPosition);

	ViewportPbr.OnUpdate(ts);
	ViewportRasterization.OnUpdate(ts);

	Render();

	SceneHierarchyPanel.SetContext(Runtime::EditorContext.ActiveScene);

	DeltaTime = ts.GetSeconds();
}

void EditorLayer::OnImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Cast::Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();

	ImGui::Begin("Diagnostics");

	int fps = (int)(1.0f / DeltaTime);
	static int maxFPS = 0;
	static int minFPS = fps;
	maxFPS = std::max(maxFPS, fps);
	minFPS = std::min(minFPS, fps);

	static int maxFPSDisplay = fps;
	static int minFPSDisplay = fps;

	static time_t startTime = time(0);
	if (difftime(time(0), startTime) >= 1)
	{
		startTime = time(0);
		maxFPSDisplay = maxFPS;
		minFPSDisplay = minFPS;
		maxFPS = 0;
		minFPS = fps;
	}

	ImGui::Text("FPS: %d", fps);
	ImGui::Text("Max FPS: %d", maxFPSDisplay);
	ImGui::Text("Min FPS: %d", minFPSDisplay);
	ImGui::Text("Frametime: %.2f", DeltaTime * 1000.f);

	ImGui::Text("Camera Position: %f, %f, %f", Runtime::EditorContext.ActiveCamera->GetPosition().x, Runtime::EditorContext.ActiveCamera->GetPosition().y, Runtime::EditorContext.ActiveCamera->GetPosition().z);

	ImGui::End();

	ViewportRasterization.OnImGuiRender();
	ViewportPbr.OnImGuiRender();

	SceneHierarchyPanel.OnImGuiRender();
}

void EditorLayer::OnEvent(Cast::Event& e)
{
	Cast::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Cast::MouseScrolledEvent>(CAST_BIND_EVENT_FUNC(EditorLayer::OnMouseScrolled));

	ViewportPbr.OnEvent(e);
	ViewportRasterization.OnEvent(e);
}

bool EditorLayer::OnMouseScrolled(Cast::MouseScrolledEvent& e)
{
	if (e.GetYOffset() < 0.f) CameraSpeed *= 0.9f;
	else if (e.GetYOffset() > 0.f) CameraSpeed *= 1.1f;

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
	Cast::Entity lightEntity = Runtime::EditorContext.ActiveScene->CreateEntity("Light");
	lightEntity.AddComponents<Cast::Component::LightComponent>(Cast::DirectionalLight(), Runtime::EditorContext.ActiveScene);

	Cast::Create::Cube("Cube_1", Runtime::EditorContext.ActiveScene.get());
}